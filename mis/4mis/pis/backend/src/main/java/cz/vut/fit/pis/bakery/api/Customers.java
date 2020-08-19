package cz.vut.fit.pis.bakery.api;

import java.util.List;

import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.naming.NamingException;
import javax.ws.rs.Consumes;
import javax.ws.rs.core.HttpHeaders;
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

import cz.vut.fit.pis.bakery.back.AuthFilter;
import cz.vut.fit.pis.bakery.back.Authorized;
import cz.vut.fit.pis.bakery.back.UserRole;
import cz.vut.fit.pis.bakery.data.User;
import cz.vut.fit.pis.bakery.service.UserManager;

/**
 * The class implements processing of incoming HTTP requests to 
 * the customers REST end-points and creates the HTTP response for them.
 * 
 * @author David Bednařík
 */
@Stateless
@Path("/customers")
public class Customers {
	@EJB
	private UserManager manager;
    @Context
    private UriInfo context;

    /**
     * Default constructor. 
     */
    public Customers() 
    {
    }
    
    @Path("/list")
    @GET
    @Authorized(role = UserRole.Baker)
    @Produces(MediaType.APPLICATION_JSON)
    public List<User> getJsonCustomers() throws NamingException 
    {
    	return manager.findCustomers();
    }
    
    @Path("/{userId}")
    @GET
    @Produces(MediaType.APPLICATION_JSON)
    public Response getJsonSingleCustomer(
    		@Context HttpHeaders headers, 
    		@PathParam("userId") int userId) throws NamingException 
    {
    	User u = manager.findCustomer(userId);
    	String token = headers.getHeaderString("Authorization");
    	User authUser = AuthFilter.retrieveUser(manager, token);
    	
    	if (u == null)
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such customer\"}").build();
    	else if (authUser == null)
    		return Response.status(Status.UNAUTHORIZED).entity("{\"error\": \"Customer is not authorized\"}").build();
    	else if (authUser.getUserId() != userId)
    		return Response.status(Status.UNAUTHORIZED).entity("{\"error\": \"Customer is not authorized\"}").build();

    	return Response.ok(u).build();
    }
    
    @POST
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Response postJsonCustomers(User u) 
    {
    	if (manager.findByEmail(u.getEmail()) != null)
    		return Response.status(Status.CONFLICT).entity("{\"error\": \"User with given email already exists\"}").build();
    	
    	u.setUserId(0);
    	u.setRole("customer");
    	
    	u = manager.save(u);
    	
    	return Response.ok(u).build();
    }
    
    @Path("/{userId}")
    @PUT
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Response putJsonCustomer(
    		User u, 
    		@Context HttpHeaders headers, 
    		@PathParam("userId") int userId) throws NamingException 
    {
    	String token = headers.getHeaderString("Authorization");
    	User authUser = AuthFilter.retrieveUser(manager, token);
    	
    	if (manager.find(userId) == null)
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"No such customer\"}").build();
    	else if (u.getUserId() != userId)
    		return Response.status(Status.CONFLICT).entity("{\"error\": \"Updating customer has different id\"}").build();
    	else if (authUser == null)
    		return Response.status(Status.UNAUTHORIZED).entity("{\"error\": \"Customer is not authorized\"}").build();
    	else if (authUser.getUserId() != userId)
    		return Response.status(Status.UNAUTHORIZED).entity("{\"error\": \"Customer is not authorized\"}").build();
    	
    	u.setRole("customer");
    	
    	manager.save(u);
    	
    	return Response.ok(u).build();
    }
}
