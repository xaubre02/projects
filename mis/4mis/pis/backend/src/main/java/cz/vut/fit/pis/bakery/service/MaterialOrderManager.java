package cz.vut.fit.pis.bakery.service;

import java.util.List;

import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;

import cz.vut.fit.pis.bakery.data.MaterialOrder;

/**
 * The class manages material order entities. It allows to get, 
 * update, insert and delete material orders.
 * 
 * @author David Bednařík
 */
@Stateless
public class MaterialOrderManager {
	@PersistenceContext
    private EntityManager em;

    public MaterialOrder find(int orderId)
    {
    	return em.find(MaterialOrder.class, orderId);
    }
    
    public MaterialOrder save(MaterialOrder order)
    {
    	return em.merge(order);
    }
    
    public void refresh(MaterialOrder order)
    {
    	em.flush();
    	em.refresh(order);
    }
	
    public void remove(MaterialOrder order)
    {
    	em.remove(order);
    	em.flush();
    }
    
	public List<MaterialOrder> findAll()
    {
    	return em.createQuery("SELECT mo FROM MaterialOrder mo", MaterialOrder.class).getResultList();
    }
	
	public List<MaterialOrder> findAllInState(boolean delivered)
	{
		return em.createQuery("SELECT mo FROM MaterialOrder mo WHERE mo.delivered = :val", MaterialOrder.class)
				.setParameter("val", delivered)
				.getResultList();
	}
}
