package cz.vut.fit.pis.bakery.data;

import java.lang.String;
import java.util.ArrayList;
import java.util.List;

import javax.json.bind.annotation.JsonbTransient;
import javax.persistence.*;
import static javax.persistence.GenerationType.IDENTITY;
import static javax.persistence.CascadeType.ALL;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.FetchType.EAGER;

/**
 * @author David Bednařík
 * Entity implementation class for Entity: Pastry
 */
@Entity
@Table(name = "Pastry")
public class Pastry {
	@Id
	@GeneratedValue(strategy = IDENTITY)
	private int pastryId;
	private String name;
	private double cost;
	@ManyToOne(cascade = { PERSIST, MERGE }, fetch = EAGER)
	private PastryCategory category;
	@OneToMany(cascade = { ALL }, mappedBy = "pastry", fetch = EAGER, orphanRemoval = true)
	@JsonbTransient
	private List<PastryMaterial> materials;

	public Pastry() 
	{
		materials = new ArrayList<PastryMaterial>();
	}   
	
	public boolean equals(Object other)
    {
        if (other instanceof Pastry)
            return ((Pastry) other).getPastryId() == pastryId;
        else
            return false;
    }
	
	public int getPastryId() 
	{
		return this.pastryId;
	}

	public void setPastryId(int pastryId) 
	{
		this.pastryId = pastryId;
	}  
	
	public String getName() 
	{
		return this.name;
	}

	public void setName(String name) 
	{
		this.name = name;
	}   
	
	public double getCost() 
	{
		return this.cost;
	}

	public void setCost(double cost) 
	{
		this.cost = cost;
	}
	
	public PastryCategory getCategory()
	{
		return category;
	}
	
	public void setCategory(PastryCategory category)
	{
		this.category = category;
	}
	
	public List<PastryMaterial> getMaterials() 
	{
		return materials;
	}
	
	public void setMaterials(List<PastryMaterial> materials) 
	{
		this.materials = materials;
	}
}
