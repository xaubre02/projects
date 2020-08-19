package cz.vut.fit.pis.bakery.service;

import java.util.List;

import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.NoResultException;
import javax.persistence.PersistenceContext;

import cz.vut.fit.pis.bakery.data.PastryCategory;

/**
 * The class manages pastry category entities. It allows to get, 
 * update, insert and delete pastry categories.
 * 
 * @author David Bednařík
 */
@Stateless
public class PastryCategoryManager {
	@PersistenceContext
    private EntityManager em;

	public PastryCategory find(int categoryId)
    {
    	return em.find(PastryCategory.class, categoryId);
    }
	
	public PastryCategory findByName(String name) {
		try {
			return (PastryCategory)em.createQuery("SELECT pc FROM PastryCategory pc WHERE pc.name = :val")
					.setParameter("val", name)
					.getSingleResult();
		}
		catch (NoResultException ex) {
			return null;
		}
	}
	
    public PastryCategory save(PastryCategory category)
    {
    	return em.merge(category);
    }
	
    public void remove(PastryCategory category)
    {
    	em.remove(category);
    	em.flush();
    }
    
	public List<PastryCategory> findAll()
    {
    	return em.createQuery("SELECT ms FROM PastryCategory ms", PastryCategory.class).getResultList();
    }
}
