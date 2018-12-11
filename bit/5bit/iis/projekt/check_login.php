<?php
	session_start();
	ini_set("default_charset", "utf-8");

	// pokud neni uzivatel prihlasen
	if (!isset($_SESSION["login"]))
	{
		SetCookie('disp_form', 3);
		header('Location: index.php');
	}
	else
	{
		// pokud vyprsela casomira pro session
		if (time() - $_SESSION["time"] > 600)
		{
			include("logout.php");
			SetCookie('disp_form', 4);
			header('Location: index.php');
		}
		// reset casomiry
		else 
		{
			$_SESSION["time"] = time();
		}
	}

	// Uzivatel se chce odhlasit
	if(isset($_GET['logout']))
	{
		include("logout.php");
		SetCookie('disp_form', 5);
		header('Location: index.php');
	}
?>