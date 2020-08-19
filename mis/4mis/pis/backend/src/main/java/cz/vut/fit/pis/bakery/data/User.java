package cz.vut.fit.pis.bakery.data;

import static javax.persistence.GenerationType.IDENTITY;

import java.time.LocalDate;

import javax.json.bind.annotation.JsonbDateFormat;
import javax.json.bind.annotation.JsonbTransient;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.Table;
import javax.persistence.Id;

/**
 * @author David Bednařík
 * Entity implementation class for Entity: Employee
 */
@Entity
@Table(name = "User")
public class User {
	@Id
	@GeneratedValue(strategy = IDENTITY)
	private int userId;
	private String name;
	private String surname;
    @JsonbDateFormat("yyyy-MM-dd")
    private LocalDate born;
    @Column(unique = true)
	private String email;
	// the password is ignored during serialization
	private String password;
	private String role;
	
	public User() 
	{
	}
   
	public boolean equals(Object other)
    {
        if (other instanceof User)
            return ((User) other).getUserId() == userId;
        else
            return false;
    }
	
	public int getUserId()
	{
	    return userId;
	}
	
	public void setUserId(int userId)
	{
	    this.userId = userId;
	}
	
	public String getName()
	{
	    return name;
	}
	
	public void setName(String name)
	{
	    this.name = name;
	}
	
	public String getSurname()
	{
	    return surname;
	}
	
	public void setSurname(String surname)
	{
	    this.surname = surname;
	}
	
	public LocalDate getBorn()
	{
	    return born;
	}
	
	public void setBorn(LocalDate born)
	{
	    this.born = born;
	}
	
	public String getEmail()
	{
	    return email;
	}
	
	public void setEmail(String email)
	{
	    this.email = email;
	}
	
	@JsonbTransient
	public String getPassword()
	{
	    return password;
	}
	
	public void setPassword(String password)
	{
	    this.password = password;
	}
	
	public String getRole()
	{
	    return role;
	}
	
	public void setRole(String role)
	{
	    this.role = role;
	}
}
