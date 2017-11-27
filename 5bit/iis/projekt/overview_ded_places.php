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
		<a href="overview_ded_places.php" class="selected">Dedikovaná místa</a>
	</nav>
</header>

<div class="content">
	<h1>Dedikovaná místa</h1>
	<div class="search">
		<form method="get">
			<input type="text" placeholder="Místo" name="place" required>
			<input type="submit" name="search" value="Hledat" class="button">
		</form>
	</div>
	
	<div class="table">
		<?php
			include("connect_to_db.php");
			$form = '<div class="table_head">
						<span id="column_sec_element">Jméno</span>
						<span>Element</span>
						<span>Procento</span>
					</div>';
			
			$sqlQuery = 'SELECT * FROM Ded_misto';
			$result = mysql_query($sqlQuery, $db);
			$found_any = FALSE;
			
			while ($row = mysql_fetch_array($result))
			{
				if (isset($_GET['search']))
				{
					if ($_GET['place'] == $row['druh'] or $_GET['place'] == $row['jmeno'] or $_GET['place'] == $row['procento'])
					{
						$found_any = TRUE;
						$form .= '<br><div class="table_content">
										<span id="column_sec_element">' .$row['jmeno']. '</span>
										<span>' .$row['druh']. '</span>
										<span>' .$row['procento'].'%'. '</span></div>';
					}
				}
				else
				{
					$found_any = TRUE;
					$form .= '<br><div class="table_content">
						<span id="column_sec_element">' .$row['jmeno']. '</span>
						<span>' .$row['druh']. '</span>
						<span>' .$row['procento'].'%'. '</span></div>';

				}
				
				// dalsi prava pro admina - editace a odstraneni
				if ($_SESSION['login'] == "admin")
				{
					$form .= '	<a href="overview_edit.php?table=Ded_misto" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['jmeno'] .'\')">
									<img src="https://www.stud.fit.vutbr.cz/~xaubre02/pic/edit.png" Title="Upravit">
								</a>
								<a href="overview_delete.php?ozn=dedikované místo&table=Ded_misto&item=jmeno" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['jmeno'] .'\')">
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