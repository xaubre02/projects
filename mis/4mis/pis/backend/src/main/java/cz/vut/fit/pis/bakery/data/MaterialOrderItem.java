package cz.vut.fit.pis.bakery.data;

import static javax.persistence.FetchType.EAGER;

import javax.persistence.Entity;
import javax.persistence.Id;
import javax.persistence.IdClass;
import javax.persistence.ManyToOne;
import javax.persistence.Table;

@Entity
@IdClass(MaterialOrderItemId.class)
@Table(name = "MaterialOrderItem")
public class MaterialOrderItem {
	@Id
	@ManyToOne(fetch = EAGER)
	private Material material;
	@Id
	@ManyToOne(fetch = EAGER)
	private MaterialOrder order;
	private double amount;
	private double cost;
	
	public MaterialOrderItem() {
		material = null;
		order = null;
		amount = 0.0;
		cost = 0.0;
	}

	public Material getMaterial() 
	{
		return material;
	}

	public void setMaterial(Material material) 
	{
		this.material = material;
	}

	public MaterialOrder getOrder() 
	{
		return order;
	}

	public void setOrder(MaterialOrder order)
	{
		this.order = order;
	}

	public double getAmount()
	{
		return amount;
	}

	public void setAmount(double amount)
	{
		this.amount = amount;
	}

	public double getCost()
	{
		return cost;
	}

	public void setCost(double cost)
	{
		this.cost = cost;
	}
}
