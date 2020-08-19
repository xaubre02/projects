package cz.vut.fit.pis.bakery.service;

import java.util.Calendar;
import java.util.Date;
import java.util.List;

import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;

import cz.vut.fit.pis.bakery.data.User;
import cz.vut.fit.pis.bakery.data.PastryOrder;
import cz.vut.fit.pis.bakery.data.PastryOrderItem;
import cz.vut.fit.pis.bakery.back.PastryOrderState;

/**
 * @author Martin Fišer
 */
@Stateless
public class PastryOrderManager {
	@PersistenceContext
	private EntityManager em;

	public PastryOrder find(int orderId)
	{
		return em.find(PastryOrder.class, orderId);
	}

	public List<PastryOrder> findByState(PastryOrderState state)
	{
		return em.createQuery("SELECT o FROM PastryOrder o WHERE o.state = :val", PastryOrder.class)
				.setParameter("val", state.toString())
				.getResultList();
	}

	public List<PastryOrder> findByDeliveryDate(Date deliveryDate)
	{
		return em.createQuery("SELECT o FROM PastryOrder o WHERE o.deliveryDate = :val", PastryOrder.class)
				.setParameter("val", deliveryDate)
				.getResultList();
	}

	public List<PastryOrderItem> findToProductionPlan(Date productionDate)
	{
		// tomorrow's orders
		Calendar c = Calendar.getInstance();
		c.setTime(productionDate);
		c.add(Calendar.DATE, 1);
		Date deliveryDate = c.getTime();

		return em.createQuery("SELECT o.items FROM PastryOrder o WHERE o.deliveryDate = :dd AND o.state = :s", PastryOrderItem.class)
				.setParameter("dd", deliveryDate)
				.setParameter("s", "created")
				.getResultList();
	}

	public List<PastryOrder> findByCustomer(User user)
	{
		return em.createQuery("SELECT o FROM PastryOrder o WHERE o.customer = :val", PastryOrder.class)
				.setParameter("val", user)
				.getResultList();
	}

	public List<PastryOrder> findByCustomerAndState(User user, PastryOrderState state) {
		return em.createQuery("SELECT o FROM PastryOrder o WHERE o.customer = :c AND o.state = :s", PastryOrder.class)
				.setParameter("c", user)
				.setParameter("s", state.toString())
				.getResultList();
	}

	public PastryOrder save(PastryOrder order)
	{
		PastryOrder res = em.merge(order);
		em.flush();
		return res;
	}

	public void refresh(PastryOrder order)
	{
		em.flush();
		em.refresh(order);
	}

	public void remove(PastryOrder order)
	{
		em.remove(order);
		em.flush();
	}

	public List<PastryOrder> findAll()
	{
		return em.createQuery("SELECT o FROM PastryOrder o", PastryOrder.class).getResultList();
	}
}
