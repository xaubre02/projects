package cz.vut.fit.pis.bakery.api;

import java.util.List;

import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.naming.NamingException;
import javax.ws.rs.Consumes;
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
import cz.vut.fit.pis.bakery.data.User;
import cz.vut.fit.pis.bakery.service.UserManager;

/**
 * The class implements processing of incoming HTTP requests to 
 * the employees REST end-points and creates the HTTP response for them.
 * 
 * @author David Bednařík
 */
@Stateless
@Path("/employees")
public class Employees {
	@EJB
	private UserManager manager;
    @Context
    private UriInfo context;

    /**
     * Default constructor. 
     */
    public Employees() 
    {
    }

    @Path("/list")
    @GET
    @Authorized(role = UserRole.Baker)
    @Produces(MediaType.APPLICATION_JSON)
    public List<User> getJsonEmployees() throws NamingException 
    {
    	return manager.findEmployees();
    }
    
    @Path("/{userId}")
    @GET
    @Authorized(role = UserRole.Baker)
    @Produces(MediaType.APPLICATION_JSON)
    public Response getJsonSingleEmployee(@PathParam("userId") int userId) throws NamingException 
    {
    	User u = manager.findEmployee(userId);
    	if (u == null)
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such user\"}").build();
    	
    	return Response.ok(u).build();
    }
    
    @POST
    @Authorized(role = UserRole.Manager)
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Response postJsonEmployees(User u) 
    {
    	if (manager.findByEmail(u.getEmail()) != null)
    		return Response.status(Status.CONFLICT).entity("{\"error\": \"User with given email already exists\"}").build();
    	
    	if (!u.getRole().equals("baker") &&
    		!u.getRole().equals("materialstoreman") &&
    		!u.getRole().equals("pastrystoreman") && 
    		!u.getRole().equals("manager")) {
    		return Response.status(Status.CONFLICT).entity("{\"error\": \"Invalid employee role\"}").build();
    	}
    		
    	u.setUserId(0);
    	
    	u = manager.save(u);
    	
    	return Response.ok(u).build();
    }
    
    @Path("/{userId}")
    @PUT
    @Authorized(role = UserRole.Manager)
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Response putJsonEmployee(User u, @PathParam("userId") int userId) throws NamingException 
    {
    	if (manager.find(userId) == null)
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such employee\"}").build();
    	else if (u.getUserId() != userId)
    		return Response.status(Status.CONFLICT).entity("{\"error\": \"Updating employee has different id\"}").build();
    	
    	if (!u.getRole().equals("baker") &&
        	!u.getRole().equals("materialstoreman") &&
    		!u.getRole().equals("pastrystoreman") && 
    		!u.getRole().equals("manager")) {
    		return Response.status(Status.CONFLICT).entity("{\"error\": \"Invalid employee role\"}").build();
    	}
    	
    	manager.save(u);
    	
    	return Response.ok(u).build();
    }
}
