package cz.vut.fit.pis.bakery.api;

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
import cz.vut.fit.pis.bakery.data.Pastry;
import cz.vut.fit.pis.bakery.data.PastryCategory;
import cz.vut.fit.pis.bakery.data.PastryMaterial;
import cz.vut.fit.pis.bakery.data.PastryStorage;
import cz.vut.fit.pis.bakery.service.MaterialManager;
import cz.vut.fit.pis.bakery.service.PastryCategoryManager;
import cz.vut.fit.pis.bakery.service.PastryManager;
import cz.vut.fit.pis.bakery.service.PastryMaterialManager;
import cz.vut.fit.pis.bakery.service.PastryStorageManager;

/**
 * The class implements processing of incoming HTTP requests to 
 * the pastry REST end-points and creates the HTTP response for them.
 * 
 * @author David Bednařík
 */
@Stateless
@Path("/pastry")
public class Pastries {
	@EJB
	private PastryManager manager;
	@EJB
	private MaterialManager materialManager;
	@EJB
	private PastryCategoryManager categoryManager;
	@EJB
	private PastryMaterialManager pmManager;
	@EJB
	private PastryStorageManager psManager;
    @Context
    private UriInfo context;

    /**
     * Default constructor. 
     */
    public Pastries() 
    {
    }
    
    @Path("/list")
    @GET
    @Produces(MediaType.APPLICATION_JSON)
    public JsonArray getJson() throws NamingException 
    {
    	List<Pastry> pastry = manager.findAll();
    	
    	JsonArrayBuilder arrayBuilder = Json.createArrayBuilder();
    	for (int i = 0; i < pastry.size(); i++) { 		
    		arrayBuilder.add(serializePastry(pastry.get(i)));
    	}
    	
    	return arrayBuilder.build();
    }
    
    @Path("/{id}")
    @GET
    @Produces(MediaType.APPLICATION_JSON)
    public Response getJsonSingle(@PathParam("id") int id) throws NamingException 
    {
    	Pastry p = manager.find(id);
    	if (p != null)
    		return Response.ok(serializePastry(p)).build();
    	else
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such pastry\"}").build();
    }
    
    @Path("/category/{categoryId}")
    @GET
    @Produces(MediaType.APPLICATION_JSON)
    public Response getJsonCategory(@PathParam("categoryId") int categoryId) throws NamingException 
    {
    	PastryCategory category = categoryManager.find(categoryId);
    	if (category == null)
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such category\"}").build();
    	
    	List<Pastry> pastry = manager.findAllInCategory(category);
    	
    	JsonArrayBuilder arrayBuilder = Json.createArrayBuilder();
    	for (int i = 0; i < pastry.size(); i++) { 		
    		arrayBuilder.add(serializePastry(pastry.get(i)));
    	}
    	
    	return Response.ok(arrayBuilder.build()).build();
    }
    
    @POST
    @Authorized(role = UserRole.Manager)
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Response postJson(JsonObject jsonObject) 
    {
    	Pastry p = null;
    	try {
    		p = deserializePastry(jsonObject);
    	}
    	catch (NotFoundException e) {
    		return Response.status(Status.CONFLICT).entity("{\"error\": \"" + e.getMessage() + "\"}").build();
    	}
    	
    	p.setPastryId(0);
    	if (manager.findByName(p.getName()) != null)
    		return Response.status(Status.CONFLICT).entity("{\"error\": \"Pastry with given name already exists\"}").build();
    	
    	List<PastryMaterial> materials = p.getMaterials();
    	p.setMaterials(new ArrayList<PastryMaterial>());    	
    	manager.save(p); // first save new pastry without materials
    	
    	Pastry res = manager.findByName(p.getName());
    	mergeMaterials(res, materials); // second save all pastry's materials
    	
    	manager.refresh(res); // load materials to the pastry
    	
    	return Response.ok(serializePastry(res)).build();
    }
    
    @Path("/{id}")
    @PUT
    @Authorized(role = UserRole.Manager)
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Response putJson(JsonObject jsonObject, @PathParam("id") int id) throws NamingException 
    {
    	Pastry p = null;
    	try {
    		p = deserializePastry(jsonObject);
    	}
    	catch (NotFoundException e) {
    		return Response.status(Status.CONFLICT).entity("{\"error\": \"" + e.getMessage() + "\"}").build();
    	}
    	
    	if (manager.find(id) == null)
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such pastry\"}").build();
    	else if (p.getPastryId() != id)
    		return Response.status(Status.CONFLICT).entity("{\"error\": \"Updating pastry has different id\"}").build();
    	
    	Pastry dbp = manager.find(p.getPastryId());
    	List<PastryMaterial> oldMaterials = dbp.getMaterials();
    	List<PastryMaterial> newMaterials = p.getMaterials();
    	
    	dbp.setName(p.getName());
    	dbp.setCost(p.getCost());
    	dbp.setCategory(p.getCategory());
    	dbp.setMaterials(p.getMaterials()); // update materials (new and updated)
    	
    	removeMaterials(oldMaterials, newMaterials); // remove removed materials
    	
    	manager.save(dbp); // save changes
    	manager.refresh(dbp); // load all changes with materials
    	
    	return Response.ok(serializePastry(dbp)).build();
    }
    
    @Path("/{id}")
    @DELETE
    @Authorized(role = UserRole.Manager)
    public Response deleteJson(@PathParam("id") int id) throws NamingException 
    {
    	Pastry p = manager.find(id);
		PastryStorage ps = psManager.find(id);
    	
    	if (p == null) {
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such pastry\"}").build();
    	}
    	else if (ps != null && ps.getCount() > 0) {
    		return Response.status(Status.CONFLICT)
    				.entity("{\"error\": \"Pecivo nemuze byt odstraneno, je na sklade\"}")
    				.build();
    	}
    	
    	if (ps != null) // delete record in material storage (amount is zero)
    		psManager.remove(ps);
    	
    	// First delete all materials associated with deleted pastry
    	List<PastryMaterial> materials = p.getMaterials();
    	for (int i = 0; i < materials.size(); i++)
    		pmManager.remove(materials.get(i));
    	manager.refresh(p);
    	
    	try {
    		manager.remove(p);
    	}
    	catch (Exception ex) {
    		// If the pastry is not deleted restore it's materials
    		for (int i = 0; i < materials.size(); i++)
        		pmManager.save(materials.get(i));
    		
    		return Response.status(Status.CONFLICT)
    				.entity("{\"error\": \"Pecivo nemuze byt odstraneno, je pouzivano\"}")
    				.build();
    	}
    	
    	return Response.ok().build();
    }
    
    private void mergeMaterials(Pastry p, List<PastryMaterial> materials)
    {
    	for (int i = 0; i < materials.size(); i++) {
    		materials.get(i).setPastry(p);
    		pmManager.save(materials.get(i));
    	}
    }
    
    private void removeMaterials(List<PastryMaterial> oldMaterials, List<PastryMaterial> newMaterials) 
    {
    	for (int i = 0; i < oldMaterials.size(); i++) {
    		boolean found = false;
    		
    		for (int j = 0; j < newMaterials.size(); j++) {
    			if (oldMaterials.get(i).getMaterial().equals(newMaterials.get(j).getMaterial())) {
    				found = true;
    				break;
    			}
    		}
    		
    		if (found == false)
    			pmManager.remove(oldMaterials.get(i));
    	}
    }
    
    private JsonObject serializePastry(Pastry p)
    {
    	JsonArrayBuilder materialsBuilder = Json.createArrayBuilder();
		List<PastryMaterial> materials = p.getMaterials();
		for (int j = 0; j < materials.size(); j++) {
			JsonObjectBuilder materialBuilder = Json.createObjectBuilder();
			materialBuilder.add("materialId", materials.get(j).getMaterial().getMaterialId());
			materialBuilder.add("name", materials.get(j).getMaterial().getName());
			materialBuilder.add("amount", materials.get(j).getAmount());
			
			materialsBuilder.add(materialBuilder.build());
		}
		
		JsonObjectBuilder categoryBuilder = Json.createObjectBuilder();
		categoryBuilder.add("categoryId", p.getCategory().getCategoryId());
		categoryBuilder.add("name", p.getCategory().getName());
		
		JsonObjectBuilder pastryBuilder = Json.createObjectBuilder();
		pastryBuilder.add("pastryId", p.getPastryId());
		pastryBuilder.add("name", p.getName());
		pastryBuilder.add("cost", p.getCost());		
		pastryBuilder.add("category", categoryBuilder.build());
		pastryBuilder.add("materials", materialsBuilder.build());
		
		return pastryBuilder.build();
    }
    
    private Pastry deserializePastry(JsonObject jsonObject)
    {
    	Pastry p = new Pastry();
    	
    	if (jsonObject.containsKey("pastryId"))
    		p.setPastryId(jsonObject.getInt("pastryId"));
    	
    	p.setName(jsonObject.getString("name"));
    	p.setCost(jsonObject.getJsonNumber("cost").doubleValue());
    	int categoryId = jsonObject.getJsonObject("category").getInt("categoryId");
    	PastryCategory category = categoryManager.find(categoryId);
    	if (category == null)
    		throw new NotFoundException("Pastry category with id " + categoryId + " not found");
    	p.setCategory(category);
    	
    	List<PastryMaterial> materials = new ArrayList<PastryMaterial>();
    	JsonArray materialsArray = jsonObject.getJsonArray("materials");
    	for (int i = 0; i < materialsArray.size(); i++) {
    		JsonObject materialObject = materialsArray.getJsonObject(i);
    		
    		int materialId = materialObject.getInt("materialId");
    		Material material = materialManager.find(materialId);
    		if (material == null)
    			throw new NotFoundException("Material with id " + materialId + " not found");
    		
    		PastryMaterial pm = new PastryMaterial();
    		pm.setPastry(p);
    		pm.setMaterial(material);
    		pm.setAmount(materialObject.getJsonNumber("amount").doubleValue());
    		
    		materials.add(pm);
    	}
    		
    	p.setMaterials(materials);
    	
    	return p;
    }
}
