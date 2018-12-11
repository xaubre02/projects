/* Multimedia package */
package pdb18.gui.multimedia;

/**
 * The IDNamePair represents an primary key and the name of the corresponding row.
 * 
 * @author Tomas Aubrecht (xaubre02@stud.fit.vutbr.cz)
 */
public class IDNamePair {
    private final int id;
    private final String name;

    /**
     * Creates a new pair of ID and Name of the DB object.
     *
     * @param id   object ID
     * @param name object name
     */
    public IDNamePair(int id, String name) {
        this.id = id;
        this.name = name;
    }

    /**
     * Returns the ID of the DB object.
     *
     * @return object id
     */
    public int getID() {
        return this.id;
    }

    /**
     * Returns the name of the DB object.
     *
     * @return object name
     */
    public String getName() {
        return this.name;
    }

    /**
     * Returns the name of the DB object.
     *
     * @return object name
     */
    @Override
    public String toString() {
        return this.getName();
    }
}
