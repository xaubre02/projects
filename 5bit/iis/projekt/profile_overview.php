<?
	include("check_login.php");
	include("header.php"); 
?>

<header>
	<nav class="main-menu">
		<a href="home.php">Home</a>
		<a href="overview_wizards.php">Přehled</a>
		<a href="#" class="selected"><? echo $_SESSION['login']?></a>
		<form method="get">
			<input type="submit" name="logout" value="Odhlásit se">
		</form>
	</nav>
	<nav class="sub-menu">
		<a href="#" class="selected">Přehled</a>
		<a href="profile_edit.php">Upravit profil</a>
		<a href="profile_chpwd.php">Změnit heslo</a>
	</nav>
</header>

<div class="content">
	<h1>Váš profil</h1>
	<div class="profile_content">
	<?php
		$form = '';
		// pripojeni k databazi a ziskani informaci o aktualnim uzivateli
		include("connect_to_db.php");
		$sqlQuery = 'SELECT * FROM Kouzelnik WHERE jmeno_kouz=\'' . $_SESSION['login'] . '\'';
		$wiz = mysql_query($sqlQuery, $db);
		$wiz = mysql_fetch_array($wiz);
		
		$sqlQuery = 'SELECT nazev_kouz FROM Kouz_umi WHERE jmeno_kouz=\'' . $_SESSION['login'] . '\'';
		$kouz = mysql_query($sqlQuery, $db);
		
		if ($_SESSION['login'] == "admin")
		{	
			echo '<div class="profile_part">
					<span>Typ účtu:</span>
					<div class="profile_part_wrapper">
						<p class="profile_db">Administrátor</p>
					</div>
				</div>
				<div class="profile_part">
					<span>Jméno:</span>
					<div class="profile_part_wrapper">
						<p class="profile_db">admin</p>
					</div>
				</div>';
			return;
		}
		
		echo '
		<div class="profile_part">
			<span>Typ účtu:</span>
			<div class="profile_part_wrapper">
				<p class="profile_db">Uživatel</p>
			</div>
		</div>
		<div class="profile_part">
			<span>Jméno:</span>
			<div class="profile_part_wrapper">
				<p class="profile_db">'.$wiz['jmeno_kouz'].'</p>
			</div>
		</div>
		<div class="profile_part">
			<span>Mana:</span>
			<div class="profile_part_wrapper">
				<p class="profile_db">'.$wiz['mana'].'</p>
			</div>
		</div>
		<div class="profile_part">
			<span>Úroveň:</span>
			<div class="profile_part_wrapper">
				<p class="profile_db">'.$wiz['uroven'].'</p>
			</div>
		</div>
		<div class="profile_part">
			<span>Synergie:</span>
			<div class="profile_part_wrapper">';
			if ($wiz['synergie'] == NULL)
			{
				echo '<p class="profile_db">Žádná</p>';
			}
			else
			{
				echo '<p class="profile_db">'. $wiz['synergie'] .'</p>';
			}
		echo '</div></div>
		<div class="profile_part">
			<span>Kouzla:</span>
			<div class="profile_part_wrapper">';
			if (mysql_num_rows($kouz) == 0)
			{
				echo '<p class="profile_db">Žádná</p>';
			}
			else
			{
				while ($row = mysql_fetch_array($kouz))
				{
					echo '<p class="profile_db">'. $row[0] .'</p>';
				}
			}
		echo'</div></div>';
		mysql_close($db);
	?>
	</div>
</div>

<? include("footer.php"); ?>