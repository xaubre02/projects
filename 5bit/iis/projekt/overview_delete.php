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
	<h1>Odstranit</h1>
	<form action="overview_delete.php" method="post">
	<? 
		if (isset($_POST['delete']))
		{
			include("connect_to_db.php");
			
			// Odstranit kouzlo
			if ($_POST['table'] == "Kouzlo")
			{
				$sqlQuery = 'SELECT * FROM Grim_obsah WHERE nazev_kouz=\''.$_POST['id'].'\'';
				$grim = mysql_query($sqlQuery, $db);
				
				$sqlQuery = 'SELECT * FROM Svitek WHERE nazev_kouz=\''.$_POST['id'].'\'';
				$scroll = mysql_query($sqlQuery, $db);
				
				// kontrola, zdali neni kouzlo obsazeno v grimoaru nebo ve svitku
				if (mysql_num_rows($grim) != 0 or mysql_num_rows($scroll) != 0)
					echo '	<p>Nelze odstranit kouzlo, protože je součástí některého grimoáru nebo svitku!</p>
							<a href='.'"'.$_COOKIE['last_page'].'"' . 'class="button">Rozumím</a>';
				
				else
				{
					// odstraneni vedlejsich elementu
					$sqlQuery = 'DELETE FROM Ma_ved_elem WHERE nazev_kouz=\''.$_POST['id'].'\'';
					$result = mysql_query($sqlQuery, $db);
					
					// odstraneni vazeb na kouzelnika
					$sqlQuery = 'DELETE FROM Kouz_umi WHERE nazev_kouz=\''.$_POST['id'].'\'';
					$result = mysql_query($sqlQuery, $db);
					
					// odstraneni kouzla samotneho
					$sqlQuery = 'DELETE FROM Kouzlo WHERE nazev_kouz=\''.$_POST['id'].'\'';
					$result = mysql_query($sqlQuery, $db);
					
					echo '	<p>Odstranení proběhlo úspěšně!</p>
							<a href='.'"'.$_COOKIE['last_page'].'"' . 'class="button">Rozumím</a>';
				}
			}
			
			// Odstranit svitek
			else if ($_POST['table'] == "Svitek")
			{
				// odstraneni svitku
				$sqlQuery = 'DELETE FROM Svitek WHERE nazev=\''.$_POST['id'].'\'';
				$result = mysql_query($sqlQuery, $db);
				
				echo '	<p>Odstranení proběhlo úspěšně!</p>
						<a href='.'"'.$_COOKIE['last_page'].'"' . 'class="button">Rozumím</a>';
			}
			
			// Odstranit dedikovane misto
			else if ($_POST['table'] == "Ded_misto")
			{
				$sqlQuery = 'SELECT * FROM Nab_ded_mista WHERE misto=\''.$_POST['id'].'\'';
				$recharge = mysql_query($sqlQuery, $db);
				
				// kontrola, zdali neni element soucasti grimoaru, kouzla nebo dedikovaneho mista
				if (mysql_num_rows($recharge) != 0)
					echo '	<p>Nelze odstranit dedikované místo, protože je součástí některého nabití!</p>
							<a href='.'"'.$_COOKIE['last_page'].'"' . 'class="button">Rozumím</a>';
				
				else
				{
					// odstraneni dedikovaneho mista
					$sqlQuery = 'DELETE FROM Ded_misto WHERE jmeno=\''.$_POST['id'].'\'';
					$result = mysql_query($sqlQuery, $db);
					
					echo '	<p>Odstranení proběhlo úspěšně!</p>
							<a href='.'"'.$_COOKIE['last_page'].'"' . 'class="button">Rozumím</a>';
				}
			}
			
			// Odstranit element
			else if ($_POST['table'] == "Element")
			{
				$sqlQuery = 'SELECT * FROM Grimoar WHERE druh=\''.$_POST['id'].'\'';
				$grim = mysql_query($sqlQuery, $db);
				
				$sqlQuery = 'SELECT * FROM Kouzlo WHERE druh=\''.$_POST['id'].'\'';
				$spell = mysql_query($sqlQuery, $db);
				
				$sqlQuery = 'SELECT * FROM Ded_misto WHERE druh=\''.$_POST['id'].'\'';
				$place = mysql_query($sqlQuery, $db);
				
				// kontrola, zdali neni element soucasti grimoaru, kouzla nebo dedikovaneho mista
				if (mysql_num_rows($grim) != 0 or mysql_num_rows($spell) != 0 or mysql_num_rows($place) != 0)
					echo '	<p>Nelze odstranit element, protože je součástí některého grimoáru, kouzla nebo dedikovaného místa!</p>
							<a href='.'"'.$_COOKIE['last_page'].'"' . 'class="button">Rozumím</a>';
				
				else
				{
					// odstraneni vedlejsich elementu
					$sqlQuery = 'DELETE FROM Ma_ved_elem WHERE druh=\''.$_POST['id'].'\'';
					$result = mysql_query($sqlQuery, $db);
					
					// odstraneni vazeb na kouzelnika
					$sqlQuery = 'UPDATE Kouzelnik SET synergie=NULL WHERE synergie=\''.$_POST['id'].'\'';
					$result = mysql_query($sqlQuery, $db);
					
					// odstraneni elementu samotneho
					$sqlQuery = 'DELETE FROM Element WHERE druh=\''.$_POST['id'].'\'';
					$result = mysql_query($sqlQuery, $db);
					
					echo '	<p>Odstranení proběhlo úspěšně!</p>
							<a href='.'"'.$_COOKIE['last_page'].'"' . 'class="button">Rozumím</a>';
				}
			}
			
			// Odstranit grimoar
			else if ($_POST['table'] == "Grimoar")
			{
				// odstraneni vazeb na obsahujici kouzla
				$sqlQuery = 'DELETE FROM Grim_obsah WHERE nazev_grim=\''.$_POST['id'].'\'';
				$result = mysql_query($sqlQuery, $db);
				
				// odstraneni historie vlastnictvi
				$sqlQuery = 'DELETE FROM Kouz_mel WHERE nazev_grim=\''.$_POST['id'].'\'';
				$result = mysql_query($sqlQuery, $db);
				
				// odstraneni historie nabiti
				$sqlQuery = 'SELECT * FROM Nabiti WHERE nazev_grim=\''.$_POST['id'].'\'';
				$recharge = mysql_query($sqlQuery, $db);
				if (mysql_num_rows($recharge) != 0)
				{
					while ($row = mysql_fetch_array($recharge))
					{
						$sqlQuery = 'DELETE FROM Nab_ded_mista WHERE Nabiti=\''.$row['id_nabiti'].'\'';
						$result = mysql_query($sqlQuery, $db);
					}
				}
				$sqlQuery = 'DELETE FROM Nabiti WHERE nazev_grim=\''.$_POST['id'].'\'';
				$result = mysql_query($sqlQuery, $db);
				
				// odstraneni grimoaru
				$sqlQuery = 'DELETE FROM Grimoar WHERE nazev_grim=\''.$_POST['id'].'\'';
				$result = mysql_query($sqlQuery, $db);
				
				echo '	<p>Odstranení proběhlo úspěšně!</p>
						<a href='.'"'.$_COOKIE['last_page'].'"' . 'class="button">Rozumím</a>';
			}

			// Odstranit kouzelnika
			else if ($_POST['table'] == "Kouzelnik")
			{
				$sqlQuery = 'SELECT * FROM Nabiti WHERE jmeno_kouz=\''.$_POST['id'].'\'';
				$recharge = mysql_query($sqlQuery, $db);
				
				// kontrola, zdali neni kouzelnik soucasti nektereho nabiti
				if (mysql_num_rows($recharge) != 0)
					echo '	<p>Nelze odstranit kouzelníka, protože je součástí některého nabití!</p>
							<a href='.'"'.$_COOKIE['last_page'].'"' . 'class="button">Rozumím</a>';
				
				else
				{
					// odstraneni vazeb na kouzla, ktera umi
					$sqlQuery = 'DELETE FROM Kouz_umi WHERE jmeno_kouz=\''.$_POST['id'].'\'';
					$result = mysql_query($sqlQuery, $db);
					
					// odstraneni historie vlastnictvi
					$sqlQuery = 'DELETE FROM Kouz_mel WHERE jmeno_kouz=\''.$_POST['id'].'\'';
					$result = mysql_query($sqlQuery, $db);
				
					// odstraneni vazeb na kouzelnika
					$sqlQuery = 'UPDATE Grimoar SET jmeno_kouz=NULL WHERE jmeno_kouz=\''.$_POST['id'].'\'';
					$result = mysql_query($sqlQuery, $db);
					$sqlQuery = 'UPDATE Svitek SET jmeno_kouz=NULL WHERE jmeno_kouz=\''.$_POST['id'].'\'';
					$result = mysql_query($sqlQuery, $db);
					
					// odstraneni kouzelnika
					$sqlQuery = 'DELETE FROM Kouzelnik WHERE jmeno_kouz=\''.$_POST['id'].'\'';
					$result = mysql_query($sqlQuery, $db);
					
					echo '	<p>Odstranení proběhlo úspěšně!</p>
							<a href='.'"'.$_COOKIE['last_page'].'"' . 'class="button">Rozumím</a>';
				}
			}
			
			mysql_close($db);
		}
		else
		{
			echo '	<p>Opravdu chcete odstranit ' . $_GET['ozn'].' <i>'.$_COOKIE['id'].'</i> &nbsp z databáze?</p>
					<input type="hidden" name="table" value="'. $_GET['table'] .'">
					<input type="hidden" name="id" value="'. $_COOKIE['id'] .'">
					<div class="button_choice">
						<a href='.'"'.$_COOKIE['last_page'].'"' . 'class="button">Ne</a>
						<input type="submit" value="Ano" class="button" name="delete">
					</div>';
		}
	?>
	</form>

</div>

<? include("footer.php"); ?>