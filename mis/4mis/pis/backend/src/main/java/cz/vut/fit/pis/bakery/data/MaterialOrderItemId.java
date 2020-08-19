package cz.vut.fit.pis.bakery.data;

import java.io.Serializable;
import java.util.Objects;

/**
 * @author David Bednařík
 * Class implements ID for link many to many between 
 * MaterialOrder and Material.
 */
public class MaterialOrderItemId implements Serializable {
	private static final long serialVersionUID = 1L;
    private int order;
    private int material;

	public int getOrderId() 
	{
		return this.order;
	}

	public void setOrderId(int orderId) 
	{
		this.order = orderId;
	}

	public int getMaterialId() 
	{
		return this.material;
	}

	public void setMaterialId(int materialId) 
	{
		this.material = materialId;
	}  

    @Override
    public boolean equals(Object other) {
    	if (other instanceof MaterialOrderItemId) {
    		MaterialOrderItemId oi = (MaterialOrderItemId) other;
    		boolean res = 
    			oi.order == this.order && 
    			oi.material == this.material;
            return res;
    	}
        else {
            return false;
        }
    }
 
    @Override
    public int hashCode() {
        return Objects.hash(order, material);
    }
}
