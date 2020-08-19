package cz.vut.fit.pis.bakery.service;

import java.util.List;

import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;

import cz.vut.fit.pis.bakery.data.MaterialStorage;


/**
 * The class manages materials in storage. It allows to get, 
 * update, insert and delete materials in storage.
 * 
 * @author David Bednařík
 */
@Stateless
public class MaterialStorageManager {
	@PersistenceContext
    private EntityManager em;

	public MaterialStorage find(int materialId)
    {
    	return em.find(MaterialStorage.class, materialId);
    }
	
    public MaterialStorage save(MaterialStorage materialStorage)
    {
    	return em.merge(materialStorage);
    }
    
    public void saveList(List<MaterialStorage> mss)
    {
		for (int i = 0; i < mss.size(); i++)
			save(mss.get(i));
    }
	
    public void remove(MaterialStorage materialStorage)
    {
    	em.remove(materialStorage);
    	em.flush();
    }
    
	public List<MaterialStorage> findAll()
    {
    	return em.createQuery("SELECT ms FROM MaterialStorage ms", MaterialStorage.class).getResultList();
    }
}
