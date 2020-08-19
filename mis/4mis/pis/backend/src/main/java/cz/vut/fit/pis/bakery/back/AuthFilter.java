package cz.vut.fit.pis.bakery.back;

import java.io.IOException;
import java.lang.reflect.Method;

import javax.annotation.Priority;
import javax.ejb.EJB;
import javax.ws.rs.container.ContainerRequestContext;
import javax.ws.rs.container.ContainerRequestFilter;
import javax.ws.rs.container.ResourceInfo;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.Response;
import javax.ws.rs.ext.Provider;
import javax.ws.rs.Priorities;

import com.auth0.jwt.JWT;
import com.auth0.jwt.JWTVerifier;
import com.auth0.jwt.algorithms.Algorithm;
import com.auth0.jwt.exceptions.JWTVerificationException;
import com.auth0.jwt.interfaces.DecodedJWT;

import cz.vut.fit.pis.bakery.data.User;
import cz.vut.fit.pis.bakery.service.UserManager;

@Provider
@Authorized
@Priority(Priorities.AUTHENTICATION)
public class AuthFilter implements ContainerRequestFilter {
	@EJB
	private UserManager manager;
	@Context
	private ResourceInfo resourceInfo;
     
    @Override
    public void filter(ContainerRequestContext ctx) throws IOException 
    {
    	String token = ctx.getHeaderString("Authorization");
    	
    	if (token == null) {  		
    		abortRequest(ctx, "Unauthorized employee request - JWT token does not exists");
    		return;
    	}
    	
    	try {
    		Method method = resourceInfo.getResourceMethod();
    		if(method == null)
    			return;
            
            Authorized authAnnotation = method.getAnnotation(Authorized.class);
            UserRole role =  authAnnotation.role();
    	    
    	    User user = retrieveUser(manager, token);
    	    if (user == null) {
    	    	abortRequest(ctx, "Unauthorized employee request - JWT does not contain subject with user email");
    	    	return;
    	    }
    	    	
    	    analyzePermissions(ctx, user, role);
    	} catch (JWTVerificationException exception) {
    	    //Invalid signature/claims
    		abortRequest(ctx, "Unauthorized employee request - JWTVerificationException");
    	}
    }
    
    static public User retrieveUser(UserManager manager, String token)
    {
    	Algorithm algorithm = Algorithm.HMAC256("very long secret used as the key of encryption algorithm");
	    JWTVerifier verifier = JWT.require(algorithm)
	        .withIssuer("auth0")
	        .build(); //Reusable verifier instance
	    DecodedJWT jwt = verifier.verify(token);
	    
	    return manager.findByEmail(jwt.getSubject());
    }
    
    private void analyzePermissions(ContainerRequestContext ctx, User user, UserRole role) 
    {
    	if (role == UserRole.None)
    		return;
    	
    	String eRole = user.getRole();
    	
    	if (role == UserRole.Manager && !eRole.equals("manager"))
	    	abortRequest(ctx, "Unauthorized " + eRole + " request - min permission is manager");
	    else if (role == UserRole.MaterialStoreman && (!eRole.equals("materialstoreman") && !eRole.equals("manager")))
	    	abortRequest(ctx, "Unauthorized " + eRole + " request - min permission is material storeman");
	    else if (role == UserRole.PastryStoreman && (!eRole.equals("pastrystoreman") && !eRole.equals("manager")))
	    	abortRequest(ctx, "Unauthorized " + eRole + " request - min permission is pastry storeman");
	    else if (role == UserRole.Baker && (!eRole.equals("baker") && !eRole.equals("pastrystoreman") && !eRole.equals("materialstoreman") && !eRole.equals("manager")))
	    	abortRequest(ctx, "Unauthorized " + eRole + " request - min permission is baker");
	    else if (role == UserRole.Customer && (!eRole.equals("customer") && !eRole.equals("pastrystoreman") && !eRole.equals("manager")))
	    	abortRequest(ctx, "Unauthorized " + eRole + " request - min permission is customer");
    }
    
    private void abortRequest(ContainerRequestContext ctx, String msg) 
    {
    	System.out.println(msg);
		ctx.abortWith(Response.status(Response.Status.UNAUTHORIZED).build());
    }
}
