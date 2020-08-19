package cz.vut.fit.pis.bakery.service;

import java.util.Date;
import java.util.List;

import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.NoResultException;
import javax.persistence.PersistenceContext;

import cz.vut.fit.pis.bakery.data.ProductionPlan;

/**
 * @author Martin Fišer
 */
@Stateless
public class ProductionPlanManager {
	@PersistenceContext
	private EntityManager em;

	public ProductionPlan find(int planId)
	{
		return em.find(ProductionPlan.class, planId);
	}

	public ProductionPlan findByProductionDate(Date productionDate)
	{
		try {
			return em.createQuery("SELECT p FROM ProductionPlan p WHERE p.productionDate = :val", ProductionPlan.class)
					.setParameter("val", productionDate)
					.getSingleResult();
		}
		catch (NoResultException e) {
			return null;
		}
	}

	public ProductionPlan save(ProductionPlan plan)
	{
		ProductionPlan res = em.merge(plan);
		em.flush();
		return res;
	}

	public void refresh(ProductionPlan plan)
	{
		em.flush();
		em.refresh(plan);
	}

	public void remove(ProductionPlan plan)
	{
		em.remove(plan);
		em.flush();
	}

	public List<ProductionPlan> findAll()
	{
		return em.createQuery("SELECT p FROM ProductionPlan p", ProductionPlan.class).getResultList();
	}
}
