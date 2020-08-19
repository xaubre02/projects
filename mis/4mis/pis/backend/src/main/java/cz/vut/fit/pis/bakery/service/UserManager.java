package cz.vut.fit.pis.bakery.service;

import java.util.List;

import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.NoResultException;
import javax.persistence.PersistenceContext;

import cz.vut.fit.pis.bakery.data.User;

/**
 * The class manages user entities. It allows to get, 
 * update, insert and delete user.
 * 
 * @author David Bednařík
 */
@Stateless
public class UserManager {
	@PersistenceContext
    private EntityManager em;

    public User find(int userId)
    {
    	return em.find(User.class, userId);
    }
    
    public User findEmployee(int userId)
    {
    	try {
			return (User)em.createQuery("SELECT u FROM User u WHERE u.userId = :val AND u.role != 'customer'")
					.setParameter("val", userId)
					.getSingleResult();
		}
		catch (NoResultException ex) {
			return null;
		}
    }
    
    public User findCustomer(int userId)
    {
    	try {
			return (User)em.createQuery("SELECT u FROM User u WHERE u.userId = :val AND u.role = 'customer'")
					.setParameter("val", userId)
					.getSingleResult();
		}
		catch (NoResultException ex) {
			return null;
		}
    }
    
    public User findByEmail(String email)
    {
    	try {
			return (User)em.createQuery("SELECT u FROM User u WHERE u.email = :val")
					.setParameter("val", email)
					.getSingleResult();
		}
		catch (NoResultException ex) {
			return null;
		}
    }
      
    public User save(User user)
    {
    	return em.merge(user);
    }
	
    public void remove(User user)
    {
    	em.remove(user);
    	em.flush();
    }
    
	public List<User> findAll()
    {
    	return em.createQuery("SELECT u FROM User u", User.class).getResultList();
    }
	
	public List<User> findEmployees()
    {
    	return em.createQuery("SELECT u FROM User u WHERE u.role != 'customer'", User.class).getResultList();
    }
	
	public List<User> findCustomers()
    {
    	return em.createQuery("SELECT u FROM User u WHERE u.role = 'customer'", User.class).getResultList();
    }
}
