package cz.vut.fit.pis.bakery.service;

import java.util.List;

import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.NoResultException;
import javax.persistence.PersistenceContext;

import cz.vut.fit.pis.bakery.data.Material;

/**
 * The class manages material entities. It allows to get, 
 * update, insert and delete materials.
 * 
 * @author David Bednařík
 */
@Stateless
public class MaterialManager {
	@PersistenceContext
    private EntityManager em;

    public Material find(int materialId)
    {
    	return em.find(Material.class, materialId);
    }
    
    public Material findByName(String name) {
		try {
			return (Material)em.createQuery("SELECT m FROM Material m WHERE m.name = :val")
					.setParameter("val", name)
					.getSingleResult();
		}
		catch (NoResultException ex) {
			return null;
		}
	}
    
    public Material save(Material material)
    {
    	return em.merge(material);
    }
	
    public void remove(Material material)
    {
    	em.remove(material);
    	em.flush();
    }
    
	public List<Material> findAll()
    {
    	return em.createQuery("SELECT m FROM Material m", Material.class).getResultList();
    }
}
