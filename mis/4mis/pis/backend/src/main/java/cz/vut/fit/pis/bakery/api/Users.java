package cz.vut.fit.pis.bakery.api;

import java.util.List;

import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.json.Json;
import javax.json.JsonObject;
import javax.naming.NamingException;
import javax.ws.rs.Consumes;
import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriInfo;

import com.auth0.jwt.JWT;
import com.auth0.jwt.algorithms.Algorithm;
import com.auth0.jwt.exceptions.JWTCreationException;

import javax.ws.rs.core.Response.Status;

import cz.vut.fit.pis.bakery.back.Authorized;
import cz.vut.fit.pis.bakery.back.UserRole;
import cz.vut.fit.pis.bakery.data.User;
import cz.vut.fit.pis.bakery.service.UserManager;

/**
 * The class implements processing of incoming HTTP requests to 
 * the users REST end-points and creates the HTTP response for them.
 * 
 * @author David Bednařík
 */
@Stateless
@Path("/users")
public class Users {
	@EJB
	private UserManager manager;
    @Context
    private UriInfo context;

    /**
     * Default constructor. 
     */
    public Users() 
    {
    }
    
    @Path("/list")
    @GET
    @Authorized(role = UserRole.Baker)
    @Produces(MediaType.APPLICATION_JSON)
    public List<User> getJsonUsers() throws NamingException 
    {
    	return manager.findAll();
    }
    
    @Path("/login")
    @POST
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Response loginEmployee(JsonObject inJsonObject) 
    {    	
    	String email = inJsonObject.getString("email");
    	String password = inJsonObject.getString("password");
    	
    	if (email == null || password == null)
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"Authorization of user failed (wrong format)\"}").build();
    	
    	User e = manager.findByEmail(email);
    	if (e == null)
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"Authorization of user failed (user does not exist)\"}").build();
    	else if (!e.getPassword().equals(password))
    		return Response.status(Status.NOT_FOUND).entity("{\"error\": \"Authorization of user failed (wrong email or password)\"}").build(); 	
    	
    	try {
    	    Algorithm algorithm = Algorithm.HMAC256("very long secret used as the key of encryption algorithm");
    	    String token = JWT.create()
    	        .withIssuer("auth0")
    	        .withSubject(email)
    	        .sign(algorithm);
    	    
    	    JsonObject jsonObject = Json.createObjectBuilder()
        			.add("token", token)
        			.add("userId", e.getUserId())
        			.add("name", e.getName())
        			.add("surname", e.getSurname())
        			.add("role", e.getRole())
        			.build(); 
    	    
    	    return Response.ok(jsonObject).build();
    	} catch (JWTCreationException exception){
    	    //Invalid Signing configuration / Couldn't convert Claims.
    		return Response.status(Status.INTERNAL_SERVER_ERROR).entity("{\"error\": \"Authorization of user failed (signing failed)\"}").build(); 
    	}
    }
}
