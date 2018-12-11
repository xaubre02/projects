/* Multimedia package */
package pdb18.gui.multimedia;

/* IMPORTS */
import java.sql.CallableStatement;
import java.util.ArrayList;

import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Date;

/**
 * The animal class that represents an animal in the real world.
 * 
 * @author Tomas Aubrecht (xaubre02@stud.fit.vutbr.cz)
 */
public class Animal {
    /* SQL Queries */
    private static final String SQL_INSERT                 = "INSERT INTO zvire (cislo_zvirete, cislo_druhu, cislo_vybehu, jmeno, validni_od, validni_do, image) VALUES(?, ?, ?, ?, ?, ?, ORDImage.init())";
    private static final String SQL_DELETE                 = "DELETE FROM zvire WHERE cislo_zvirete = ?";
    private static final String SQL_GET_MAX_ID             = "SELECT MAX(cislo_zvirete) FROM Zvire";
    private static final String SQL_GET_ALL_IDS            = "SELECT cislo_zvirete FROM zvire ORDER BY cislo_zvirete";
 
    private static final String SQL_GET_ALL_SPECIES        = "SELECT cislo_druhu, druh FROM druh_zvirete ORDER BY cislo_druhu";
    private static final String SQL_GET_VALID_ENCLOSURES   = "SELECT cislo_vybehu, typ FROM vybeh WHERE validni_do IS NULL ORDER BY cislo_vybehu";
    private static final String SQL_GET_ALL_ENCLOSURES     = "SELECT v1.cislo_vybehu, v1.typ, v1.validni_od FROM vybeh v1 "
                                                           + "WHERE v1.validni_od = (SELECT MAX(v2.validni_od) FROM vybeh v2 WHERE v1.cislo_vybehu = v2.cislo_vybehu)";
    
    
    private final Connection conn;
    private              int animalID;
    private              int speciesID;
    private              int enclosureID;
    private           String name;
    private             Date valid_from;
    private             Date valid_to;
    private          Picture picture;
    
    /**
     * Construct a new Animal object from the provided information.
     *
     * @param conn        database connection
     * @param speciesID   species ID
     * @param enclosureID enclosure ID
     * @param name        name of the animal
     * @param valid_from  valid_from date of the animal
     * @param valid_to    date of valid_to of the animal
     */
    public Animal(Connection conn,
                         int speciesID, 
                         int enclosureID,
                      String name, 
                        Date valid_from, 
                        Date valid_to) {
        this.conn        = conn;
        this.animalID    = -1;
        this.speciesID   = speciesID;
        this.enclosureID = enclosureID;
        this.name        = name;
        this.valid_from  = valid_from;
        this.valid_to    = valid_to;
        this.picture     = null;
    }
    
    /**
     * Construct a new Animal object from the provided information.
     *
     * @param conn        database connection
     * @param animalID    animal ID
     * @param speciesID   species ID
     * @param enclosureID enclosure ID
     * @param name        name of the animal
     * @param valid_from  valid_from date of the animal
     * @param valid_to    date of valid_to of the animal
     */
    public Animal(Connection conn,
                         int animalID,
                         int speciesID, 
                         int enclosureID,
                      String name, 
                        Date valid_from, 
                        Date valid_to) {
        this.conn        = conn;
        this.animalID    = animalID;
        this.speciesID   = speciesID;
        this.enclosureID = enclosureID;
        this.name        = name;
        this.valid_from  = valid_from;
        this.valid_to    = valid_to;
        this.picture     = new Picture(conn, animalID);
    }
    
    /**
     * Set the species of the animal.
     *
     * @param ID new species ID
     */
    public void setSpecies(int ID) {
        this.speciesID = ID;
    }
    
    /**
     * Set the enclosure of the animal.
     *
     * @param ID new species ID
     */
    public void setEnclosure(int ID) {
        this.enclosureID = ID;
    }
    
    /**
     * Set the name of the animal.
     *
     * @param name new animal name
     */
    public void setName(String name) {
        this.name = name;
    }
    
    /**
     * Set the date of the valid_from of animal.
     *
     * @param date date of valid_from
     */
    public void setValid_from(Date date) {
        this.valid_from = date;
    }
    
    /**
     * Set the date of the valid_to of animal.
     *
     * @param date date of valid_to
     */
    public void setValid_to(Date date) {
        this.valid_to = date;
    }
    
    /**
     * Get the ID of the animal.
     *
     * @return ID of the animal
     */
    public int getAnimalID() {
        return this.animalID;
    }
    
    /**
     * Get the ID of the species of the animal.
     *
     * @return ID of the species
     */
    public int getSpeciesID() {
        return this.speciesID;
    }
    
    /**
     * Get the ID of the enclosure of the animal.
     *
     * @return ID of the enclosure
     */
    public int getEnclosureID() {
        return this.enclosureID;
    }
    
    /**
     * Get the name of the animal.
     *
     * @return name of the animal
     */
    public String getName(){
        return this.name;
    }
    
    /**
     * Get the valid_from date of the animal.
     *
     * @return valid_from date
     */
    public Date getValid_from() {
        return this.valid_from;
    }
    
    /**
     * Get the valid_to date of the animal.
     *
     * @return valid_to date
     */
    public Date getValid_to() {
        return this.valid_to;
    }
    
    /**
     * Get the picture of the animal.
     *
     * @return valid_to date of the animal
     */
    public Picture getPicture() {
        return this.picture;
    }
    
    /**
     * Returns the name of the animal.
     *
     * @return animal name
     */
    @Override
    public String toString(){
        return this.getName();
    }

    /**
     * Save properties of the animal to a database.
     *
     * @throws SQLException SQL error
     */
    public void saveToDB() throws SQLException {
        try (PreparedStatement pstmt = conn.prepareStatement(SQL_INSERT)) {
            // set the animal properties
            pstmt.setInt(1, animalID);
            pstmt.setInt(2, speciesID);
            pstmt.setInt(3, enclosureID);
            pstmt.setString(4, name);
            pstmt.setDate(5, valid_from);
            pstmt.setDate(6, valid_to);
            // execute the query
            pstmt.executeUpdate();
            picture = new Picture(conn, animalID);
        }
    }
    
    /**
     * Get the maximum ID of the animal from the database.
     *
     * @return maximum ID of the animal
     * @throws SQLException SQL error
     */
    private int getAnimalMaxIDFromDB() throws SQLException {
        try (PreparedStatement pstmt = conn.prepareStatement(SQL_GET_MAX_ID)) {
            pstmt.executeUpdate();
            try (ResultSet resSet = pstmt.executeQuery()) {
                if (resSet.next()) {
                    return resSet.getInt(1);
                }
                return 42;
            }
        }
    }

    /**
     * Update the properties of the animal in the database.
     *
     * @param connection database connection
     * @throws SQLException SQL error
     */
    public void updateInDB(Connection connection) throws SQLException {
        // valid_to was changed -> call procedure delete with specified date()
        if (valid_to != null) {
            // call procedure deleteZvire(n_cislo_zvirete IN NUMBER, n_validni_do IN DATE) 
            try (CallableStatement cstmt = conn.prepareCall("BEGIN deleteZvire(?, ?); END;")) {
                cstmt.setInt(1, animalID);
                cstmt.setDate(2, valid_to);
                cstmt.execute();
            }
            return;
        }
        
        // call procedure updateZvire(n_cislo_zvirete IN NUMBER, n_validni_od IN DATE, n_jmeno IN VARCHAR, n_cislo_druhu IN NUMBER, n_cislo_vybehu IN NUMBER)
        try (CallableStatement cstmt = conn.prepareCall("BEGIN updateZvire(?, ?, ?, ?, ?); END;")) {
            cstmt.setInt(1, animalID);
            cstmt.setDate(2, valid_from);
            cstmt.setNString(3, name);
            cstmt.setInt(4, speciesID);
            cstmt.setInt(5, enclosureID);
            cstmt.execute();
        }
    }
    
    /**
     * Delete the animal from the database.
     *
     * @throws SQLException SQL error
     */
    public void deleteFromDB() throws SQLException {
        // call procedure deleteZvire(n_cislo_zvirete IN NUMBER, n_validni_do IN DATE)
        try (CallableStatement cstmt = conn.prepareCall("BEGIN deleteZvire(?, to_char(sysdate,'dd-mm-rr')); END;")) {
                cstmt.setInt(1, animalID);
                cstmt.execute();
        }
    }
    
    /**
     * Completely delete the animal from the database. Do not keep any information.
     *
     * @throws SQLException SQL error
     */
    public void simpleDeleteFromDB() throws SQLException {
        // call procedure deleteZvire(n IN NUMBER)
        try (PreparedStatement pstmt = conn.prepareStatement(SQL_DELETE)) {
                pstmt.setInt(1, animalID);
                pstmt.execute();
        }
    }

    /**
     * Get all animal species from the database and return them as a list of IDNamePair.
     *
     * @param conn database connection
     * @return list of all animal species
     * @throws SQLException SQL error
     */
    public static ArrayList<IDNamePair> getAllAnimalSpecies(Connection conn) throws SQLException {
        try (PreparedStatement pstmt = conn.prepareStatement(Animal.SQL_GET_ALL_SPECIES)) {
            try (ResultSet resSet = pstmt.executeQuery()) {
                /* fill the list with animal species */
                ArrayList<IDNamePair> species = new ArrayList<>();
                while (resSet.next()) {
                    species.add(new IDNamePair(resSet.getInt("cislo_druhu"), resSet.getString("druh")));
                }
                return species;
            }
        }
    }
    
    /**
     * Get all valid animal enclosures from the database and return them as a list of IDNamePair.
     *
     * @param conn database connection
     * @return list of all animal enclosures
     * @throws SQLException SQL error
     */
    public static ArrayList<IDNamePair> getValidAnimalEnclosures(Connection conn) throws SQLException {
        try (PreparedStatement pstmt = conn.prepareStatement(Animal.SQL_GET_VALID_ENCLOSURES)) {
            try (ResultSet resSet = pstmt.executeQuery()) {
                /* fill the list with animal enclosures that has valid_to == null */
                ArrayList<IDNamePair> enclosures = new ArrayList<>();
                while (resSet.next()) {
                    enclosures.add(new IDNamePair(resSet.getInt("cislo_vybehu"), resSet.getString("typ")));
                }
                return enclosures;
            }
        }
    }
    
    /**
     * Get all animal enclosures from the database and return them as a list of IDNamePair.
     *
     * @param conn database connection
     * @return list of all animal enclosures
     * @throws SQLException SQL error
     */
    public static ArrayList<IDNamePair> getAllAnimalEnclosures(Connection conn) throws SQLException {
        try (PreparedStatement pstmt = conn.prepareStatement(Animal.SQL_GET_ALL_ENCLOSURES)) {
            try (ResultSet resSet = pstmt.executeQuery()) {
                /* fill the list with animal enclosures */
                ArrayList<IDNamePair> enclosures = new ArrayList<>();
                enclosures.add(new IDNamePair(0, "Libovolný"));
                
                while (resSet.next()) {
                    enclosures.add(new IDNamePair(resSet.getInt("cislo_vybehu"), resSet.getString("typ")));
                }
                return enclosures;
            }
        }
    }
    
    /**
     * Check if required ID is available in the database. If so, return it, otherwise get the first available animal ID.
     *
     * @param conn database connection
     * @param requiredID required ID by user
     * @return list of all animal enclosures
     * @throws SQLException SQL error
     */
    public static int getFirstAvailableID(Connection conn, int requiredID) throws SQLException {
        int firstAvail = 1;
        int usedID;
        boolean reqIsFree = true;
        try (PreparedStatement pstmt = conn.prepareStatement(SQL_GET_ALL_IDS)) {
            pstmt.executeUpdate();
            try (ResultSet resSet = pstmt.executeQuery()) {
                while (resSet.next()) {
                    usedID = resSet.getInt(1);
                    if (usedID == firstAvail) {
                        firstAvail++;
                    }
                    if (usedID == requiredID) {
                        reqIsFree = false;
                    }
                }
            }
        }
        
        if (reqIsFree) {
            return requiredID;
        } 
        else {
            return firstAvail;
        }
    }
}
