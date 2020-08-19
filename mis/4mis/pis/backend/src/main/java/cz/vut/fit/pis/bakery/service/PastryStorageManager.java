package cz.vut.fit.pis.bakery.service;

import java.util.List;

import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;

import cz.vut.fit.pis.bakery.data.PastryStorage;

/**
 * @author David Bednařík
 */
@Stateless
public class PastryStorageManager {
	@PersistenceContext
    private EntityManager em;

	public PastryStorage find(int pastryId)
    {
    	return em.find(PastryStorage.class, pastryId);
    }
	
    public void save(PastryStorage pastryStorage)
    {
    	em.merge(pastryStorage);
    }
    
    public void saveList(List<PastryStorage> pss)
    {
		for (int i = 0; i < pss.size(); i++)
			save(pss.get(i));
    }
	
    public void remove(PastryStorage pastryStorage)
    {
    	em.remove(pastryStorage);
    	em.flush();
    }
    
    public void removeList(List<PastryStorage> pss)
    {
		for (int i = 0; i < pss.size(); i++)
			remove(pss.get(i));
    }

	public List<PastryStorage> findAll()
    {
    	return em.createQuery("SELECT ps FROM PastryStorage ps", PastryStorage.class).getResultList();
    }
}
