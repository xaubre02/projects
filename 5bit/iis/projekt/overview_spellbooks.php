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
		<a href="overview_spellbooks.php" class="selected">Grimoáry</a>
		<a href="overview_scrolls.php">Svitky</a>
		<a href="overview_elements.php">Elementy</a>
		<a href="overview_ded_places.php">Dedikovaná místa</a>
	</nav>
</header>

<div class="content">
	<h1>Grimoáry</h1>
	<div class="search">
		<form method="get">
			<input type="text" placeholder="Grimoár" name="spellbook" required>
			<input type="submit" name="search" value="Hledat" class="button">
		</form>
	</div>
	
	<div class="table">
		<?php
			include("connect_to_db.php");
			$form = '<div class="table_head">
						<span id="column_scroll">Název</span>
						<span>Mana</span>
						<span>Element</span>
						<span>Majitel</span>
						<span id="column_sec_element">Kouzla</span>
						<span>Vlastnictví</span>
					</div>';
			
			$sqlQuery = 'SELECT * FROM Grimoar';
			$result = mysql_query($sqlQuery, $db);
			$found_any = FALSE;
			
			while ($row = mysql_fetch_array($result))
			{
				if (isset($_GET['search']))
				{
					if ($_GET['spellbook'] == $row['nazev_grim'] or $_GET['spellbook'] == $row['mana'] or $_GET['spellbook'] == $row['druh'] or $_GET['spellbook'] == $row['jmeno_kouz'])
					{
						$found_any = TRUE;
						$form .= '<br><div class="table_content">
										<span id="column_scroll">' .$row['nazev_grim']. '</span>
										<span>' .$row['mana']. '</span>
										<span>' .$row['druh']. '</span>
										<span>';
										if ($row['jmeno_kouz'] == "")
											$form .= 'není</span>';
										else
											$form .= $row['jmeno_kouz'].'</span>';
										
										$form .= '<span id="column_sec_element">';
										$sqlQuery = 'SELECT nazev_kouz FROM Grim_obsah WHERE nazev_grim=\''.$row['nazev_grim'].'\'';
										$result2 = mysql_query($sqlQuery, $db);
										while ($row2 = mysql_fetch_array($result2))
											$form .= $row2['nazev_kouz'].'<br>';
						
						$form .= '	</span>
							<span><a href="overview_history.php?spellbook=' .$row['nazev_grim']. '">historie</a></span>
						</div>';
					}
				}
				else 
				{
					$found_any = TRUE;
					$form .= '<br><div class="table_content">
									<span id="column_scroll">' .$row['nazev_grim']. '</span>
									<span>' .$row['mana']. '</span>
									<span>' .$row['druh']. '</span>
									<span>';
									if ($row['jmeno_kouz'] == "")
										$form .= 'není</span>';
									else
										$form .= $row['jmeno_kouz'].'</span>';
									
									$form .= '<span id="column_sec_element">';
									$sqlQuery = 'SELECT nazev_kouz FROM Grim_obsah WHERE nazev_grim=\''.$row['nazev_grim'].'\'';
									$result2 = mysql_query($sqlQuery, $db);
									while ($row2 = mysql_fetch_array($result2))
										$form .= $row2['nazev_kouz'].'<br>';
					
					$form .= '	</span>
						<span><a href="overview_history.php?spellbook=' .$row['nazev_grim']. '">historie</a></span>
					</div>';
				}

				// dalsi prava pro admina - editace a odstraneni
				if ($_SESSION['login'] == "admin")
				{
					$form .= '	<a href="overview_edit.php?table=Grimoar" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['nazev_grim'] .'\')">
									<img src="https://www.stud.fit.vutbr.cz/~xaubre02/pic/edit.png" Title="Upravit">
								</a>
								<a href="overview_delete.php?ozn=grimoár&table=Grimoar&item=nazev_grim" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['nazev_grim'] .'\')">
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