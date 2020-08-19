package cz.vut.fit.pis.bakery.data;

import static javax.persistence.CascadeType.ALL;
import static javax.persistence.FetchType.EAGER;
import static javax.persistence.GenerationType.IDENTITY;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;

import javax.json.bind.annotation.JsonbDateFormat;
import javax.json.bind.annotation.JsonbTransient;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.Id;
import javax.persistence.OneToMany;
import javax.persistence.Table;

/**
 * @author David Bednařík
 * Entity implementation class for Entity: MaterialOrder
 */
@Entity
@Table(name = "MaterialOrder")
public class MaterialOrder {
	@Id
	@GeneratedValue(strategy = IDENTITY)
	private int orderId;
	@JsonbDateFormat("yyyy-MM-dd")
    private LocalDate dateOfCreation;
	private boolean delivered;
	@OneToMany(cascade = { ALL }, mappedBy = "order", fetch = EAGER)
	@JsonbTransient
	private List<MaterialOrderItem> materials;
	
	public MaterialOrder() 
	{
		materials = new ArrayList<MaterialOrderItem>();
	}   
	
	public boolean equals(Object other)
    {
        if (other instanceof Pastry)
            return ((MaterialOrder) other).getOrderId() == orderId;
        else
            return false;
    }

	public int getOrderId() 
	{
		return orderId;
	}

	public void setOrderId(int orderId) 
	{
		this.orderId = orderId;
	}

	public LocalDate getDateOfCreation() 
	{
		return dateOfCreation;
	}

	public void setDateOfCreation(LocalDate dateOfCreation) 
	{
		this.dateOfCreation = dateOfCreation;
	}

	public boolean isDelivered() 
	{
		return delivered;
	}

	public void setDelivered(boolean delivered) 
	{
		this.delivered = delivered;
	}

	public List<MaterialOrderItem> getMaterials() 
	{
		return materials;
	}

	public void setMaterials(List<MaterialOrderItem> materials) 
	{
		this.materials = materials;
	}
}
