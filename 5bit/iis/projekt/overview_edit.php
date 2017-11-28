<?
	include("check_login.php");
	include("header.php"); 
?>

<header>
	<nav class="main-menu">
		<a href="home.php">Home</a>
		<a href="#" class="selected">Přehled</a>
		<a href="profile_overview"><? echo $_SESSION['login']?></a>
		<form method="get">
			<input type="submit" name="logout" value="Odhlásit se">
		</form>
	</nav>
	<nav class="sub-menu">
		<a href="overview_wizards.php">Kouzelníci</a>
		<a href="overview_spells.php">Kouzla</a>
		<a href="overview_spellbooks.php">Grimoáry</a>
		<a href="overview_scrolls.php">Svitky</a>
		<a href="overview_elements.php">Elementy</a>
		<a href="overview_ded_places.php">Dedikovaná místa</a>
		<a href="overview_recharge.php" class="selected">Nabití</a>
	</nav>
</header>

<div class="content">
	<?php
	if ($_SESSION['login'] != "admin")
		echo '<h1>Přístup zamítnut!<br>Pro tuto akci nemáte dostatečé oprávnění.</h1>';

	else
	{
		// pripojeni k databazi a ziskani informaci o aktualni polozce
		include("connect_to_db.php");
		
		if (isset($_GET['table']))
		{
			if ($_GET['table'] == "Element")
			{
				echo '	<h1>Upravit element</h1>
						<form action="overview_edit.php" method="post">';
				include("edit/element.php");
			}
			else if ($_GET['table'] == "Ded_misto")
			{
				echo '	<h1>Upravit dedikované místo</h1>
						<form action="overview_edit.php" method="post">';
				include("edit/ded_misto.php");
			}
			else if ($_GET['table'] == "Svitek")
			{
				echo '	<h1>Upravit svitek</h1>
						<form action="overview_edit.php" method="post">';
				include("edit/svitek.php");
			}
			else if ($_GET['table'] == "Kouzlo")
			{
				echo '	<h1>Upravit kouzlo</h1>
						<form action="overview_edit.php" method="post">';
				include("edit/kouzlo.php");
			}
			else if ($_GET['table'] == "Kouzelnik")
			{
				echo '	<h1>Upravit kouzelníka</h1>
						<form action="overview_edit.php" method="post">';
				include("edit/kouzelnik.php");
			}
			else if ($_GET['table'] == "Grimoar")
			{
				echo '	<h1>Upravit grimoár</h1>
						<form action="overview_edit.php" method="post">';
				include("edit/grimoar.php");
			}
			else if ($_GET['table'] == "Kouz_mel")
			{
				echo '	<h1>Upravit záznam o historii vlastnictví</h1>
						<form action="overview_edit.php" method="post">';
				include("edit/historie.php");
			}
			else if ($_GET['table'] == "Nabiti")
			{
				echo '	<h1>Upravit nabití</h1>
						<form action="overview_edit.php" method="post">';
				include("edit/nabiti.php");
			}
		}
		else
		{
			if ($_POST['table'] == "Element")
			{
				echo '	<h1>Upravit element</h1>
						<form action="overview_edit.php" method="post">';
				include("edit/element.php");
			}
			else if ($_POST['table'] == "Ded_misto")
			{
				echo '	<h1>Upravit dedikované místo</h1>
						<form action="overview_edit.php" method="post">';
				include("edit/ded_misto.php");
			}
			else if ($_POST['table'] == "Svitek")
			{
				echo '	<h1>Upravit svitek</h1>
						<form action="overview_edit.php" method="post">';
				include("edit/svitek.php");
			}
			else if ($_POST['table'] == "Kouzlo")
			{
				echo '	<h1>Upravit kouzlo</h1>
						<form action="overview_edit.php" method="post">';
				include("edit/kouzlo.php");
			}
			else if ($_POST['table'] == "Kouzelnik")
			{
				echo '	<h1>Upravit kouzelníka</h1>
						<form action="overview_edit.php" method="post">';
				include("edit/kouzelnik.php");
			}
			else if ($_POST['table'] == "Grimoar")
			{
				echo '	<h1>Upravit grimoár</h1>
						<form action="overview_edit.php" method="post">';
				include("edit/grimoar.php");
			}
			else if ($_POST['table'] == "Kouz_mel")
			{
				echo '	<h1>Upravit záznam o historii vlastnictví</h1>
						<form action="overview_edit.php" method="post">';
				include("edit/historie.php");
			}
			else if ($_POST['table'] == "Nabiti")
			{
				echo '	<h1>Upravit nabití</h1>
						<form action="overview_edit.php" method="post">';
				include("edit/nabiti.php");
			}
		}
		
		echo '</form>';
		mysql_close($db);
	}
	?>
</div>

<? include("footer.php"); ?>