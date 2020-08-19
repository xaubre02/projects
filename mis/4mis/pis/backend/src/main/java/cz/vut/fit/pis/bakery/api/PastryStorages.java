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
import cz.vut.fit.pis.bakery.data.Pastry;
import cz.vut.fit.pis.bakery.data.PastryStorage;
import cz.vut.fit.pis.bakery.service.PastryManager;
import cz.vut.fit.pis.bakery.service.PastryStorageManager;

/**
 * The class implements processing of incoming HTTP requests to 
 * the pastry storage REST end-points and creates the HTTP response for them.
 * 
 * @author David Bednařík
 */
@Stateless
@Path("/pastrystorage")
public class PastryStorages {
	@EJB
	private PastryStorageManager manager;
	@EJB
	private PastryManager pastryManager;
    @Context
    private UriInfo context;
    
    /**
     * Default constructor. 
     */
    public PastryStorages() 
    {
    }
    
    @Path("/list")
    @GET
    @Authorized(role = UserRole.Baker)
    @Produces(MediaType.APPLICATION_JSON)
    public List<PastryStorage> getJson() throws NamingException 
    {
    	List<PastryStorage> pss = manager.findAll();
    	
    	return pss;
    }
    
    @Path("/{pastryId}")
    @GET
    @Authorized(role = UserRole.Baker)
    @Produces(MediaType.APPLICATION_JSON)
    public Response getJsonSingle(@PathParam("pastryId") int pastryId) throws NamingException 
    {
    	Pastry pastry = pastryManager.find(pastryId);
    	if (pastry == null)
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"Pastry does not exist\"}").build();
    		
    	PastryStorage ps = manager.find(pastryId);
    	
    	if (ps == null) {
    		ps = new PastryStorage();
    		ps.setPastry(pastry);
    		ps.setCount(0);
    	}
    	
    	return Response.ok(JsonbBuilder.create().toJson(ps)).build();
    }
    
    @Path("/enter")
    @POST
    @Authorized(role = UserRole.PastryStoreman)
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Response postJsonEnter(JsonObject jsonObject) 
    {
    	PastryStorage ps;
    	try {
    		ps = deserializeEnter(jsonObject);
    	}
    	catch (NotFoundException ex) {
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"" + ex.getMessage() + "\"}").build();
    	}
    	
    	manager.save(ps);
    	return Response.ok().build();
    }
    
    @Path("/enter/list")
    @POST
    @Authorized(role = UserRole.MaterialStoreman)
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Response postJsonEnterList(JsonArray jsonArray) 
    {
    	List<PastryStorage> pss = new ArrayList<PastryStorage>();
    	
    	for (int i = 0; i < jsonArray.size(); i++) {
    		JsonObject jsonObject = jsonArray.getJsonObject(i);
    		
    		PastryStorage ps;
        	try {
        		ps = deserializeEnter(jsonObject);
        	}
        	catch (NotFoundException ex) {
        		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"" + ex.getMessage() + "\"}").build();
        	}
        	
        	pss.add(ps);
    	}
    	
    	manager.saveList(pss);
    	return Response.ok().build();
    }
    
    @Path("/withdraw")
    @POST
    @Authorized(role = UserRole.PastryStoreman)
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Response postJsonWithdraw(JsonObject jsonObject) 
    {
    	PastryStorage ps;
    	try {
    		ps = deserializeWithdraw(jsonObject);
    	}
    	catch (NotFoundException ex) {
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"" + ex.getMessage() + "\"}").build();
    	} 
    	catch (ValidationException ex) {
    		return Response.status(Status.CONFLICT).entity("{\"error\": \"" + ex.getMessage() + "\"}").build();
    	}
    	
    	manager.save(ps);
    	return Response.ok().build();
    }
    
    @Path("/withdraw/list")
    @POST
    @Authorized(role = UserRole.MaterialStoreman)
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Response postJsonWithdrawList(JsonArray jsonArray) 
    {
    	List<PastryStorage> pss = new ArrayList<PastryStorage>();
    	
    	for (int i = 0; i < jsonArray.size(); i++) {
    		JsonObject jsonObject = jsonArray.getJsonObject(i);
    		
    		PastryStorage ps;
        	try {
        		ps = deserializeWithdraw(jsonObject);
        	}
        	catch (NotFoundException ex) {
        		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"" + ex.getMessage() + "\"}").build();
        	} 
        	catch (ValidationException ex) {
        		return Response.status(Status.CONFLICT).entity("{\"error\": \"" + ex.getMessage() + "\"}").build();
        	}
        	
        	pss.add(ps);
    	}
    	
    	manager.saveList(pss);
    	return Response.ok().build();
    }
    
    private PastryStorage deserializeEnter(JsonObject jsonObject) 
    {
    	int count = jsonObject.getInt("count");
    	Pastry pastry = pastryManager.find(jsonObject.getInt("pastryId"));
    	if (pastry == null)
    		throw new NotFoundException("Pastry does not exist");
    	
    	PastryStorage ps = manager.find(pastry.getPastryId());
    	
    	if (ps == null) {
    		ps = new PastryStorage();
    		ps.setPastry(pastry);
    		ps.setCount(0);
    	}
    	ps.setCount(ps.getCount() + count);
    	
    	return ps;
    }
    
    private PastryStorage deserializeWithdraw(JsonObject jsonObject) 
    {
    	int count = jsonObject.getInt("count");
    	Pastry pastry = pastryManager.find(jsonObject.getInt("pastryId"));
    	if (pastry == null)
    		throw new NotFoundException("Pastry does not exist");
    	
    	PastryStorage ps = manager.find(pastry.getPastryId());
    	
    	if (ps == null)
    		throw new ValidationException("Too little pastry\"}");
    	
    	if (ps.getCount() < count)
    		throw new ValidationException("Too little pastry\"}");
    	  	
    	ps.setCount(ps.getCount() - count);
    	
    	return ps;
    }
}
