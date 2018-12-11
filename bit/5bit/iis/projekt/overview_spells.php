<?
	include("check_login.php");
	include("header.php"); 
		
	if (isset($_GET['add']))
	{
		setcookie("last_page", basename($_SERVER['PHP_SELF']));
		header('Location: overview_add.php?table=Kouzlo&nazev_kouz='.$_GET['spell']);
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
		<a href="overview_wizards.php">Kouzelníci</a>
		<a href="overview_spells.php" class="selected">Kouzla</a>
		<a href="overview_spellbooks.php">Grimoáry</a>
		<a href="overview_scrolls.php">Svitky</a>
		<a href="overview_elements.php">Elementy</a>
		<a href="overview_ded_places.php">Dedikovaná místa</a>
		<a href="overview_recharge.php">Nabití</a>
	</nav>
</header>

<div class="content">
	<h1>Kouzla</h1>
	<div class="search">
		<form method="get">
			<input type="text" placeholder="Kouzlo" name="spell" value="<? if(isset($_GET['search'])) echo $_GET['spell'] ?>">
			<input type="submit" name="search" value="Hledat" class="button">
			<?
				if ($_SESSION['login'] == "admin")
					echo '<input type="submit" name="add" value="Přidat" class="button" id="add">';
			?>
		</form>
	</div>
	
	<div class="table"  style="min-width:1070px">
		<?php
			include("connect_to_db.php");
			$form = '<div class="table_head">
						<span style="width:240px">Název</span>
						<span>Obtížnost</span>
						<span>Síla</span>
						<span>Typ</span>
						<span style="font-size: 19px; padding-top: 2px">Hlavní element</span>
						<span style="font-size: 19px; padding-top: 2px">Vedlejší elementy</span>
					</div>';
			
			$sqlQuery = 'SELECT * FROM Kouzlo';
			$result = mysql_query($sqlQuery, $db);
			$found_any = FALSE;
			
			while ($row = mysql_fetch_array($result))
			{
				if (isset($_GET['search']))
				{
					if ($_GET['spell'] == $row['nazev_kouz'] or $_GET['spell'] == $row['obtiznost'] or $_GET['spell'] == $row['sila'] or $_GET['spell'] == $row['typ'] or $_GET['spell'] == $row['druh'] or $_GET['spell'] == "")
					{
						$found_any = TRUE;
						$form .= '<br><div class="table_content">
										<span style="width:240px">' .$row['nazev_kouz']. '</span>
										<span>' .$row['obtiznost']. '</span>
										<span>' .$row['sila']. '</span>
										<span>' .$row['typ']. '</span>
										<a href="overview_elements.php?element='.$row['druh'].'&search=Hledat"><span>' .$row['druh']. '</span></a>
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
													$form .= '<a href="overview_elements.php?element='.$row2['druh'].'&search=Hledat">'.$row2['druh'].'</a><br>';
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
				else
				{
					$found_any = TRUE;
					$form .= '<br><div class="table_content">
									<span style="width:240px">' .$row['nazev_kouz']. '</span>
									<span>' .$row['obtiznost']. '</span>
									<span>' .$row['sila']. '</span>
									<span>' .$row['typ']. '</span>
									<a href="overview_elements.php?element='.$row['druh'].'&search=Hledat"><span>' .$row['druh']. '</span></a>
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
												$form .= '<a href="overview_elements.php?element='.$row2['druh'].'&search=Hledat">'.$row2['druh'].'</a><br>';
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