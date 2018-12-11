/* Employees package */
package pdb18.gui.employees;

/* IMPORTS */
import java.util.ArrayList;

import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.sql.Connection;
import java.sql.ResultSet;

/**
 * The AnimalList class extends ArrayList of animals by a fill method, that initializes the list.
 * 
 *  @author Vit Ambroz (xambro15@stud.fit.vutbr.cz)
 */
public class EmployeeList extends ArrayList<Employee> {
    /* SQL Queries */
    private static final String SQL_INIT_ALL = "SELECT rodne_cislo, jmeno, prijmeni, datum_narozeni, validni_od, validni_do FROM zamestnanec";
    private static final String SQL_INIT_CURRENT = "SELECT rodne_cislo, jmeno, prijmeni, datum_narozeni, validni_od, validni_do FROM zamestnanec "
                                                + "WHERE validni_do IS NULL";
    private static final String SQL_INIT_PAST = "SELECT rodne_cislo, jmeno, prijmeni, datum_narozeni, validni_od, validni_do FROM zamestnanec "
                                                + "WHERE validni_do IS NOT NULL";
    
    /**
     * Fills itself with employees from the database.
     *
     * @param conn database connection
     * @param filter flag for filtering employees according temporal validity
     * @param sort flag for condition of query ORDER BY
     * @param from date the employee is valid from
     * @param to date the employee is valid to
     * @throws SQLException SQL error
     */
    public void fill(Connection conn, int filter, int sort, java.util.Date from, java.util.Date to) throws SQLException {
        // selected index 0
        String query = SQL_INIT_ALL;
        
        // filter only current employees
        if (filter == 1) {
            query = SQL_INIT_CURRENT;
        }
        else if (filter == 2) { // filter only past employees
            query = SQL_INIT_PAST;
        }
        else if (filter == 3) {
            if (from != null && to != null) {
                query += " WHERE validni_od < ? AND (validni_do > ? OR validni_do IS NULL)";
            }
            else if (from != null) {
                query += " WHERE validni_do > ? OR validni_do IS NULL";
            }
            else if (to != null) {
                query += " WHERE validni_od < ?";
            }
        }
        
        // add sort condition
        switch (sort){
            case 0:
                query += " ORDER BY prijmeni";
                break;
            case 1:
                query += " ORDER BY jmeno";
                break;
            case 2:
                query += " ORDER BY rodne_cislo";
                break;
            case 3:
                query += " ORDER BY datum_narozeni";
                break;
            case 4:
                query += " ORDER BY validni_od";
                break;
            default:
                break;
        }
        
        try (PreparedStatement pstmt = conn.prepareStatement(query)) {
            // add temporal interval to prepared statement when there was specified interval
            if (filter == 3) {
                if (from != null && to != null) {
                    pstmt.setDate(1, new java.sql.Date(to.getTime()));
                    pstmt.setDate(2, new java.sql.Date(from.getTime()));
                }
                else if (from != null) {
                    pstmt.setDate(1, new java.sql.Date(from.getTime()));
                }
                else if (to != null) {
                    pstmt.setDate(1, new java.sql.Date(to.getTime()));
                }
            }
            
            try (ResultSet rset = pstmt.executeQuery()) {
                /* fill the list with animals */
                while (rset.next()) {
                    String birthNum = rset.getNString("rodne_cislo");
                    String name = rset.getNString("jmeno");
                    String surname = rset.getNString("prijmeni");

                    java.sql.Date birthDate = rset.getDate("datum_narozeni");
                    java.util.Date birthDateConverted = new java.util.Date(birthDate.getTime());

                    java.sql.Date validFrom = rset.getDate("validni_od");
                    java.util.Date validFromConverted = new java.util.Date(validFrom.getTime());

                    java.sql.Date validTo = rset.getDate("validni_do");
                    java.util.Date validToConverted = null;
                    if (validTo != null) {
                        validToConverted = new java.util.Date(validTo.getTime());
                    }
                        
                    // add to arraylist
                    this.add(new Employee(birthNum, name, surname, birthDateConverted, validFromConverted, validToConverted));
                }
            }
        }
    }
}
