package cz.vut.fit.pis.bakery.api;

import java.util.ArrayList;
import java.util.List;

import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.json.JsonArray;
import javax.json.JsonObject;
import javax.json.bind.JsonbBuilder;
import javax.naming.NamingException;
import javax.validation.ValidationException;
import javax.ws.rs.Consumes;
import javax.ws.rs.GET;
import javax.ws.rs.NotFoundException;
import javax.ws.rs.POST;
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
import cz.vut.fit.pis.bakery.data.MaterialStorage;
import cz.vut.fit.pis.bakery.service.MaterialManager;
import cz.vut.fit.pis.bakery.service.MaterialStorageManager;

/**
 * The class implements processing of incoming HTTP requests to 
 * the material storage REST end-points and creates the HTTP response for them.
 * 
 * @author David Bednařík
 */
@Stateless
@Path("/materialstorage")
public class MaterialStorages {
	@EJB
	private MaterialStorageManager manager;
	@EJB
	private MaterialManager materialManager;
    @Context
    private UriInfo context;
    
    /**
     * Default constructor. 
     */
    public MaterialStorages() 
    {
    }
    
    @Path("/list")
    @GET
    @Authorized(role = UserRole.Baker)
    @Produces(MediaType.APPLICATION_JSON)
    public List<MaterialStorage> getJson() throws NamingException 
    {
    	List<MaterialStorage> mss = manager.findAll();
    	
    	return mss;
    }
    
    @Path("/{materialId}")
    @GET
    @Authorized(role = UserRole.Baker)
    @Produces(MediaType.APPLICATION_JSON)
    public Response getJsonSingle(@PathParam("materialId") int materialId) throws NamingException 
    {
    	Material material = materialManager.find(materialId);
    	if (material == null)
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"Material does not exist\"}").build();
    		
    	MaterialStorage ms = manager.find(materialId);
    	
    	if (ms == null) {
    		ms = new MaterialStorage();
    		ms.setMaterial(material);
    		ms.setAmount(0.0);
    	}
    	
    	return Response.ok(JsonbBuilder.create().toJson(ms)).build();
    }
    
    @Path("/enter")
    @POST
    @Authorized(role = UserRole.MaterialStoreman)
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Response postJsonEnter(JsonObject jsonObject) 
    {
    	MaterialStorage ms;
    	try {
    		ms = deserializeEnter(jsonObject);
    	}
    	catch (NotFoundException ex) {
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"" + ex.getMessage() + "\"}").build();
    	}
    	
    	manager.save(ms);
    	return Response.ok().build();
    }
    
    @Path("/enter/list")
    @POST
    @Authorized(role = UserRole.MaterialStoreman)
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Response postJsonEnterList(JsonArray jsonArray) 
    {
    	List<MaterialStorage> mss = new ArrayList<MaterialStorage>();
    	
    	for (int i = 0; i < jsonArray.size(); i++) {
    		JsonObject jsonObject = jsonArray.getJsonObject(i);
    		
    		MaterialStorage ms;
        	try {
        		ms = deserializeEnter(jsonObject);
        	}
        	catch (NotFoundException ex) {
        		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"" + ex.getMessage() + "\"}").build();
        	}
        	
        	mss.add(ms);
    	}
    	
    	manager.saveList(mss);
    	return Response.ok().build();
    }
    
    @Path("/withdraw")
    @POST
    @Authorized(role = UserRole.MaterialStoreman)
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Response postJsonWithdraw(JsonObject jsonObject) 
    {
    	MaterialStorage ms;
    	try {
    		ms = deserializeWithdraw(jsonObject);
    	}
    	catch (NotFoundException ex) {
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"" + ex.getMessage() + "\"}").build();
    	} 
    	catch (ValidationException ex) {
    		return Response.status(Status.CONFLICT).entity("{\"error\": \"" + ex.getMessage() + "\"}").build();
    	}
    	
    	manager.save(ms);
    	return Response.ok().build();
    }
    
    @Path("/withdraw/list")
    @POST
    @Authorized(role = UserRole.MaterialStoreman)
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Response postJsonWithdrawList(JsonArray jsonArray) 
    {
    	List<MaterialStorage> mss = new ArrayList<MaterialStorage>();
    	
    	for (int i = 0; i < jsonArray.size(); i++) {
    		JsonObject jsonObject = jsonArray.getJsonObject(i);
    		
    		MaterialStorage ms;
        	try {
        		ms = deserializeWithdraw(jsonObject);
        	}
        	catch (NotFoundException ex) {
        		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"" + ex.getMessage() + "\"}").build();
        	} 
        	catch (ValidationException ex) {
        		return Response.status(Status.CONFLICT).entity("{\"error\": \"" + ex.getMessage() + "\"}").build();
        	}
        	
        	mss.add(ms);
    	}
    	
    	manager.saveList(mss);
    	return Response.ok().build();
    }
    
    private MaterialStorage deserializeEnter(JsonObject jsonObject) 
    {
    	double amount = jsonObject.getJsonNumber("amount").doubleValue();
    	Material material = materialManager.find(jsonObject.getInt("materialId"));
    	if (material == null)
    		throw new NotFoundException("Material does not exist");
    	
    	MaterialStorage ms = manager.find(material.getMaterialId());
    	
    	if (ms == null) {
    		ms = new MaterialStorage();
    		ms.setMaterial(material);
    		ms.setAmount(0.0);
    	}
    	ms.setAmount(ms.getAmount() + amount);
    	
    	return ms;
    }
    
    private MaterialStorage deserializeWithdraw(JsonObject jsonObject) 
    {
    	double amount = jsonObject.getJsonNumber("amount").doubleValue();
    	Material material = materialManager.find(jsonObject.getInt("materialId"));
    	if (material == null)
    		throw new NotFoundException("Material does not exist");
    	
    	MaterialStorage ms = manager.find(material.getMaterialId());
    	
    	if (ms == null)
    		throw new ValidationException("Too little material\"}");
    	
    	if (ms.getAmount() < amount)
    		throw new ValidationException("Too little material\"}");
    	  	
    	ms.setAmount(ms.getAmount() - amount);
    	
    	return ms;
    }
}
