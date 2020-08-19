package cz.vut.fit.pis.bakery.service;

import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;

import cz.vut.fit.pis.bakery.data.MaterialOrderItem;

/**
 * The class manages items of material order. It allows to 
 * update, insert and delete items of material order.
 * 
 * @author David Bednařík
 */
@Stateless
public class MaterialOrderItemManager {
	@PersistenceContext
    private EntityManager em;
    
    public MaterialOrderItem save(MaterialOrderItem moi)
    {
    	return em.merge(moi);
    }
    
    public void remove(MaterialOrderItem moi)
    {
    	em.remove(moi);
    	em.flush();
    }
}
