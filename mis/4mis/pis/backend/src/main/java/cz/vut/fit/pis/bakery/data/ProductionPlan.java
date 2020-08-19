package cz.vut.fit.pis.bakery.data;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import javax.json.bind.annotation.JsonbTransient;
import javax.persistence.*;
import static javax.persistence.CascadeType.ALL;
import static javax.persistence.FetchType.EAGER;
import static javax.persistence.GenerationType.IDENTITY;

/**
 * @author Martin Fišer
 * Entity implementation class for Entity: ProductionPlan
 */
@Entity
@Table(name = "ProductionPlan")
public class ProductionPlan {
	@Id
	@GeneratedValue(strategy = IDENTITY)
	private int planId;
	@Column(unique=true)
	private Date productionDate;
	private boolean accepted;
	@OneToMany(cascade = { ALL }, mappedBy = "plan", fetch = EAGER, orphanRemoval = true)
	@JsonbTransient
	private List<ProductionPlanItem> items;

	public ProductionPlan()
	{
		items= new ArrayList<ProductionPlanItem>();
	}

	public boolean equals(Object other)
	{
		if (other instanceof ProductionPlan)
			return ((ProductionPlan) other).getPlanId() == planId;
		else
			return false;
	}

	public int getPlanId()
	{
		return planId;
	}

	public void setPlanId(int planId)
	{
		this.planId = planId;
	}

	public Date getProductionDate()
	{
		return productionDate;
	}

	public void setProductionDate(Date productionDate)
	{
		this.productionDate = productionDate;
	}

	public boolean getAccepted()
	{
		return accepted;
	}

	public void setAccepted(boolean accepted)
	{
		this.accepted = accepted;
	}

	public List<ProductionPlanItem> getItems()
	{
		return items;
	}

	public void setItems(List<ProductionPlanItem> items)
	{
		this.items = items;
	}
	
	public void addItem(ProductionPlanItem item)
	{
		this.items.add(item);
	}
}
