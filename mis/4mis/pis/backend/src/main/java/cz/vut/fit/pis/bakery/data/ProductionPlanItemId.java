package cz.vut.fit.pis.bakery.data;

import java.io.Serializable;
import java.util.Objects;

/**
 * @author Martin Fišer
 * Class implements ID for link many to many between
 * ProductionPlan and Pastry.
 */
public class ProductionPlanItemId implements Serializable {
	private static final long serialVersionUID = 1L;
	private int plan;
	private int pastry;

	public int getProductionPlanId()
	{
		return this.plan;
	}

	public void setProductionPlanId(int planId)
	{
		this.plan = planId;
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
		if (other instanceof ProductionPlanItemId) {
			ProductionPlanItemId ppi = (ProductionPlanItemId) other;
			boolean res =
					ppi.plan == this.plan &&
					ppi.pastry == this.pastry;
			return res;
		}
		else {
			return false;
		}
	}

	@Override
	public int hashCode() {
		return Objects.hash(plan, pastry);
	}
}
