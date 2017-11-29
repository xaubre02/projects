<?
	include("check_login.php");
	include("header.php"); 
	
	if (isset($_GET['add']))
	{
		setcookie("last_page", basename($_SERVER['PHP_SELF']));
		header('Location: overview_add.php?table=Element&druh='.$_GET['element']);
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
		<a href="overview_elements.php" class="selected">Elementy</a>
		<a href="overview_ded_places.php">Dedikovaná místa</a>
		<a href="overview_recharge.php">Nabití</a>
	</nav>
</header>

<div class="content">
	<h1>Elementy</h1>
	<div class="search">
		<form method="get">
			<input type="text" placeholder="Element" name="element" value="<? if(isset($_GET['search'])) echo $_GET['element'] ?>">
			<input type="submit" name="search" value="Hledat" class="button">
			<?
				if ($_SESSION['login'] == "admin")
					echo '<input type="submit" name="add" value="Přidat" class="button" id="add">';
			?>
		</form>
	</div>
	
	<div class="table" style="min-width:750px">		
		<?php
			include("connect_to_db.php");
			$form ='<div class="table_head">
						<span>Název</span>
						<span>Barva</span>
						<span>Specializace</span>
						<span style="width:240px">Místo prosakování</span>
					</div>';
			
			$sqlQuery = 'SELECT * FROM Element';
			$result = mysql_query($sqlQuery, $db);
			$found_any = FALSE;
			
			while ($row = mysql_fetch_array($result))
			{
				if (isset($_GET['search']))
				{
					if ($_GET['element'] == $row['druh'] or $_GET['element'] == $row['barva'] or $_GET['element'] == $row['specializace'] or $_GET['element'] == "")
					{
						$found_any = TRUE;
						$form .= '<br><div class="table_content">
										<span>' .$row['druh']. '</span>
										<span>' .$row['barva']. '</span>
										<span>' .$row['specializace']. '</span>
										<span style="width:240px">';
										
										$sqlQuery = 'SELECT jmeno FROM Ded_misto WHERE druh=\''.$row['druh'].'\'';
										$result2 = mysql_query($sqlQuery, $db);
										if (mysql_num_rows($result2) == 0)
											$form .= 'žádné';
										
										else
										{
											while ($row2 = mysql_fetch_array($result2))
											{
												$form .= '<a href="overview_ded_places.php?place='.$row2['jmeno'].'&search=Hledat">'.$row2['jmeno'].'</a><br>';
											}
										}
						$form .= '</span></div>';
						// dalsi prava pro admina - editace a odstraneni
						if ($_SESSION['login'] == "admin")
						{
							$form .= '	<a href="overview_edit.php?table=Element" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['druh'] .'\')">
											<img src="https://www.stud.fit.vutbr.cz/~xaubre02/pic/edit.png" Title="Upravit">
										</a>
										<a href="overview_delete.php?ozn=element&table=Element&item=druh" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['druh'] .'\')">
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
								<span>' .$row['druh']. '</span>
								<span>' .$row['barva']. '</span>
								<span>' .$row['specializace']. '</span>
								<span style="width:240px">';
								
								$sqlQuery = 'SELECT jmeno FROM Ded_misto WHERE druh=\''.$row['druh'].'\'';
								$result2 = mysql_query($sqlQuery, $db);
								if (mysql_num_rows($result2) == 0)
									$form .= 'žádné';
								
								else
								{
									while ($row2 = mysql_fetch_array($result2))
									{
										$form .= '<a href="overview_ded_places.php?place='.$row2['jmeno'].'&search=Hledat">'.$row2['jmeno'].'</a><br>';
									}
								}
					$form .= '</span></div>';
					// dalsi prava pro admina - editace a odstraneni
					if ($_SESSION['login'] == "admin")
					{
						$form .= '	<a href="overview_edit.php?table=Element" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['druh'] .'\')">
										<img src="https://www.stud.fit.vutbr.cz/~xaubre02/pic/edit.png" Title="Upravit">
									</a>
									<a href="overview_delete.php?ozn=element&table=Element&item=druh" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['druh'] .'\')">
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