package cz.vut.fit.pis.bakery.api;

import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.time.LocalDate;
import java.time.ZoneId;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;

import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.json.Json;
import javax.json.JsonArray;
import javax.json.JsonArrayBuilder;
import javax.json.JsonObject;
import javax.json.JsonObjectBuilder;
import javax.ws.rs.Consumes;
import javax.ws.rs.GET;
import javax.ws.rs.NotFoundException;
import javax.ws.rs.POST;
import javax.ws.rs.PUT;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriInfo;
import javax.ws.rs.core.Response.Status;

import cz.vut.fit.pis.bakery.back.Authorized;
import cz.vut.fit.pis.bakery.back.UserRole;
import cz.vut.fit.pis.bakery.data.ProductionPlan;
import cz.vut.fit.pis.bakery.data.ProductionPlanItem;
import cz.vut.fit.pis.bakery.data.Material;
import cz.vut.fit.pis.bakery.data.MaterialStorage;
import cz.vut.fit.pis.bakery.data.Pastry;
import cz.vut.fit.pis.bakery.data.PastryMaterial;
import cz.vut.fit.pis.bakery.data.PastryOrderItem;
import cz.vut.fit.pis.bakery.service.ProductionPlanItemManager;
import cz.vut.fit.pis.bakery.service.ProductionPlanManager;
import cz.vut.fit.pis.bakery.service.MaterialStorageManager;
import cz.vut.fit.pis.bakery.service.PastryManager;
import cz.vut.fit.pis.bakery.service.PastryOrderManager;

/**
 * @author Martin Fišer
 */
@Stateless
@Path("/productionPlans")
public class ProductionPlans {
	@EJB
	private ProductionPlanManager productionPlanManager;
	@EJB
	private ProductionPlanItemManager productionPlanItemManager;
	@EJB
	private PastryManager pastryManager;
	@EJB
	private PastryOrderManager pastryOrderManager;
	@EJB
	private MaterialStorageManager materialStorageManager;
	@Context
	private UriInfo context;

	public ProductionPlans()
	{
	}

	@Path("/list")
	@GET
	@Authorized(role = UserRole.Baker)
	@Produces(MediaType.APPLICATION_JSON)
	public JsonArray getJson()
	{
		List<ProductionPlan> plans = productionPlanManager.findAll();

		JsonArrayBuilder arrayBuilder = Json.createArrayBuilder();
		for (int i = 0; i < plans.size(); i++) {
			arrayBuilder.add(serializePlan(plans.get(i)));
		}

		return arrayBuilder.build();
	}

	@Path("/{id}")
	@GET
	@Authorized(role = UserRole.Baker)
	@Produces(MediaType.APPLICATION_JSON)
	public Response getJsonSingle(@PathParam("id") int id)
	{
		ProductionPlan p = productionPlanManager.find(id);

		if (p != null)
			return Response.ok(serializePlan(p)).build();
		else
			return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such plan\"}").build();
	}

	@Path("/productionDate/{date}")
	@GET
	@Authorized(role = UserRole.Baker)
	@Produces(MediaType.APPLICATION_JSON)
	public Response getJsonDeliveryDate(@PathParam("date") String dateStr)
	{
		Date date;
		try {
			date = new SimpleDateFormat("yyyy-MM-dd").parse(dateStr);
		} catch (ParseException e) {
			date = null;
		}
		if (date == null)
			return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such date\"}").build();

		ProductionPlan p = productionPlanManager.findByProductionDate(date);

		if (p != null)
			return Response.ok(serializePlan(p)).build();
		else
			return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such plan\"}").build();
	}

	@Path("/create/{date}")
	@GET
	@Authorized(role = UserRole.Manager)
	@Produces(MediaType.APPLICATION_JSON)
	public Response getJsonCreate(@PathParam("date") String dateStr)
	{
		Date productionDate;
		try {
			productionDate = new SimpleDateFormat("yyyy-MM-dd").parse(dateStr);
		} catch (ParseException e) {
			productionDate = null;
		}
		if (productionDate == null)
			return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such date\"}").build();

		ProductionPlan plan = productionPlanManager.findByProductionDate(productionDate);
		if (plan == null) {
			plan = new ProductionPlan();
			plan.setProductionDate(productionDate);
			plan.setAccepted(false);
		}

		List<ProductionPlanItem> items = new ArrayList<ProductionPlanItem>();
		// find tomorrow's order's items
		List<PastryOrderItem> ordersItems = pastryOrderManager.findToProductionPlan(productionDate);
		// <pastryId, ProductionPlanItem>
		HashMap<Integer, ProductionPlanItem> map = new HashMap<Integer, ProductionPlanItem>();

		for (int i = 0; i < ordersItems.size(); i++) {

			Pastry newPastry = ordersItems.get(i).getPastry();
			int newCount = ordersItems.get(i).getCount();

			// add new pastry to the plan
			if(!map.containsKey(newPastry.getPastryId())) {
				ProductionPlanItem ppi = new ProductionPlanItem();
				ppi.setCount(newCount);
				ppi.setPastry(newPastry);
				ppi.setPlan(plan);
				items.add(ppi);
				map.put(newPastry.getPastryId(), ppi);
			}
			// add count of pastry
			else {
				int pastryId = newPastry.getPastryId();
				map.get(pastryId).addCount(newCount);
			}
		}

		ProductionPlan res;
		if (plan.getPlanId() != 0) {
			plan.setItems(items);
			res = productionPlanManager.save(plan);
		}
		else {
			res = productionPlanManager.save(plan); // first save new plan without items
			mergeItems(res, items); // second save all plan's items
			productionPlanManager.refresh(res); // load items to the plan
		}

		return Response.ok(serializePlan(res)).build();
	}

	@POST
	@Authorized(role = UserRole.Manager)
	@Consumes(MediaType.APPLICATION_JSON)
	@Produces(MediaType.APPLICATION_JSON)
	public Response postJson(JsonObject jsonObject)
	{
		ProductionPlan p = null;
		try {
			p = deserializePlan(jsonObject);
		}
		catch (Exception e) {
			return Response.status(Status.CONFLICT).entity("{\"error\": \"" + e.getMessage() + "\"}").build();
		}

		if (productionPlanManager.findByProductionDate(p.getProductionDate()) != null)
			return Response.status(Status.NOT_FOUND).entity("{\"error\": \"Production plan for this date already exist\"}").build();

		List<ProductionPlanItem> items = p.getItems();
		p.setItems(new ArrayList<ProductionPlanItem>());
		ProductionPlan res = productionPlanManager.save(p); // first save new plan without items

		mergeItems(res, items); // second save all plan's items
		productionPlanManager.refresh(res); // load items to the plan

		return Response.ok(serializePlan(res)).build();
	}

	@Path("/{id}")
	@PUT
	@Authorized(role = UserRole.Manager)
	@Consumes(MediaType.APPLICATION_JSON)
	@Produces(MediaType.APPLICATION_JSON)
	public Response putJson(JsonObject jsonObject, @PathParam("id") int id)
	{
		ProductionPlan p = null;
		try {
			p = deserializePlan(jsonObject);
		}
		catch (Exception e) {
			return Response.status(Status.CONFLICT).entity("{\"error\": \"" + e.getMessage() + "\"}").build();
		}

		if (productionPlanManager.find(id) == null)
			return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such plan\"}").build();
		else if (p.getPlanId() != id)
			return Response.status(Status.CONFLICT).entity("{\"error\": \"Updating plan has different id\"}").build();

		productionPlanManager.save(p);
		return Response.ok(serializePlan(p)).build();
	}

	private void mergeItems(ProductionPlan p, List<ProductionPlanItem> items)
	{
		for (int i = 0; i < items.size(); i++) {
			items.get(i).setPlan(p);
			productionPlanItemManager.save(items.get(i));
		}
	}

	private JsonObject serializePlan(ProductionPlan p)
	{
		JsonArrayBuilder itemsBuilder = Json.createArrayBuilder();
		List<ProductionPlanItem>items = p.getItems();

		// <materiaId, PastryMaterial in plan>
		HashMap<Integer, PastryMaterial> materialsMap = new HashMap<Integer, PastryMaterial>();

		for (int i = 0; i < items.size(); i++) {
			ProductionPlanItem planItem = items.get(i);
			JsonObjectBuilder itemBuilder = Json.createObjectBuilder();
			itemBuilder.add("pastryId", planItem.getPastry().getPastryId());
			itemBuilder.add("name", planItem.getPastry().getName());
			itemBuilder.add("count", planItem.getCount());

			itemsBuilder.add(itemBuilder.build());

			// calculates amount of materials
			int pastryCount = planItem.getCount();
			List<PastryMaterial> pastryMaterials = planItem.getPastry().getMaterials();

			for (int j = 0; j < pastryMaterials.size(); j++) {

				PastryMaterial pastryMaterial = new PastryMaterial();

				Material material = new Material();
				material.setMaterialId(pastryMaterials.get(j).getMaterial().getMaterialId());
				material.setName(pastryMaterials.get(j).getMaterial().getName());

				pastryMaterial.setMaterial(material);

				double amount = pastryMaterials.get(j).getAmount()*pastryCount;
				pastryMaterial.setAmount(amount);

				if(materialsMap.containsKey(material.getMaterialId())) {
					materialsMap.get(material.getMaterialId()).addAmount(amount);
				}
				else {
					materialsMap.put(material.getMaterialId(), pastryMaterial);
				}
			}
		}

		Date productionDate = p.getProductionDate();
		LocalDate production = productionDate.toInstant().atZone(ZoneId.systemDefault()).toLocalDate();

		Date now = new Date();
		LocalDate today = now.toInstant().atZone(ZoneId.systemDefault()).toLocalDate();

		// add amount of materials
		JsonArrayBuilder materialsBuilder = Json.createArrayBuilder();
		for (HashMap.Entry<Integer, PastryMaterial> entry : materialsMap.entrySet()) {
			PastryMaterial material = entry.getValue();
			JsonObjectBuilder materialBuilder = Json.createObjectBuilder();
			materialBuilder.add("materialId", material.getMaterial().getMaterialId());
			materialBuilder.add("name", material.getMaterial().getName());
			materialBuilder.add("amount", material.getAmount());

			// today's or future plan -> add diff
			if (production.equals(today) || production.isAfter(today)) {
				MaterialStorage materialStorage = materialStorageManager.find(material.getMaterial().getMaterialId());

				// material is missing
				if (materialStorage == null) {
					materialBuilder.add("isEnough", false);
					materialBuilder.add("diff", material.getAmount());
				}
				else if (materialStorage.getAmount() < material.getAmount()) {
					materialBuilder.add("isEnough", false);
					materialBuilder.add("diff", Math.abs(materialStorage.getAmount() - material.getAmount()));
				}
				// material is not missing
				else {
					materialBuilder.add("isEnough", true);
					materialBuilder.add("diff", 0);
				}
			}

			materialsBuilder.add(materialBuilder.build());
		}

		JsonObjectBuilder planBuilder = Json.createObjectBuilder();
		planBuilder.add("planId", p.getPlanId());
		planBuilder.add("accepted", p.getAccepted());
		DateFormat f = new SimpleDateFormat("yyyy-MM-dd");
		planBuilder.add("productionDate", f.format(p.getProductionDate()));

		planBuilder.add("items", itemsBuilder.build());
		planBuilder.add("materials", materialsBuilder.build());

		return planBuilder.build();
	}

	private ProductionPlan deserializePlan(JsonObject jsonObject) throws ParseException
	{
		ProductionPlan p = new ProductionPlan();

		if (jsonObject.containsKey("planId"))
			p.setPlanId(jsonObject.getInt("planId"));

		if(!jsonObject.containsKey("accepted"))
			throw new NotFoundException("Accepted not found");
		if(!jsonObject.containsKey("productionDate"))
			throw new NotFoundException("ProductionDate not found");
		if(!jsonObject.containsKey("items"))
			throw new NotFoundException("Items not found");

		p.setAccepted(jsonObject.getBoolean("accepted"));
		Date productionDate = new SimpleDateFormat("yyyy-MM-dd").parse(jsonObject.getString("productionDate"));
		p.setProductionDate(productionDate);

		List<ProductionPlanItem> items = new ArrayList<ProductionPlanItem>();
		JsonArray itemsArray = jsonObject.getJsonArray("items");

		for (int i = 0; i < itemsArray.size(); i++) {
			JsonObject itemObject = itemsArray.getJsonObject(i);

			if (!itemObject.containsKey("pastryId"))
				throw new NotFoundException("PastryId not found");
			if (!itemObject.containsKey("count"))
				throw new NotFoundException("Count not found");

			int pastryId = itemObject.getInt("pastryId");
			Pastry pastry = pastryManager.find(pastryId);
			if (pastry == null)
				throw new NotFoundException("Pastry with id " + pastryId + " not found");

			ProductionPlanItem ppi = new ProductionPlanItem();
			ppi.setPlan(p);
			ppi.setPastry(pastry);
			ppi.setCount(itemObject.getInt("count"));

			items.add(ppi);
		}

		p.setItems(items);
		return p;
	}
}
