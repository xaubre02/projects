package cz.vut.fit.pis.bakery.data;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import javax.json.bind.annotation.JsonbTransient;
import javax.persistence.*;
import static javax.persistence.CascadeType.ALL;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.EAGER;
import static javax.persistence.GenerationType.IDENTITY;

/**
 * @author Martin Fišer
 * Entity implementation class for Entity: PastryOrder
 */
@Entity
@Table(name = "PastryOrder")
public class PastryOrder {
	@Id
	@GeneratedValue(strategy = IDENTITY)
	private int orderId;
	private double price;
	private String state;
	private Date deliveryDate;
	@ManyToOne(cascade = { PERSIST, MERGE }, fetch = EAGER)
	private User customer;
	@OneToMany(cascade = { ALL }, mappedBy = "order", fetch = EAGER, orphanRemoval = true)
	@JsonbTransient
	private List<PastryOrderItem> items;

	public PastryOrder()
	{
		items= new ArrayList<PastryOrderItem>();
	}

	public boolean equals(Object other)
	{
		if (other instanceof PastryOrder)
			return ((PastryOrder) other).getOrderId() == orderId;
		else
			return false;
	}

	public int getOrderId() {
		return orderId;
	}

	public void setOrderId(int orderId) {
		this.orderId = orderId;
	}

	public double getPrice() {
		return price;
	}

	public void setPrice(double price) {
		this.price = price;
	}

	public String getState() {
		return state;
	}

	public void setState(String state) {
		this.state = state;
	}

	public Date getDeliveryDate() {
		return deliveryDate;
	}

	public void setDeliveryDate(Date deliveryDate) {
		this.deliveryDate = deliveryDate;
	}

	public User getCustomer() {
		return customer;
	}

	public void setCustomer(User customer) {
		this.customer = customer;
	}

	public List<PastryOrderItem> getItems() {
		return items;
	}

	public void setItems(List<PastryOrderItem> items) {
		this.items = items;
	}
}
