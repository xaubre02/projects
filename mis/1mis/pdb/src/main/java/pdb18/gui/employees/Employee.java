/* Employees package */
package pdb18.gui.employees;

/* IMPORTS */
import java.util.Date;

/**
 * The employee class that represents an employee in the real world.
 * 
 * @author Vit Ambroz (xambro15@stud.fit.vutbr.cz)
 */
public class Employee {
    private String birthNum;
    private String name;
    private String surname;
    private java.util.Date birthDate;
    private java.util.Date validFrom;
    private java.util.Date validTo;
    
    /**
     * Construct a new Employee object with initial values
     */
    public Employee() {
        birthNum = null;
        name = null;
        surname = null;
        birthDate = null;
        validFrom = null;
        validTo = null;
    }
    
    /**
     * Construct a new Employee object from the provided values
     *
     * @param bnum birth number(id of employee)
     * @param n name of employee
     * @param sn surname of employee
     * @param bd birth date of employee
     * @param vf date the employee is valid from
     * @param vt date the employee is valid to
     */
    public Employee(String bnum, String n, String sn, Date bd, Date vf, Date vt) {
        birthNum = bnum;
        name = n;
        surname = sn;
        birthDate = bd;
        validFrom = vf;
        validTo = vt;
    }
    
    
    /**
     * Get the birth number of employee
     *
     * @return birth number of employee
     */
    public String getBirthNum() {
        return birthNum;
    }
    
    /**
     * Get the name of employee
     *
     * @return name of employee
     */
    public String getName() {
        return name;
    }
    
    /**
     * Get the surname of employee
     *
     * @return surname of employee
     */
    public String getSurname() {
        return surname;
    }
    
    /**
     * Get the birth date of employee
     *
     * @return birth date of employee
     */
    public java.util.Date getBirthDate() {
        return birthDate;
    }
    
    /**
     * Get the date validFrom of employee
     *
     * @return date validFrom of employee
     */
    public java.util.Date getValidFrom() {
        return validFrom;
    }
    
    /**
     * Get the date validTo of employee
     *
     * @return date validTo of employee
     */
    public java.util.Date getValidTo() {
        return validTo;
    }
    
    
    /**
     * Set the birth number of employee
     *
     * @param bnum new value of birth number
     */
    public void setBirthNum(String bnum) {
        birthNum = bnum;
    }
    
    /**
     * Set the name of employee
     *
     * @param n new value of name
     */
    public void setName(String n) {
        name = n;
    }
    
    /**
     * Set the surname of employee
     *
     * @param sn new value of surname
     */
    public void setSurname(String sn) {
        surname = sn;
    }
    
    /**
     * Set the birth date of employee
     *
     * @param bd new value of birth date
     */
    public void setBirthDate(Date bd) {
        birthDate = bd;
    }
    
    /**
     * Set the validFrom date of employee
     *
     * @param vf new value of validFrom date
     */
    public void setValidFrom(Date vf) {
        validFrom = vf;
    }
    
    /**
     * Set the validTo date of employee
     *
     * @param vt new value of validTo date
     */
    public void setValidTo(Date vt) {
        validTo = vt;
    }
}
