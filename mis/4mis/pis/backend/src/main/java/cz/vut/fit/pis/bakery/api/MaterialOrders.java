package cz.vut.fit.pis.bakery.api;

import java.time.LocalDate;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.List;

import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.json.Json;
import javax.json.JsonArray;
import javax.json.JsonArrayBuilder;
import javax.json.JsonObject;
import javax.json.JsonObjectBuilder;
import javax.naming.NamingException;
import javax.ws.rs.Consumes;
import javax.ws.rs.DELETE;
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
import cz.vut.fit.pis.bakery.data.Material;
import cz.vut.fit.pis.bakery.data.MaterialOrder;
import cz.vut.fit.pis.bakery.data.MaterialOrderItem;
import cz.vut.fit.pis.bakery.data.MaterialStorage;
import cz.vut.fit.pis.bakery.service.MaterialManager;
import cz.vut.fit.pis.bakery.service.MaterialOrderItemManager;
import cz.vut.fit.pis.bakery.service.MaterialOrderManager;
import cz.vut.fit.pis.bakery.service.MaterialStorageManager;

/**
 * The class implements processing of incoming HTTP requests to 
 * the material orders REST end-points and creates the HTTP response for them.
 * 
 * @author David Bednařík
 */
@Stateless
@Path("/materialorders")
public class MaterialOrders {
	@EJB
	private MaterialOrderManager manager;
	@EJB
	private MaterialManager materialManager;
	@EJB
	private MaterialOrderItemManager moiManager;
	@EJB
	private MaterialStorageManager msManager;
    @Context
    private UriInfo context;

    /**
     * Default constructor. 
     */
    public MaterialOrders() 
    {
    }
    
    @Path("/list")
    @GET
    @Authorized(role = UserRole.MaterialStoreman)
    @Produces(MediaType.APPLICATION_JSON)
    public JsonArray getJson() throws NamingException 
    {
    	List<MaterialOrder> orders = manager.findAll();
    	
    	JsonArrayBuilder arrayBuilder = Json.createArrayBuilder();
    	for (int i = 0; i < orders.size(); i++) { 		
    		arrayBuilder.add(serializeOrder(orders.get(i)));
    	}
    	
    	return arrayBuilder.build();
    }
    
    @Path("/{id}")
    @GET
    @Authorized(role = UserRole.MaterialStoreman)
    @Produces(MediaType.APPLICATION_JSON)
    public Response getJsonSingle(@PathParam("id") int id) throws NamingException 
    {
    	MaterialOrder mo = manager.find(id);
    	if (mo != null)
    		return Response.ok(serializeOrder(mo)).build();
    	else
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such material order\"}").build();
    }
    
    @Path("/delivered/{state}")
    @GET
    @Authorized(role = UserRole.MaterialStoreman)
    @Produces(MediaType.APPLICATION_JSON)
    public Response getJsonState(@PathParam("state") String stateStr)
    {
    	boolean delivered;
    	if (stateStr.equals("true"))
    		delivered = true;
    	else if (stateStr.equals("false"))
    		delivered = false;
    	else 
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such delivered state\"}").build();
    	
    	List<MaterialOrder> orders = manager.findAllInState(delivered);
    	
    	JsonArrayBuilder arrayBuilder = Json.createArrayBuilder();
    	for (int i = 0; i < orders.size(); i++) { 		
    		arrayBuilder.add(serializeOrder(orders.get(i)));
    	}

    	return Response.ok(arrayBuilder.build()).build();
    }
    
    @POST
    @Authorized(role = UserRole.Manager)
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Response postJson(JsonObject jsonObject) 
    {
    	MaterialOrder mo = null;
    	try {
    		mo = deserializeOrder(jsonObject);
    	}
    	catch (Exception e) {
    		return Response.status(Status.CONFLICT).entity("{\"error\": \"" + e.getMessage() + "\"}").build();
    	}
    	
    	mo.setOrderId(0);
    	List<MaterialOrderItem> materials = mo.getMaterials();
    	mo.setMaterials(new ArrayList<MaterialOrderItem>());
    	MaterialOrder res = manager.save(mo); // first save new order without materials
    	
    	for (int i = 0; i < materials.size(); i++) {
    		materials.get(i).setOrder(mo);
    		moiManager.save(materials.get(i));
    	}
    	
    	manager.refresh(res); // load materials to the order
    	
    	return Response.ok(serializeOrder(res)).build();
    }
    
    @Path("/accept/{id}")
    @PUT
    @Authorized(role = UserRole.MaterialStoreman)
    @Produces(MediaType.APPLICATION_JSON)
    public Response putDelivered(@PathParam("id") int id) throws NamingException
    {
    	MaterialOrder mo = manager.find(id);
    	
    	if (mo == null)
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such material order\"}").build();
    	else if (mo.isDelivered())
    		return Response.status(Status.CONFLICT).entity("{\"error\": \"Material order is already delivered\"}").build();
    	
    	mo.setDelivered(true);
    	manager.save(mo);
    	
    	List<MaterialOrderItem> materials = mo.getMaterials();
    	for (int i = 0; i < materials.size(); i++) { // Enter all materials into material storage
    		MaterialOrderItem item = materials.get(i);
    		
    		MaterialStorage ms = msManager.find(item.getMaterial().getMaterialId());
    		if (ms == null) {
    			ms = new MaterialStorage();
    			ms.setMaterial(item.getMaterial());
    			ms.setAmount(0.0);
    		}
    		ms.setAmount(ms.getAmount() + item.getAmount());
    		
    		msManager.save(ms);
    	}
    	
    	return Response.ok().build();
    }
    
    @Path("/cancel/{id}")
    @DELETE
    @Authorized(role = UserRole.Manager)
    public Response deleteJson(@PathParam("id") int id) throws NamingException 
    {
    	MaterialOrder mo = manager.find(id);
    	
    	if (mo == null)
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such material order\"}").build();
    	else if (mo.isDelivered())
    		return Response.status(Status.CONFLICT).entity("{\"error\": \"Objednavka surovin nemuze byt smazana je jiz dorucen\"}").build();
    	
    	// First delete all materials associated with deleted material order
    	List<MaterialOrderItem> materials = mo.getMaterials();
    	for (int i = 0; i < materials.size(); i++)
    		moiManager.remove(materials.get(i));
    	manager.refresh(mo);
    	
    	try {
    		manager.remove(mo);
    	}
    	catch (Exception ex) {
    		// If the materials order is not deleted restore it's materials
    		for (int i = 0; i < materials.size(); i++)
    			moiManager.save(materials.get(i));
    		
    		return Response.status(Status.CONFLICT)
    				.entity("{\"error\": \"Chyba smazani objednavky surovin\"}")
    				.build();
    	}
    	
    	return Response.ok().build();
    }
    
    private JsonObject serializeOrder(MaterialOrder mo) 
    {
    	JsonArrayBuilder materialsBuilder = Json.createArrayBuilder();
		List<MaterialOrderItem> materials = mo.getMaterials();
		for (int j = 0; j < materials.size(); j++) {
			JsonObjectBuilder materialBuilder = Json.createObjectBuilder();
			materialBuilder.add("materialId", materials.get(j).getMaterial().getMaterialId());
			materialBuilder.add("name", materials.get(j).getMaterial().getName());
			materialBuilder.add("cost", materials.get(j).getCost());
			materialBuilder.add("amount", materials.get(j).getAmount());
			
			materialsBuilder.add(materialBuilder.build());
		}
		
		JsonObjectBuilder orderBuilder = Json.createObjectBuilder();
		orderBuilder.add("orderId", mo.getOrderId());
		DateTimeFormatter f = DateTimeFormatter.ofPattern("yyyy-MM-dd");
		orderBuilder.add("dateOfCreation", mo.getDateOfCreation().format(f));
		orderBuilder.add("delivered", mo.isDelivered());
		orderBuilder.add("materials", materialsBuilder.build());
		
		return orderBuilder.build();
    }
    
    private MaterialOrder deserializeOrder(JsonObject jsonObject)
    {
    	MaterialOrder mo = new MaterialOrder();
    	
    	if (jsonObject.containsKey("orderId"))
    		mo.setOrderId(jsonObject.getInt("orderId"));
    	
    	if (jsonObject.containsKey("dateOfCreation")) {
    		DateTimeFormatter f = DateTimeFormatter.ofPattern("yyyy-MM-dd");
    		mo.setDateOfCreation(LocalDate.parse(jsonObject.getString("dateOfCreation"), f));
    	}
    	else {
    		mo.setDateOfCreation(LocalDate.now());
    	}
    	
    	mo.setDelivered(jsonObject.getBoolean("delivered", false));
    	
    	List<MaterialOrderItem> materials = new ArrayList<MaterialOrderItem>();
    	JsonArray materialsArray = jsonObject.getJsonArray("materials");
    	for (int i = 0; i < materialsArray.size(); i++) {
    		JsonObject materialObject = materialsArray.getJsonObject(i);
    		
    		int materialId = materialObject.getInt("materialId");
    		Material material = materialManager.find(materialId);
    		if (material == null)
    			throw new NotFoundException("Material with id " + materialId + " not found");
    		
    		MaterialOrderItem moi = new MaterialOrderItem();
    		moi.setOrder(mo);
    		moi.setMaterial(material);
    		moi.setAmount(materialObject.getJsonNumber("amount").doubleValue());
    		moi.setCost(materialObject.getJsonNumber("cost").doubleValue());

    		materials.add(moi);
    	}
    		
    	mo.setMaterials(materials);
    	
    	return mo;
    }
}
