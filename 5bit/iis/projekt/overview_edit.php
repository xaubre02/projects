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
	</nav>
</header>

<div class="content">
	<?
		if ($_SESSION['login'] != "admin")
		{
			echo '<h1>Přístup zamítnut!<br>Pro tuto akci nemáte dostatečé oprávnění.</h1>';
			return;
		}
	?>
	<h1>Úpravy</h1>
	<form action="overview_edit.php" method="post">
	<?php
		// pripojeni k databazi a ziskani informaci o aktualni polozce
		include("connect_to_db.php");
		
		if (isset($_GET['table']))
		{
			if ($_GET['table'] == "Element")
			{
				include("edit/element.php");
			}
			else if ($_GET['table'] == "Ded_misto")
			{
				include("edit/ded_misto.php");
			}
			else if ($_GET['table'] == "Svitek")
			{
				include("edit/svitek.php");
			}
			else if ($_GET['table'] == "Kouzlo")
			{
				include("edit/kouzlo.php");
			}
			else if ($_GET['table'] == "Kouzelnik")
			{
				include("edit/kouzelnik.php");
			}
			else if ($_GET['table'] == "Grimoar")
			{
				include("edit/grimoar.php");
			}
		}
		else
		{
			if ($_POST['table'] == "Element")
			{
				include("edit/element.php");
			}
			else if ($_POST['table'] == "Ded_misto")
			{
				include("edit/ded_misto.php");
			}
			else if ($_POST['table'] == "Svitek")
			{
				include("edit/svitek.php");
			}
			else if ($_POST['table'] == "Kouzlo")
			{
				include("edit/kouzlo.php");
			}
			else if ($_POST['table'] == "Kouzelnik")
			{
				include("edit/kouzelnik.php");
			}
			else if ($_POST['table'] == "Grimoar")
			{
				include("edit/grimoar.php");
			}
		}
		
		mysql_close($db);
	?>
	</form>

</div>

<? include("footer.php"); ?>