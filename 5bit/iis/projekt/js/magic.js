function display_form(id)
{
	// ziskani formularu na vychozi strance - index.php
	var form1 = document.getElementById("login");      // prihlasovaci formular
	var form2 = document.getElementById("register");   // registracni formular
	var form3 = document.getElementById("logout_msg"); // informacni formular

	if (id == 0)
	{
		document.cookie = "disp_form=1";
		form1.className += " hide";
		form2.className = form2.className.replace("hide", "");
	}
	else if (id == 1)
	{
		document.cookie = "disp_form=0";
		form2.className += " hide";
		form1.className = form1.className.replace("hide", "");
	}
	
	else if (id == 2)
	{
		document.cookie = "disp_form=0";
		form2.className += " hide";
		form1.className = form1.className.replace("hide", "");
	}
	
	else if (id == 3)
	{
		document.cookie = "disp_form=0";
		form3.className += " hide";
		form1.className = form1.className.replace("hide", "");
	}
}

// nastaveni cookies
function set_cookie(name, val)
{
	document.cookie = name + "=" + encodeURI(val); 
}

// formatovani data
$( 
	function() 
	{
		$( "#datum_od" ).datepicker({ dateFormat: "dd.mm. yy", showAnim: "slideDown", maxDate: new Date() });
		$( "#datum_do" ).datepicker({ dateFormat: "dd.mm. yy", showAnim: "slideDown", maxDate: new Date() });
	} 
);

  