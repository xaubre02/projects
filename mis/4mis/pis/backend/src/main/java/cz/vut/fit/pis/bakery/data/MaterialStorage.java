package cz.vut.fit.pis.bakery.data;

import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.EAGER;

import javax.persistence.Entity;
import javax.persistence.Id;
import javax.persistence.OneToOne;
import javax.persistence.Table;

/**
 * @author David Bednařík
 * Entity implementation class for Entity: Material Storage
 */
@Entity
@Table(name = "MaterialStorage")
public class MaterialStorage {
	@Id
	@OneToOne(cascade = { PERSIST, MERGE }, fetch = EAGER)
	private Material material;
	private double amount;
	
	public Material getMaterial() 
	{
		return material;
	}
	
	public void setMaterial(Material material) 
	{
		this.material = material;
	}
	
	public double getAmount() 
	{
		return amount;
	}
	
	public void setAmount(double amount) 
	{
		this.amount = amount;
	}
	
	
}
