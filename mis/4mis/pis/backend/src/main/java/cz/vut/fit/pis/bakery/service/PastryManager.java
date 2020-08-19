package cz.vut.fit.pis.bakery.service;

import java.util.List;

import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.NoResultException;
import javax.persistence.PersistenceContext;

import cz.vut.fit.pis.bakery.data.Pastry;
import cz.vut.fit.pis.bakery.data.PastryCategory;

/**
 * The class manages pastry entities. It allows to get, 
 * update, insert and delete pastry.
 * 
 * @author David Bednařík
 */
@Stateless
public class PastryManager {
	@PersistenceContext
    private EntityManager em;

    public Pastry find(int pastryId)
    {
    	return em.find(Pastry.class, pastryId);
    }
    
    public Pastry findByName(String name) {
		try {
			return (Pastry)em.createQuery("SELECT p FROM Pastry p WHERE p.name = :val")
					.setParameter("val", name)
					.getSingleResult();
		}
		catch (NoResultException ex) {
			return null;
		}
	}
    
    public Pastry save(Pastry pastry)
    {
    	return em.merge(pastry);
    }
    
    public void refresh(Pastry pastry)
    {
    	em.flush();
    	em.refresh(pastry);
    }
	
    public void remove(Pastry pastry)
    {
    	em.remove(pastry);
    	em.flush();
    }
    
	public List<Pastry> findAll()
    {
    	return em.createQuery("SELECT p FROM Pastry p", Pastry.class).getResultList();
    }
	
	public List<Pastry> findAllInCategory(PastryCategory category)
	{
		return em.createQuery("SELECT p FROM Pastry p WHERE p.category = :val", Pastry.class)
				.setParameter("val", category)
				.getResultList();
	}
}
