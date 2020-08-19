package cz.vut.fit.pis.bakery.data;

import static javax.persistence.CascadeType.ALL;
import static javax.persistence.FetchType.EAGER;
import static javax.persistence.GenerationType.IDENTITY;

import java.util.List;

import javax.json.bind.annotation.JsonbTransient;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.Id;
import javax.persistence.OneToMany;
import javax.persistence.Table;


/**
 * @author David Bednařík
 * Entity implementation class for Entity: PastryCategory
 */
@Entity
@Table(name = "PastryCategory")
public class PastryCategory {
	@Id
	@GeneratedValue(strategy = IDENTITY)
	private int categoryId;
	private String name;
	@OneToMany(cascade = { ALL }, fetch = EAGER, mappedBy = "category")
	@JsonbTransient
	private List<Pastry> pastries;
	
	public int getCategoryId() 
	{
		return categoryId;
	}
	
	public void setCategoryId(int categoryId) 
	{
		this.categoryId = categoryId;
	}
	
	public String getName() 
	{
		return name;
	}
	
	public void setName(String name) 
	{
		this.name = name;
	}
	
	public List<Pastry> getPastries()
	{
		return pastries;
	}
	
	public void setPastries(List<Pastry> pastries)
	{
		this.pastries = pastries;
	}
}
