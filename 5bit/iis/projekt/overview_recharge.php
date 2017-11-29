<?
	include("check_login.php");
	include("header.php"); 
	
	if (isset($_GET['add']))
	{
		setcookie("last_page", basename($_SERVER['PHP_SELF']));
		header('Location: overview_add.php?table=Nabiti&jmeno='.$_GET['place']);
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
		<a href="overview_spellbooks.php">Grimoáry</a>
		<a href="overview_scrolls.php">Svitky</a>
		<a href="overview_elements.php">Elementy</a>
		<a href="overview_ded_places.php">Dedikovaná místa</a>
		<a href="overview_recharge.php" class="selected">Nabití</a>
	</nav>
</header>

<div class="content">
	<h1>Nabití</h1>
	<div class="search">
		<form method="get">
			<input type="text" placeholder="Nabití" name="recharge" value="<? if(isset($_GET['search'])) echo $_GET['recharge'] ?>">
			<input type="submit" name="search" value="Hledat" class="button">
			<?
				if ($_SESSION['login'] == "admin")
					echo '<input type="submit" name="add" value="Přidat" class="button" id="add">';
			?>
		</form>
	</div>
	
	<div class="table" style="min-width:950px">
		<?php
			include("connect_to_db.php");
			$form = '<div class="table_head">
						<span style="width:240px">Grimoár</span>
						<span style="width:240px">Kouzelník</span>
						<span>Datum</span>
						<span style="width:240px">Místo</span>
					</div>';
			
			$sqlQuery = 'SELECT * FROM Nabiti ORDER BY nazev_grim';
			$result = mysql_query($sqlQuery, $db);
			$found_any = FALSE;
			
			while ($row = mysql_fetch_array($result))
			{
				$datum = date_format(date_create_from_format('Y-m-d', $row['datum']), 'd.m. Y');
				if (isset($_GET['search']))
				{
					if ($_GET['recharge'] == $row['nazev_grim'] or $_GET['recharge'] == $row['jmeno_kouz'] or $_GET['recharge'] == $datum or $_GET['recharge'] == "")
					{
						$found_any = TRUE;
						$datum = date_format(date_create_from_format('Y-m-d', $row['datum']), 'd.m. Y');
						$form .= '<br><div class="table_content">
							<a href="overview_spellbooks.php?spellbook='.$row['nazev_grim'].'&search=Hledat"><span style="width:240px">' .$row['nazev_grim']. '</span></a>
							<a href="overview_wizards.php?wizard='.$row['jmeno_kouz'].'&search=Hledat"><span style="width:240px">' .$row['jmeno_kouz']. '</span></a>
							<span>' .$datum. '</span>
							<span style="width:240px">';
							
							$sqlQuery = 'SELECT misto FROM Nab_ded_mista WHERE Nabiti=\''.$row['id_nabiti'].'\'';
							$result2 = mysql_query($sqlQuery, $db);
							
							
							while ($row2 = mysql_fetch_array($result2))
							{
								$form .= '<a href="overview_ded_places.php?place='.$row2['misto'].'&search=Hledat">'.$row2['misto'].'</a><br>';
							}
							$form .= '</span></div>';
							
						// dalsi prava pro admina - editace a odstraneni
						if ($_SESSION['login'] == "admin")
						{
							$form .= '	<a href="overview_edit.php?table=Nabiti" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['id_nabiti'] .'\')">
											<img src="https://www.stud.fit.vutbr.cz/~xaubre02/pic/edit.png" Title="Upravit">
										</a>
										<a href="overview_delete.php?ozn=nabití&table=Nabiti&item=id_nabiti" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['id_nabiti'] .'\')">
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
						<a href="overview_spellbooks.php?spellbook='.$row['nazev_grim'].'&search=Hledat"><span style="width:240px">' .$row['nazev_grim']. '</span></a>
						<a href="overview_wizards.php?wizard='.$row['jmeno_kouz'].'&search=Hledat"><span style="width:240px">' .$row['jmeno_kouz']. '</span></a>
						<span>' .$datum. '</span>
						<span style="width:240px">';
						
						$sqlQuery = 'SELECT * FROM Nab_ded_mista WHERE Nabiti=\''.$row['id_nabiti'].'\'';
						$result2 = mysql_query($sqlQuery, $db);
						
						
						while ($row2 = mysql_fetch_array($result2))
						{
							$form .= '<a href="overview_ded_places.php?place='.$row2['misto'].'&search=Hledat">'.$row2['misto'].'</a><br>';
						}
						$form .= '</span></div>';
					
					// dalsi prava pro admina - editace a odstraneni
					if ($_SESSION['login'] == "admin")
					{
						$form .= '	<a href="overview_edit.php?table=Nabiti" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['id_nabiti'] .'\')">
										<img src="https://www.stud.fit.vutbr.cz/~xaubre02/pic/edit.png" Title="Upravit">
									</a>
									<a href="overview_delete.php?ozn=toto nabití&table=Nabiti&item=id_nabiti" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['id_nabiti'] .'\')">
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