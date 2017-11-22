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
		<a href="#" class="selected">Dedikovaná místa</a>
	</nav>
</header>

<div class="content">
	<h1>Dedikovaná místa</h1>
	<?php
		include("connect_to_db.php");
		$form = '';
		
		$sqlQuery = 'SELECT * FROM Ded_misto';
		$result = mysql_query($sqlQuery, $db);

		while ($row = mysql_fetch_array($result))
		{
			$form .= $row[0] . '<br>';
		}

		echo $form;
	?>
</div>

<? include("footer.php"); ?>