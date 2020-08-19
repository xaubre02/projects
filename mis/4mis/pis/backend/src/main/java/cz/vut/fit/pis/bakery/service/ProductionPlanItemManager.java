package cz.vut.fit.pis.bakery.service;

import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;

import cz.vut.fit.pis.bakery.data.ProductionPlanItem;

/**
 * @author Martin Fišer
 */
@Stateless
public class ProductionPlanItemManager {
	@PersistenceContext
	private EntityManager em;

	public void save(ProductionPlanItem ppi)
	{
		em.merge(ppi);
	}

	public void remove(ProductionPlanItem ppi)
	{
		em.remove(ppi);
		em.flush();
	}
}
