package cz.vut.fit.pis.bakery.data;

import java.io.Serializable;
import java.util.Objects;

/**
 * @author David Bednařík
 * Class implements ID for link many to many between 
 * Pastry and Material.
 */
public class PastryMaterialId implements Serializable {
	private static final long serialVersionUID = 1L;
    private int pastry;
    private int material;
    
    public int getPastryId() 
	{
		return this.pastry;
	}

	public void setPastryId(int pastryId) 
	{
		this.pastry = pastryId;
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
    	if (other instanceof PastryMaterialId) {
    		PastryMaterialId pm = (PastryMaterialId) other;
    		boolean res = 
    			pm.pastry == this.pastry && 
    			pm.material == this.material;
            return res;
    	}
        else {
            return false;
        }
    }
 
    @Override
    public int hashCode() {
        return Objects.hash(pastry, material);
    }
}
