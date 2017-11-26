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
		<a href="#" class="selected">Kouzelníci</a>
		<a href="overview_spells.php">Kouzla</a>
		<a href="overview_spellbooks.php">Grimoáry</a>
		<a href="overview_scrolls.php">Svitky</a>
		<a href="overview_elements.php">Elementy</a>
		<a href="overview_ded_places.php">Dedikovaná místa</a>
	</nav>
</header>

<div class="content">
	<h1>Kouzelníci</h1>
	<div class="table">
		<div class="table_head">
			<span>Jméno</span>
			<span id="column_mana">Mana</span>
			<span id="column_level">Úroveň</span>
			<span>Synergie</span>
			<span>Kouzla</span>
			<span>Svitky</span>
			<span>Grimoáry</span>
			<span>Nabití</span>
		</div>
		
		<?php
			include("connect_to_db.php");
			$form = '';
			
			$sqlQuery = 'SELECT * FROM Kouzelnik';
			$result = mysql_query($sqlQuery, $db);

			while ($row = mysql_fetch_array($result))
			{
				$form .= '<br><div class="table_content">
								<span>' .$row['jmeno_kouz']. '</span>
								<span id="column_mana">' .$row['mana']. '</span>
								<span id="column_level">' .$row['uroven']. '</span>
								<span>žádná</span>
							</div>';
			}

			echo $form;
		?>
	</div>
</div>

<? include("footer.php"); ?>