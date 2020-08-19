package cz.vut.fit.pis.bakery.data;

import java.io.Serializable;
import java.util.Objects;

/**
 * @author Martin Fišer
 * Class implements ID for link many to many between
 * PastryOrder and Pastry.
 */
public class PastryOrderItemId implements Serializable {
	private static final long serialVersionUID = 1L;
	private int order;
	private int pastry;

	public int getOrderId()
	{
		return this.order;
	}

	public void setOrderId(int orderId)
	{
		this.order = orderId;
	}

	public int getPastryId()
	{
		return this.pastry;
	}

	public void setPastryId(int pastryId)
	{
		this.pastry = pastryId;
	}

	@Override
	public boolean equals(Object other) {
		if (other instanceof PastryOrderItemId) {
			PastryOrderItemId oi = (PastryOrderItemId) other;
			boolean res =
					oi.order == this.order &&
					oi.pastry == this.pastry;
			return res;
		}
		else {
			return false;
		}
	}

	@Override
	public int hashCode() {
		return Objects.hash(order, pastry);
	}
}
