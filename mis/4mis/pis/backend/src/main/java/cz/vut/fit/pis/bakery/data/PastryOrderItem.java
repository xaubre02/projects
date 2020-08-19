package cz.vut.fit.pis.bakery.data;

import javax.persistence.*;
import static javax.persistence.FetchType.EAGER;

@Entity
@IdClass(PastryOrderItemId.class)
@Table(name = "PastryOrderItem")
public class PastryOrderItem {
	@Id
	@ManyToOne(fetch = EAGER)
	private PastryOrder order;
	@Id
	@ManyToOne(fetch = EAGER)
	private Pastry pastry;
	private int count;
	private double cost;

	public PastryOrderItem() {
		order= null;
		pastry = null;
		count= 0;
		cost = 0.0;
	}

	public PastryOrder getOrder() {
		return order;
	}

	public void setOrder(PastryOrder order) {
		this.order = order;
	}

	public Pastry getPastry() {
		return pastry;
	}

	public void setPastry(Pastry pastry) {
		this.pastry = pastry;
	}

	public int getCount() {
		return count;
	}

	public void setCount(int count) {
		this.count= count;
	}

	public double getCost() {
		return cost;
	}

	public void setCost(double cost) {
		this.cost = cost;
	}
}
