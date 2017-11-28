<?
	include("check_login.php");
	include("header.php"); 
		
	if (isset($_GET['add']))
	{
		setcookie("last_page", basename($_SERVER['PHP_SELF']));
		header('Location: overview_add.php?table=Kouzelnik&jmeno_kouz='.$_GET['wizard']);
	}
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
		<a href="overview_recharge.php">Nabití</a>
	</nav>
</header>

<div class="content">
	<h1>Kouzelníci</h1>
	<div class="search">
		<form method="get">
			<input type="text" placeholder="Kouzelník" name="wizard">
			<input type="submit" name="search" value="Hledat" class="button">
			<?
				if ($_SESSION['login'] == "admin")
					echo '<input type="submit" name="add" value="Přidat" class="button" id="add">';
			?>
		</form>
	</div>
	
	<div class="table">
		<?php
			include("connect_to_db.php");
			$form = '<div class="table_head">
						<span>Jméno</span>
						<span style="width:80px">Mana</span>
						<span style="width:80px">Úroveň</span>
						<span style="width:100px">Synergie</span>
						<span style="width:200px">Kouzla</span>
						<span style="width:200px">Svitky</span>
						<span style="width:200px">Grimoáry</span>
					</div>';
			
			$sqlQuery = 'SELECT * FROM Kouzelnik';
			$result = mysql_query($sqlQuery, $db);
			$found_any = FALSE;
			
			while ($row = mysql_fetch_array($result))
			{
				if (isset($_GET['search']))
				{
					if ($_GET['wizard'] == $row['jmeno_kouz'] or $_GET['wizard'] == $row['mana'] or $_GET['wizard'] == $row['uroven'] or $_GET['wizard'] == "" or $_GET['wizard'] == $row['synergie'])
					{
						$found_any = TRUE;
						$form .= '<br><div class="table_content">
									<span>' .$row['jmeno_kouz']. '</span>
									<span style="width:80px">' .$row['mana']. '</span>
									<span style="width:80px">' .$row['uroven']. '</span>
									<span style="width:100px">';
									if ($row['synergie'] == "")
										$form .= 'žádná</span>';
									else
										$form .= $row['synergie'] . '</span>';
									
						// pridani kouzel
						$form .= '<span style="width:200px">';
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
						$form .= '<span style="width:200px">';
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
						$form .= '<span style="width:200px">';
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
				}
				else
				{
					$found_any = TRUE;
					$form .= '<br><div class="table_content">
									<span>' .$row['jmeno_kouz']. '</span>
									<span style="width:80px">' .$row['mana']. '</span>
									<span style="width:80px">' .$row['uroven']. '</span>';
									if ($row['synergie'] == "")
										$form .= '<span style="width:100px">žádná</span>';
									else
										$form .= '<a href="overview_elements.php?element='.$row['synergie'].'&search=Hledat"><span style="width:100px">'.$row['synergie'] . '</span></a>';
									
					// pridani kouzel
					$form .= '<span style="width:200px">';
					$sqlQuery = 'SELECT nazev_kouz FROM Kouz_umi WHERE jmeno_kouz=\''.$row['jmeno_kouz'].'\'';
					$result2 = mysql_query($sqlQuery, $db);
					if (mysql_num_rows($result2) == 0)
						$form .= 'žádná';
					
					else
					{
						while ($row2 = mysql_fetch_array($result2))
						{
							$form .= '<a href="overview_spells.php?spell='.$row2['nazev_kouz'].'&search=Hledat">'.$row2['nazev_kouz'].'</a><br>';
						}
					}
					$form .= '</span>';
					
					// pridani svitku
					$form .= '<span style="width:200px">';
					$sqlQuery = 'SELECT nazev FROM Svitek WHERE jmeno_kouz=\''.$row['jmeno_kouz'].'\'';
					$result2 = mysql_query($sqlQuery, $db);
					if (mysql_num_rows($result2) == 0)
						$form .= 'žádné';
					
					else
					{
						while ($row2 = mysql_fetch_array($result2))
						{
							$form .= '<a href="overview_scrolls.php?scroll='.$row2['nazev'].'&search=Hledat">'.$row2['nazev'].'</a><br>';
						}
					}
					$form .= '</span>';
					
					// pridani grimoaru
					$form .= '<span style="width:200px">';
					$sqlQuery = 'SELECT nazev_grim FROM Grimoar WHERE jmeno_kouz=\''.$row['jmeno_kouz'].'\'';
					$result2 = mysql_query($sqlQuery, $db);
					if (mysql_num_rows($result2) == 0)
						$form .= 'žádné';
					
					else
					{
						while ($row2 = mysql_fetch_array($result2))
						{
							$form .= '<a href="overview_spellbooks.php?spellbook='.$row2['nazev_grim'].'&search=Hledat">'.$row2['nazev_grim'].'</a><br>';
						}
					}
					$form .= '</span>';
			
					$form .='</div>';
					
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
			}

			if ($found_any)
				echo $form;
			
			else
				echo '<div class="table_head"><span class="table_msg">Nebyly nalezeny žádné výsledky</span></div>';
		?>
	</div>
</div>

<? include("footer.php"); ?>