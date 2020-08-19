package cz.vut.fit.pis.bakery.data;

import javax.persistence.*;

import static javax.persistence.FetchType.EAGER;

@Entity
@IdClass(PastryMaterialId.class)
@Table(name = "PastryMaterial")
public class PastryMaterial {
	@Id
	@ManyToOne(fetch = EAGER)
	private Pastry pastry;
	@Id
	@ManyToOne(fetch = EAGER)
	private Material material;
	private double amount;

	public PastryMaterial() {
		pastry = null;
		material = null;
		amount = 0.0;
	}

	public Pastry getPastry() {
		return pastry;
	}

	public void setPastry(Pastry pastry) {
		this.pastry = pastry;
	}

	public Material getMaterial() {
		return material;
	}

	public void setMaterial(Material material) {
		this.material = material;
	}

	public double getAmount() {
		return amount;
	}

	public void setAmount(double amount) {
		this.amount = amount;
	}

	public void addAmount(double amount) {
		this.amount += amount;
	}
}
