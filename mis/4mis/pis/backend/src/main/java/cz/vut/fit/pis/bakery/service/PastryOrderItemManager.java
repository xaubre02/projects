package cz.vut.fit.pis.bakery.service;

import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;

import cz.vut.fit.pis.bakery.data.PastryOrderItem;

/**
 * @author Martin Fišer
 */
@Stateless
public class PastryOrderItemManager {
	@PersistenceContext
	private EntityManager em;

	public void save(PastryOrderItem oi)
	{
		em.merge(oi);
	}

	public void remove(PastryOrderItem oi)
	{
		em.remove(oi);
		em.flush();
	}
}
