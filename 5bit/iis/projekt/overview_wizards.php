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
		<a href="overview_wizards.php" class="selected">Kouzelníci</a>
		<a href="overview_spells.php">Kouzla</a>
		<a href="overview_spellbooks.php">Grimoáry</a>
		<a href="overview_scrolls.php">Svitky</a>
		<a href="overview_elements.php">Elementy</a>
		<a href="overview_ded_places.php">Dedikovaná místa</a>
	</nav>
</header>

<div class="content">
	<h1>Kouzelníci</h1>
	<div class="search">
		<form method="get">
			<input type="text" placeholder="Kouzelník" name="wizard" required>
			<input type="submit" name="search" value="Hledat" class="button">
		</form>
	</div>
	
	<div class="table">
		<?php
			include("connect_to_db.php");
			$form = '<div class="table_head">
						<span>Jméno</span>
						<span id="column_mana">Mana</span>
						<span id="column_level">Úroveň</span>
						<span>Synergie</span>
						<span>Kouzla</span>
						<span>Svitky</span>
						<span>Grimoáry</span>
					</div>';
			
			$sqlQuery = 'SELECT * FROM Kouzelnik';
			$result = mysql_query($sqlQuery, $db);
			$found_any = FALSE;
			
			while ($row = mysql_fetch_array($result))
			{
				if (isset($_GET['search']))
				{
					if ($_GET['wizard'] == $row['jmeno_kouz'] or $_GET['wizard'] == $row['mana'] or $_GET['wizard'] == $row['uroven'])
					{
						$found_any = TRUE;
						$form .= '<br><div class="table_content">
									<span>' .$row['jmeno_kouz']. '</span>
									<span id="column_mana">' .$row['mana']. '</span>
									<span id="column_level">' .$row['uroven']. '</span>
									<span>';
									if ($row['synergie'] == "")
										$form .= 'žádná</span>';
									else
										$form .= $row['synergie'] . '</span>';
									
						// pridani kouzel
						$form .= '<span>';
						$sqlQuery = 'SELECT nazev_kouz FROM Kouz_umi WHERE jmeno_kouz=\''.$row['jmeno_kouz'].'\'';
						$result2 = mysql_query($sqlQuery, $db);
						if (mysql_num_rows($result2) == 0)
							$form .= 'žádná';
						
						else
						{
							while ($row2 = mysql_fetch_array($result2))
							{
								$form .= $row2['nazev_kouz'].'<br>';
							}
						}
						$form .= '</span>';
						
						// pridani svitku
						$form .= '<span>';
						$sqlQuery = 'SELECT nazev FROM Svitek WHERE jmeno_kouz=\''.$row['jmeno_kouz'].'\'';
						$result2 = mysql_query($sqlQuery, $db);
						if (mysql_num_rows($result2) == 0)
							$form .= 'žádné';
						
						else
						{
							while ($row2 = mysql_fetch_array($result2))
							{
								$form .= $row2['nazev'].'<br>';
							}
						}
						$form .= '</span>';
						
						// pridani grimoaru
						$form .= '<span>';
						$sqlQuery = 'SELECT nazev_grim FROM Grimoar WHERE jmeno_kouz=\''.$row['jmeno_kouz'].'\'';
						$result2 = mysql_query($sqlQuery, $db);
						if (mysql_num_rows($result2) == 0)
							$form .= 'žádné';
						
						else
						{
							while ($row2 = mysql_fetch_array($result2))
							{
								$form .= $row2['nazev_grim'].'<br>';
							}
						}
						$form .= '</span>';
				
						$form .='</div>';
						}
				}
				else
				{
					$found_any = TRUE;
					$form .= '<br><div class="table_content">
									<span>' .$row['jmeno_kouz']. '</span>
									<span id="column_mana">' .$row['mana']. '</span>
									<span id="column_level">' .$row['uroven']. '</span>
									<span>';
									if ($row['synergie'] == "")
										$form .= 'žádná</span>';
									else
										$form .= $row['synergie'] . '</span>';
									
					// pridani kouzel
					$form .= '<span>';
					$sqlQuery = 'SELECT nazev_kouz FROM Kouz_umi WHERE jmeno_kouz=\''.$row['jmeno_kouz'].'\'';
					$result2 = mysql_query($sqlQuery, $db);
					if (mysql_num_rows($result2) == 0)
						$form .= 'žádná';
					
					else
					{
						while ($row2 = mysql_fetch_array($result2))
						{
							$form .= $row2['nazev_kouz'].'<br>';
						}
					}
					$form .= '</span>';
					
					// pridani svitku
					$form .= '<span>';
					$sqlQuery = 'SELECT nazev FROM Svitek WHERE jmeno_kouz=\''.$row['jmeno_kouz'].'\'';
					$result2 = mysql_query($sqlQuery, $db);
					if (mysql_num_rows($result2) == 0)
						$form .= 'žádné';
					
					else
					{
						while ($row2 = mysql_fetch_array($result2))
						{
							$form .= $row2['nazev'].'<br>';
						}
					}
					$form .= '</span>';
					
					// pridani grimoaru
					$form .= '<span>';
					$sqlQuery = 'SELECT nazev_grim FROM Grimoar WHERE jmeno_kouz=\''.$row['jmeno_kouz'].'\'';
					$result2 = mysql_query($sqlQuery, $db);
					if (mysql_num_rows($result2) == 0)
						$form .= 'žádné';
					
					else
					{
						while ($row2 = mysql_fetch_array($result2))
						{
							$form .= $row2['nazev_grim'].'<br>';
						}
					}
					$form .= '</span>';
			
					$form .='</div>';
				}
				
				// dalsi prava pro admina - editace a odstraneni
				if ($_SESSION['login'] == "admin")
				{
					$form .= '	<a href="overview_edit.php?table=Kouzelnik" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['jmeno_kouz'] .'\')">
									<img src="https://www.stud.fit.vutbr.cz/~xaubre02/pic/edit.png" Title="Upravit">
								</a>
								<a href="overview_delete.php?ozn=kouzelníka&table=Kouzelnik&item=jmeno_kouz" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['jmeno_kouz'] .'\')">
									<img src="https://www.stud.fit.vutbr.cz/~xaubre02/pic/delete.png" Title="Odstranit">
								</a>';
					setcookie("last_page", basename($_SERVER['PHP_SELF']));
				}
			}

			if ($found_any)
				echo $form;
			
			else
				echo '<div class="table_head"><span class="table_msg">Nebyly nalezeny žádné výsledky</span></div>';
		?>
	</div>
</div>

<? include("footer.php"); ?>