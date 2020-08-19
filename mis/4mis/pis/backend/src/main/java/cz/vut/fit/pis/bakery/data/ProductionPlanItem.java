package cz.vut.fit.pis.bakery.data;

import javax.persistence.*;
import static javax.persistence.FetchType.EAGER;

@Entity
@IdClass(ProductionPlanItemId.class)
@Table(name = "ProductionPlanItem")
public class ProductionPlanItem {
	@Id
	@ManyToOne(fetch = EAGER)
	private ProductionPlan plan;
	@Id
	@ManyToOne(fetch = EAGER)
	private Pastry pastry;
	private int count;

	public ProductionPlanItem() {
		plan = null;
		pastry = null;
		count = 0;
	}

	public ProductionPlan getPlan()
	{
		return plan;
	}

	public void setPlan(ProductionPlan plan)
	{
		this.plan = plan;
	}

	public Pastry getPastry()
	{
		return pastry;
	}

	public void setPastry(Pastry pastry
			) {
		this.pastry = pastry;
	}

	public int getCount()
	{
		return count;
	}

	public void setCount(int count)
	{
		this.count = count;
	}
	
	public void addCount(int count)
	{
		this.count += count;
	}
}
