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
@Table(name = "PastryStorage")
public class PastryStorage {
	@Id
	@OneToOne(cascade = { PERSIST, MERGE }, fetch = EAGER)
	private Pastry pastry;
	private int count;
	
	public Pastry getPastry() 
	{
		return pastry;
	}
	
	public void setPastry(Pastry pastry) 
	{
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