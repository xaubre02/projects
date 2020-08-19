package cz.vut.fit.pis.bakery.api;

import java.util.List;

import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.naming.NamingException;
import javax.ws.rs.Consumes;
import javax.ws.rs.DELETE;
import javax.ws.rs.GET;
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
import cz.vut.fit.pis.bakery.data.PastryCategory;
import cz.vut.fit.pis.bakery.service.PastryCategoryManager;

/**
 * The class implements processing of incoming HTTP requests to 
 * the pastry categories REST end-points and creates the HTTP response for them.
 * 
 * @author David Bednařík
 */
@Stateless
@Path("/pastrycategories")
public class PastryCategories {
	@EJB
	private PastryCategoryManager manager;
	@Context
    private UriInfo context;
	
	/**
     * Default constructor. 
     */
    public PastryCategories() 
    {
    }
    
    @Path("/list")
    @GET
    @Produces(MediaType.APPLICATION_JSON)
    public List<PastryCategory> getJson() throws NamingException 
    {
    	return manager.findAll();
    }
    
    @Path("/{id}")
    @GET
    @Produces(MediaType.APPLICATION_JSON)
    public Response getJsonSingle(@PathParam("id") int id) throws NamingException 
    {
    	PastryCategory pc = manager.find(id);
    	if (pc != null)
    		return Response.ok(pc).build();
    	else
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such pastry category\"}").build();
    }
    
    @POST
    @Authorized(role = UserRole.Manager)
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Response postJson(PastryCategory pc) 
    {
    	pc.setCategoryId(0);
    	if (manager.findByName(pc.getName()) != null)
    		return Response.status(Status.CONFLICT).entity("{\"error\": \"Pastry category with given name already exists\"}").build();
    	
    	PastryCategory res = manager.save(pc);
    	return Response.ok(res).build();
    }
    
    @Path("/{id}")
    @PUT
    @Authorized(role = UserRole.Manager)
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Response putJson(PastryCategory pc, @PathParam("id") int id) throws NamingException 
    {
    	if (manager.find(id) == null)
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such pastry category\"}").build();
    	else if (pc.getCategoryId() != id)
    		return Response.status(Status.CONFLICT).entity("{\"error\": \"Updating pastry category has different id\"}").build();
    	else if (manager.findByName(pc.getName()) != null)
    		return Response.status(Status.CONFLICT).entity("{\"error\": \"Pastry category with given name already exists\"}").build();
    	
    	manager.save(pc);
    	
    	return Response.ok(pc).build();
    }
    
    @Path("/{id}")
    @DELETE
    @Authorized(role = UserRole.Manager)
    public Response deleteJson(@PathParam("id") int id) throws NamingException 
    {
    	PastryCategory pc = manager.find(id);
    	
    	if (pc == null)
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such pastry category\"}").build();
    	
    	try {
    		manager.remove(pc);
    	}
    	catch (Exception ex) {
    		return Response.status(Status.CONFLICT)
    				.entity("{\"error\": \"Kategorie peciva nemuze byt odstranena, je pouzivana\"}")
    				.build();
    	}
    	
    	return Response.ok().build();
    }
}
