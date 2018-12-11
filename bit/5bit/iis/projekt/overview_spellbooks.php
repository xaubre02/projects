<?
	include("check_login.php");
	include("header.php"); 
		
	if (isset($_GET['add']))
	{
		setcookie("last_page", basename($_SERVER['PHP_SELF']));
		header('Location: overview_add.php?table=Grimoar&nazev_grim='.$_GET['spellbook']);
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
		<a href="overview_spells.php">Kouzla</a>
		<a href="overview_spellbooks.php" class="selected">Grimoáry</a>
		<a href="overview_scrolls.php">Svitky</a>
		<a href="overview_elements.php">Elementy</a>
		<a href="overview_ded_places.php">Dedikovaná místa</a>
		<a href="overview_recharge.php">Nabití</a>
	</nav>
</header>

<div class="content">
	<h1>Grimoáry</h1>
	<div class="search">
		<form method="get">
			<input type="text" placeholder="Grimoár" name="spellbook" value="<? if(isset($_GET['search'])) echo $_GET['spellbook'] ?>">
			<input type="submit" name="search" value="Hledat" class="button">
			<?
				if ($_SESSION['login'] == "admin")
					echo '<input type="submit" name="add" value="Přidat" class="button" id="add">';
			?>
		</form>
	</div>
	
	<div class="table"  style="min-width:1055px">
		<?php
			include("connect_to_db.php");
			$form = '<div class="table_head">
						<span style="width:240px">Název</span>
						<span style="width:85px">Mana</span>
						<span style="width:120px">Element</span>
						<span style="width:200px">Majitel</span>
						<span style="width:240px">Kouzla</span>
						<span style="width:90px">Vlastnil</span>
					</div>';
			
			$sqlQuery = 'SELECT * FROM Grimoar';
			$result = mysql_query($sqlQuery, $db);
			$found_any = FALSE;
			
			while ($row = mysql_fetch_array($result))
			{
				if (isset($_GET['search']))
				{
					if ($_GET['spellbook'] == $row['nazev_grim'] or $_GET['spellbook'] == $row['mana'] or $_GET['spellbook'] == $row['druh'] or $_GET['spellbook'] == $row['jmeno_kouz'] or $_GET['spellbook'] == "")
					{
						$found_any = TRUE;
						$form .= '<br><div class="table_content">
										<span style="width:240px">' .$row['nazev_grim']. '</span>
										<span style="width:85px">' .$row['mana']. '</span>
										<a href="overview_elements.php?element='.$row['druh'].'&search=Hledat"><span style="width:120px">' .$row['druh']. '</span></a>';
										if ($row['jmeno_kouz'] == "")
											$form .= '<span style="width:200px">není</span>';
										else
											$form .= '<a href="overview_wizards.php?wizard='.$row['jmeno_kouz'].'&search=Hledat"><span style="width:200px">'.$row['jmeno_kouz'].'</span></a>';
										
										$form .= '<span  style="width:240px">';
										$sqlQuery = 'SELECT nazev_kouz FROM Grim_obsah WHERE nazev_grim=\''.$row['nazev_grim'].'\'';
										$result2 = mysql_query($sqlQuery, $db);
										while ($row2 = mysql_fetch_array($result2))
											$form .= '<a href="overview_spells.php?spell='.$row2['nazev_kouz'].'&search=Hledat">'.$row2['nazev_kouz'].'</a><br>';
						
						$form .= '	</span>
							<span  style="width:90px"><a href="overview_history.php?spellbook=' .$row['nazev_grim']. '">historie</a></span>
						</div>';

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
				}
				else 
				{
					$found_any = TRUE;
					$form .= '<br><div class="table_content">
									<span style="width:240px">' .$row['nazev_grim']. '</span>
									<span style="width:85px">' .$row['mana']. '</span>
									<a href="overview_elements.php?element='.$row['druh'].'&search=Hledat"><span style="width:120px">' .$row['druh']. '</span></a>';
									if ($row['jmeno_kouz'] == "")
										$form .= '<span style="width:200px">není</span>';
									else
										$form .= '<a href="overview_wizards.php?wizard='.$row['jmeno_kouz'].'&search=Hledat"><span style="width:200px">'.$row['jmeno_kouz'].'</span></a>';
									
									$form .= '<span  style="width:240px">';
									$sqlQuery = 'SELECT nazev_kouz FROM Grim_obsah WHERE nazev_grim=\''.$row['nazev_grim'].'\'';
									$result2 = mysql_query($sqlQuery, $db);
									while ($row2 = mysql_fetch_array($result2))
										$form .= '<a href="overview_spells.php?spell='.$row2['nazev_kouz'].'&search=Hledat">'.$row2['nazev_kouz'].'</a><br>';
					
					$form .= '	</span>
						<span  style="width:90px"><a href="overview_history.php?spellbook=' .$row['nazev_grim']. '">historie</a></span>
					</div>';
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
			}

			if ($found_any)
				echo $form;
			
			else
				echo '<div class="table_head"><span class="table_msg">Nebyly nalezeny žádné výsledky</span></div>';
		?>
	</div>
</div>

<? include("footer.php"); ?>