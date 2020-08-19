package cz.vut.fit.pis.bakery.service;

import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;

import cz.vut.fit.pis.bakery.data.PastryMaterial;

/**
 * The class manages materials of pastry. It allows to 
 * update, insert and delete materials of pastry.
 * 
 * @author David Bednařík
 */
@Stateless
public class PastryMaterialManager {
	@PersistenceContext
    private EntityManager em;
    
    public PastryMaterial save(PastryMaterial pm)
    {
    	return em.merge(pm);
    }
    
    public void remove(PastryMaterial pm)
    {
    	em.remove(pm);
    }
}
