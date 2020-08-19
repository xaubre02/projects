package cz.vut.fit.pis.bakery.data;

import java.lang.String;
import java.util.ArrayList;
import java.util.List;

import javax.json.bind.annotation.JsonbTransient;
import javax.persistence.*;
import static javax.persistence.GenerationType.IDENTITY;
import static javax.persistence.CascadeType.ALL;
import static javax.persistence.FetchType.EAGER;

/**
 * @author David Bednařík
 * Entity implementation class for Entity: Material
 */
@Entity
@Table(name = "Material")
public class Material {
	@Id
	@GeneratedValue(strategy = IDENTITY)
	private int materialId;
	private String name;
	@OneToMany(cascade = { ALL }, mappedBy = "material", fetch = EAGER)
	@JsonbTransient
	private List<PastryMaterial> pastries;

	public Material() 
	{
		pastries = new ArrayList<PastryMaterial>();
	}   
	
	public boolean equals(Object other)
    {
        if (other instanceof Material)
            return ((Material) other).getMaterialId() == materialId;
        else
            return false;
    }
	
	public int getMaterialId() 
	{
		return this.materialId;
	}

	public void setMaterialId(int materialId) 
	{
		this.materialId = materialId;
	}   
	
	public String getName() 
	{
		return this.name;
	}

	public void setName(String name) 
	{
		this.name = name;
	}
	
	public List<PastryMaterial> getPastries() 
	{
		return pastries;
	}
	
	public void setPastries(List<PastryMaterial> pastries) 
	{
		this.pastries = pastries;
	}
}
