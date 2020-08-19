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
import cz.vut.fit.pis.bakery.data.Material;
import cz.vut.fit.pis.bakery.data.MaterialStorage;
import cz.vut.fit.pis.bakery.service.MaterialManager;
import cz.vut.fit.pis.bakery.service.MaterialStorageManager;

/**
 * The class implements processing of incoming HTTP requests to 
 * the materials REST end-points and creates the HTTP response for them.
 * 
 * @author David Bednařík
 */
@Stateless
@Path("/materials")
public class Materials {
	@EJB
	private MaterialManager manager;
	@EJB
	private MaterialStorageManager msManager;
    @Context
    private UriInfo context;

    /**
     * Default constructor. 
     */
    public Materials() 
    {
    }

    @Path("/list")
    @GET
    @Produces(MediaType.APPLICATION_JSON)
    public List<Material> getJson() throws NamingException 
    {
    	return manager.findAll();
    }
    
    @Path("/{id}")
    @GET
    @Produces(MediaType.APPLICATION_JSON)
    public Response getJsonSingle(@PathParam("id") int id) throws NamingException 
    {
    	Material m = manager.find(id);
    	if (m != null)
    		return Response.ok(m).build();
    	else
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such material\"}").build();
    }
    
    @POST
    @Authorized(role = UserRole.MaterialStoreman)
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Response postJson(Material m) 
    {
    	m.setMaterialId(0);
    	if (manager.findByName(m.getName()) != null)
    		return Response.status(Status.CONFLICT).entity("{\"error\": \"Material with given name already exists\"}").build();
    	
    	Material res = manager.save(m);
    	return Response.ok(res).build();
    }
    
    @Path("/{id}")
    @PUT
    @Authorized(role = UserRole.MaterialStoreman)
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Response putJson(Material m, @PathParam("id") int id) throws NamingException 
    {
    	if (manager.find(id) == null)
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such material\"}").build();
    	else if (m.getMaterialId() != id)
    		return Response.status(Status.CONFLICT).entity("{\"error\": \"Updating material has different id\"}").build();
    	else if (manager.findByName(m.getName()) != null)
    		return Response.status(Status.CONFLICT).entity("{\"error\": \"Material with given name already exists\"}").build();
    	
    	manager.save(m);
    	
    	return Response.ok(m).build();
    }
    
    @Path("/{id}")
    @DELETE
    @Authorized(role = UserRole.MaterialStoreman)
    public Response deleteJson(@PathParam("id") int id) throws NamingException 
    {
    	Material m = manager.find(id);
    	MaterialStorage ms = msManager.find(id);
    	
    	if (m == null) {
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such material\"}").build();
    	}
    	else if (ms != null && ms.getAmount() > 0) {
    		return Response.status(Status.CONFLICT)
    				.entity("{\"error\": \"Material nemuze byt odstranen, je na sklade\"}")
    				.build();
    	}
    	
    	if (ms != null) // delete record in material storage (amount is zero)
    		msManager.remove(ms);
    	
    	try {
    		manager.remove(m);
    	}
    	catch (Exception ex) {
    		return Response.status(Status.CONFLICT)
    				.entity("{\"error\": \"Material nemuze byt odstranen, je pouzivan\"}")
    				.build();
    	}
    	
    	return Response.ok().build();
    }
}
