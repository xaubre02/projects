package cz.vut.fit.pis.bakery.api;

import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
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
import javax.ws.rs.core.HttpHeaders;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriInfo;
import javax.ws.rs.core.Response.Status;

import cz.vut.fit.pis.bakery.back.AuthFilter;
import cz.vut.fit.pis.bakery.back.Authorized;
import cz.vut.fit.pis.bakery.back.PastryOrderState;
import cz.vut.fit.pis.bakery.back.UserRole;
import cz.vut.fit.pis.bakery.data.User;
import cz.vut.fit.pis.bakery.data.PastryOrder;
import cz.vut.fit.pis.bakery.data.PastryOrderItem;
import cz.vut.fit.pis.bakery.data.PastryStorage;
import cz.vut.fit.pis.bakery.data.Pastry;
import cz.vut.fit.pis.bakery.service.UserManager;
import cz.vut.fit.pis.bakery.service.PastryOrderManager;
import cz.vut.fit.pis.bakery.service.PastryStorageManager;
import cz.vut.fit.pis.bakery.service.PastryOrderItemManager;
import cz.vut.fit.pis.bakery.service.PastryManager;

/**
 * @author Martin Fišer
 */
@Stateless
@Path("/orders")
public class PastryOrders {
	@EJB
	private PastryOrderManager pastryOrderManager;
	@EJB
	private PastryOrderItemManager pastryOrderItemManager;
	@EJB
	private PastryManager pastryManager;
	@EJB
	private PastryStorageManager pastryStorageManager;
	@EJB
	private UserManager userManager;
	@Context
	private UriInfo context;

	public PastryOrders()
	{
	}

	@Path("/list")
	@GET
	@Authorized(role = UserRole.PastryStoreman)
	@Produces(MediaType.APPLICATION_JSON)
	public JsonArray getJson()
	{
		List<PastryOrder> orders = pastryOrderManager.findAll();

		JsonArrayBuilder arrayBuilder = Json.createArrayBuilder();
		for (int i = 0; i < orders.size(); i++) {
			arrayBuilder.add(serializeOrder(orders.get(i)));
		}

		return arrayBuilder.build();
	}

	/*
	 * Get order by id.
	 * PastryStoreman and Manager can get any order.
	 * Customer can get only his order.
	 */
	@Path("/{id}")
	@GET
	@Authorized(role = UserRole.Customer)
	@Produces(MediaType.APPLICATION_JSON)
	public Response getJsonSingle(@Context HttpHeaders headers, @PathParam("id") int id)
	{
		PastryOrder o = pastryOrderManager.find(id);
		String token = headers.getHeaderString("Authorization");
		User authUser = AuthFilter.retrieveUser(userManager, token);

		if (o == null)
			return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such order\"}").build();
		else if (authUser == null)
			return Response.status(Status.UNAUTHORIZED).entity("{\"error\": \"Customer is not authorized\"}").build();
		else if (authUser.getRole().equals("customer") && authUser.getUserId() != o.getCustomer().getUserId())
			return Response.status(Status.UNAUTHORIZED).entity("{\"error\": \"Customer is not authorized\"}").build();

		return Response.ok(serializeOrder(o)).build();
	}

	/*
	 * Get orders in defined state.
	 * PastryStoreman and Manager get all orders in defined state.
	 * Customer get only his orders in defined state.
	 */
	@Path("/state/{state}")
	@GET
	@Authorized(role = UserRole.Customer)
	@Produces(MediaType.APPLICATION_JSON)
	public Response getJsonState(@Context HttpHeaders headers, @PathParam("state") String stateStr)
	{
		PastryOrderState state = PastryOrderState.getState(stateStr);
		String token = headers.getHeaderString("Authorization");
		User authUser = AuthFilter.retrieveUser(userManager, token);
		List<PastryOrder> orders = null;

		if (state == null)
			return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such state\"}").build();
		else if (authUser == null)
			return Response.status(Status.UNAUTHORIZED).entity("{\"error\": \"Customer is not authorized\"}").build();
		else if (authUser.getRole().equals("pastrystoreman") || authUser.getRole().equals("manager"))
			orders = pastryOrderManager.findByState(state);
		else
			orders = pastryOrderManager.findByCustomerAndState(authUser, state);

		JsonArrayBuilder arrayBuilder = Json.createArrayBuilder();
		for (int i = 0; i < orders.size(); i++) {
			arrayBuilder.add(serializeOrder(orders.get(i)));
		}

		return Response.ok(arrayBuilder.build()).build();
	}

	@Path("/deliveryDate/{date}")
	@GET
	@Authorized(role = UserRole.PastryStoreman)
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

		List<PastryOrder> orders = pastryOrderManager.findByDeliveryDate(date);

		JsonArrayBuilder arrayBuilder = Json.createArrayBuilder();
		for (int i = 0; i < orders.size(); i++) {
			arrayBuilder.add(serializeOrder(orders.get(i)));
		}

		return Response.ok(arrayBuilder.build()).build();
	}

	/*
	 * Get customer's orders.
	 * PastryStoreman and Manager can get orders from any customer.
	 * Customer can get only his orders.
	 */
	@Path("/customer/{userId}")
	@GET
	@Authorized(role = UserRole.Customer)
	@Produces(MediaType.APPLICATION_JSON)
	public Response getJsonCustomer(@Context HttpHeaders headers, @PathParam("userId") int userId)
	{
		User customer = userManager.find(userId);
		String token = headers.getHeaderString("Authorization");
		User authUser = AuthFilter.retrieveUser(userManager, token);


		if (customer == null)
			return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such customer\"}").build();
		else if (authUser == null)
			return Response.status(Status.UNAUTHORIZED).entity("{\"error\": \"Customer is not authorized\"}").build();
		else if (authUser.getRole().equals("customer") && authUser.getUserId() != customer.getUserId())
			return Response.status(Status.UNAUTHORIZED).entity("{\"error\": \"Customer is not authorized\"}").build();

		List<PastryOrder> orders = pastryOrderManager.findByCustomer(customer);

		JsonArrayBuilder arrayBuilder = Json.createArrayBuilder();
		for (int i = 0; i < orders.size(); i++) {
			arrayBuilder.add(serializeOrder(orders.get(i)));
		}

		return Response.ok(arrayBuilder.build()).build();
	}

	/*
	 * Add order.
	 * PastryStoreman and Manager can add order to anybody.
	 * Customer can add order only to himself.
	 */
	@POST
	@Authorized(role = UserRole.Customer)
	@Consumes(MediaType.APPLICATION_JSON)
	@Produces(MediaType.APPLICATION_JSON)
	public Response postJson(@Context HttpHeaders headers, JsonObject jsonObject)
	{
		String token = headers.getHeaderString("Authorization");
		User authUser = AuthFilter.retrieveUser(userManager, token);

		PastryOrder o = null;
		try {
			o = deserializeOrder(jsonObject);
		}
		catch (Exception e) {
			return Response.status(Status.CONFLICT).entity("{\"error\": \"" + e.getMessage() + "\"}").build();
		}

		if (authUser == null)
			return Response.status(Status.UNAUTHORIZED).entity("{\"error\": \"Customer is not authorized\"}").build();
		else if (authUser.getRole().equals("customer") && authUser.getUserId() != o.getCustomer().getUserId())
			return Response.status(Status.UNAUTHORIZED).entity("{\"error\": \"Customer is not authorized\"}").build();

		o.setOrderId(0);
		o.setState(PastryOrderState.Created.toString());

		List<PastryOrderItem> items = o.getItems();
		o.setItems(new ArrayList<PastryOrderItem>());
		PastryOrder res = pastryOrderManager.save(o); // first save new order without items

		mergeItems(res, items); // second save all order's items
		pastryOrderManager.refresh(res); // load items to the order

		return Response.ok(serializeOrder(res)).build();
	}

	@Path("/{id}")
	@PUT
	@Authorized(role = UserRole.PastryStoreman)
	@Consumes(MediaType.APPLICATION_JSON)
	@Produces(MediaType.APPLICATION_JSON)
	public Response putJson(JsonObject jsonObject, @PathParam("id") int id)
	{
		PastryOrder o = null;
		try {
			o = deserializeOrder(jsonObject);
		}
		catch (Exception e) {
			return Response.status(Status.CONFLICT).entity("{\"error\": \"" + e.getMessage() + "\"}").build();
		}

		PastryOrder oldOrder = pastryOrderManager.find(id);
		if (oldOrder == null)
			return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such order\"}").build();
		else if (o.getOrderId() != id)
			return Response.status(Status.CONFLICT).entity("{\"error\": \"Updating order has different id\"}").build();
		else if (oldOrder.getState().equals(PastryOrderState.Delivered.toString()))
			return Response.status(Status.CONFLICT).entity("{\"error\": \"Cannot update delivered order\"}").build();
		else if (oldOrder.getState().equals(PastryOrderState.Canceled.toString()))
			return Response.status(Status.CONFLICT).entity("{\"error\": \"Cannot update canceled order\"}").build();

		if ( // changing state to Delivered - remove pastry from storage
				!oldOrder.getState().equals(PastryOrderState.Delivered.toString()) &&
				o.getState().equals(PastryOrderState.Delivered.toString())
				)
		{
			List<PastryStorage> inStorage = pastryStorageManager.findAll();
			List<PastryOrderItem> orderItems = o.getItems();
			// <pastryId, PastryStorage item>
			HashMap<Integer, PastryStorage> map = new HashMap<Integer, PastryStorage>();

			for (int i = 0; i < inStorage.size(); i++) {
				int pastryId = inStorage.get(i).getPastry().getPastryId();
				map.put(pastryId, inStorage.get(i));
			}

			for (int i = 0; i < orderItems.size(); i++) {

				int itemPastryId = orderItems.get(i).getPastry().getPastryId();
				int itemCount = orderItems.get(i).getCount();

				if(!map.containsKey(itemPastryId)) {
					// pastry is not in storage
					return Response.status(Status.CONFLICT).entity("{\"error\": \"Cannot update state to delivered - no pastry in storage\"}").build();
				}
				else {
					map.get(itemPastryId).addCount(-itemCount);

					if (map.get(itemPastryId).getCount() < 0) {
						// too few pastries in storage
						map.get(itemPastryId).addCount(itemCount);
						return Response.status(Status.CONFLICT).entity("{\"error\": \"Cannot update state to delivered - no pastry in storage\"}").build();
					}
				}
			}
			// update PastryStorage
			pastryStorageManager.saveList(inStorage);
		}

		pastryOrderManager.save(o);
		return Response.ok(serializeOrder(o)).build();
	}

	@Path("/cancel/{id}")
	@PUT
	@Authorized(role = UserRole.Customer)
	@Consumes(MediaType.APPLICATION_JSON)
	@Produces(MediaType.APPLICATION_JSON)
	public Response putCancelSingle(@Context HttpHeaders headers, @PathParam("id") int id)
	{
		PastryOrder o = pastryOrderManager.find(id);
		String token = headers.getHeaderString("Authorization");
		User authUser = AuthFilter.retrieveUser(userManager, token);

		if (o == null)
			return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such order\"}").build();
		else if (authUser == null)
			return Response.status(Status.UNAUTHORIZED).entity("{\"error\": \"Customer is not authorized\"}").build();
		else if (authUser.getRole().equals("customer") && authUser.getUserId() != o.getCustomer().getUserId())
			return Response.status(Status.UNAUTHORIZED).entity("{\"error\": \"Customer is not authorized\"}").build();

		if (o.getState().equals(PastryOrderState.Delivered.toString()))
			return Response.status(Status.CONFLICT).entity("{\"error\": \"Cannot cancel delivered order\"}").build();
		else if (o.getState().equals(PastryOrderState.Canceled.toString()))
			return Response.status(Status.CONFLICT).entity("{\"error\": \"Cannot cancel canceled order\"}").build();

		o.setState("canceled");
		PastryOrder res = pastryOrderManager.save(o);
		return Response.ok(serializeOrder(res)).build();
	}

	private void mergeItems(PastryOrder o, List<PastryOrderItem> items)
	{
		for (int i = 0; i < items.size(); i++) {
			items.get(i).setOrder(o);
			pastryOrderItemManager.save(items.get(i));
		}
	}

	private JsonObject serializeOrder(PastryOrder o)
	{
		JsonArrayBuilder itemsBuilder = Json.createArrayBuilder();
		List<PastryOrderItem >items = o.getItems();

		for (int i = 0; i < items.size(); i++) {
			JsonObjectBuilder itemBuilder = Json.createObjectBuilder();
			itemBuilder.add("pastryId", items.get(i).getPastry().getPastryId());
			itemBuilder.add("name", items.get(i).getPastry().getName());
			itemBuilder.add("count", items.get(i).getCount());
			itemBuilder.add("cost", items.get(i).getCost());

			itemsBuilder.add(itemBuilder.build());
		}

		JsonObjectBuilder employeeBuilder = Json.createObjectBuilder();
		employeeBuilder.add("userId", o.getCustomer().getUserId());
		employeeBuilder.add("name", o.getCustomer().getName());
		employeeBuilder.add("surname", o.getCustomer().getSurname());
		employeeBuilder.add("email", o.getCustomer().getEmail());

		JsonObjectBuilder orderBuilder = Json.createObjectBuilder();
		orderBuilder.add("orderId", o.getOrderId());
		orderBuilder.add("price", o.getPrice());
		orderBuilder.add("state", o.getState());
		DateFormat f = new SimpleDateFormat("yyyy-MM-dd");
		orderBuilder.add("deliveryDate", f.format(o.getDeliveryDate()));

		orderBuilder.add("customer", employeeBuilder.build());
		orderBuilder.add("items", itemsBuilder.build());

		return orderBuilder.build();
	}

	private PastryOrder deserializeOrder(JsonObject jsonObject) throws ParseException
	{
		PastryOrder o = new PastryOrder();

		if (jsonObject.containsKey("orderId"))
			o.setOrderId(jsonObject.getInt("orderId"));

		if(!jsonObject.containsKey("state"))
			throw new NotFoundException("State not found");
		if(!jsonObject.containsKey("deliveryDate"))
			throw new NotFoundException("DeliveryDate not found");
		if(!jsonObject.containsKey("customer"))
			throw new NotFoundException("Customer not found");
		if(!jsonObject.getJsonObject("customer").containsKey("userId"))
			throw new NotFoundException("Customer userId not found");
		if(!jsonObject.containsKey("items"))
			throw new NotFoundException("Items not found");

		o.setState(jsonObject.getString("state"));
		Date deliveryDate = new SimpleDateFormat("yyyy-MM-dd").parse(jsonObject.getString("deliveryDate"));
		o.setDeliveryDate(deliveryDate);

		int userId = jsonObject.getJsonObject("customer").getInt("userId");
		User customer = userManager.find(userId);
		if (customer == null)
			throw new NotFoundException("Customer with id " + userId + " not found");
		o.setCustomer(customer);

		List<PastryOrderItem> items = new ArrayList<PastryOrderItem>();
		JsonArray itemsArray = jsonObject.getJsonArray("items");

		double totalPrice = 0.0;
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

			PastryOrderItem oi = new PastryOrderItem();
			oi.setOrder(o);
			oi.setPastry(pastry);
			oi.setCount(itemObject.getInt("count"));
			oi.setCost(itemObject.getInt("count")*pastry.getCost());
			totalPrice += oi.getCost();

			items.add(oi);
		}

		o.setPrice(totalPrice);
		o.setItems(items);
		return o;
	}
}
