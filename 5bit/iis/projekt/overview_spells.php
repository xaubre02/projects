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
		<a href="overview_spells.php" class="selected">Kouzla</a>
		<a href="overview_spellbooks.php">Grimoáry</a>
		<a href="overview_scrolls.php">Svitky</a>
		<a href="overview_elements.php">Elementy</a>
		<a href="overview_ded_places.php">Dedikovaná místa</a>
	</nav>
</header>

<div class="content">
	<h1>Kouzla</h1>
	<div class="search">
		<form method="get">
			<input type="text" placeholder="Kouzlo" name="spell" required>
			<input type="submit" name="search" value="Hledat" class="button">
		</form>
	</div>
	
	<div class="table">
		<?php
			include("connect_to_db.php");
			$form = '<div class="table_head">
						<span>Název</span>
						<span>Obtížnost</span>
						<span>Síla</span>
						<span>Typ</span>
						<span>Hlavní element</span>
						<span id="column_sec_element">Vedlejší elementy</span>
					</div>';
			
			$sqlQuery = 'SELECT * FROM Kouzlo';
			$result = mysql_query($sqlQuery, $db);
			$found_any = FALSE;
			
			while ($row = mysql_fetch_array($result))
			{
				if (isset($_GET['search']))
				{
					if ($_GET['spell'] == $row['nazev_kouz'] or $_GET['spell'] == $row['obtiznost'] or $_GET['spell'] == $row['sila'] or $_GET['spell'] == $row['typ'] or $_GET['spell'] == $row['druh'])
					{
						$found_any = TRUE;
						$form .= '<br><div class="table_content">
										<span>' .$row['nazev_kouz']. '</span>
										<span>' .$row['obtiznost']. '</span>
										<span>' .$row['sila']. '</span>
										<span>' .$row['typ']. '</span>
										<span>' .$row['druh']. '</span>
										<span id="column_sec_element">';
										
										if ($row['typ'] == "útočné")
											$form .= 'žádné';
										
										else
										{
											$sqlQuery = 'SELECT druh FROM Ma_ved_elem WHERE nazev_kouz=\''.$row['nazev_kouz'].'\'';
											$result2 = mysql_query($sqlQuery, $db);
											if (mysql_num_rows($result2) == 0)
												$form .= 'žádné';
											
											else
											{
												while ($row2 = mysql_fetch_array($result2))
												{
													$form .= $row2['druh'].'<br>';
												}
											}
										}
						
						$form .= '</span></div>';
					}
				}
				else
				{
					$found_any = TRUE;
					$form .= '<br><div class="table_content">
									<span>' .$row['nazev_kouz']. '</span>
									<span>' .$row['obtiznost']. '</span>
									<span>' .$row['sila']. '</span>
									<span>' .$row['typ']. '</span>
									<span>' .$row['druh']. '</span>
									<span id="column_sec_element">';
									
									if ($row['typ'] == "útočné")
										$form .= 'žádné';
									
									else
									{
										$sqlQuery = 'SELECT druh FROM Ma_ved_elem WHERE nazev_kouz=\''.$row['nazev_kouz'].'\'';
										$result2 = mysql_query($sqlQuery, $db);
										if (mysql_num_rows($result2) == 0)
											$form .= 'žádné';
										
										else
										{
											while ($row2 = mysql_fetch_array($result2))
											{
												$form .= $row2['druh'].'<br>';
											}
										}
									}
					
					$form .= '</span></div>';
					
					// dalsi prava pro admina - editace a odstraneni
					if ($_SESSION['login'] == "admin")
					{
						$form .= '	<a href="overview_edit.php?table=Kouzlo" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['nazev_kouz'] .'\')">
										<img src="https://www.stud.fit.vutbr.cz/~xaubre02/pic/edit.png" Title="Upravit">
									</a>
									<a href="overview_delete.php?ozn=kouzlo&table=Kouzlo" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['nazev_kouz'] .'\')">
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
			
			mysql_close($db);
		?>
	</div>
</div>

<? include("footer.php"); ?>