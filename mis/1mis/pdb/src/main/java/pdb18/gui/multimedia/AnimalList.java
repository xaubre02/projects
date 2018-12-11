/* Multimedia package */
package pdb18.gui.multimedia;

/* IMPORTS */
import java.util.ArrayList;

import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.sql.Connection;
import java.sql.ResultSet;


/**
 * The AnimalList class extends ArrayList of animals by a fill method, that initializes the list.
 * 
 *  @author Tomas Aubrecht (xaubre02@stud.fit.vutbr.cz)
 */
public class AnimalList extends ArrayList<Animal> {
    /* SQL Querry */
    private static final String SQL_INIT_ALL     = "SELECT cislo_zvirete, cislo_druhu, cislo_vybehu, jmeno, validni_od, validni_do FROM zvire";
    private static final String SQL_CURRENT      = " WHERE validni_do IS NULL";
    private static final String SQL_PAST         = " WHERE validni_do IS NOT NULL";
    
    /**
     * Fills itself with animals from the database.
     *
     * @param conn database connection
     * @param comboViewSelected temporal selection filter
     * @param comboEnclosureFilter filter selection enclosure
     * @throws SQLException SQL error
     */
    public void fill(Connection conn, int comboViewSelected, int comboEnclosureFilter) throws SQLException {
        // proper initialization
        String query = SQL_INIT_ALL;
        
        if (comboViewSelected == 0) {
            // check if enclosure filter was specified
            if (comboEnclosureFilter != 0) {
                query += " WHERE cislo_vybehu = " + comboEnclosureFilter;
            }
        }
        else {
            // filter only current valid animals
            if (comboViewSelected == 1) {
                query += SQL_CURRENT;
            }
            // filter only past (invalid) animals
            else if (comboViewSelected == 2) {
                query += SQL_PAST;
            }
            
            // check if enclosure filter was specified
            if (comboEnclosureFilter != 0) {
                query += " AND cislo_vybehu = " + comboEnclosureFilter;
            }
        }
        
        // sort
        query += " ORDER BY cislo_zvirete";
        
        try (PreparedStatement pstmt = conn.prepareStatement(query)) {
            try (ResultSet resSet = pstmt.executeQuery()) {
                /* fill the list with animals */
                while (resSet.next()) {
                    this.add(new Animal(conn,
                                        resSet.getInt   ("cislo_zvirete"),
                                        resSet.getInt   ("cislo_druhu"),
                                        resSet.getInt   ("cislo_vybehu"),
                                        resSet.getString("jmeno"),
                                        resSet.getDate  ("validni_od"),
                                        resSet.getDate  ("validni_do")));
                }
            }
        }
    }
}
