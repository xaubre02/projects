<?
	include("check_login.php");
	include("header.php"); 
		
	if (isset($_GET['add']))
	{
		setcookie("last_page", basename($_SERVER['PHP_SELF']));
		header('Location: overview_add.php?table=Svitek&nazev='.$_GET['scroll']);
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
		<a href="overview_scrolls.php" class="selected">Svitky</a>
		<a href="overview_elements.php">Elementy</a>
		<a href="overview_ded_places.php">Dedikovaná místa</a>
		<a href="overview_recharge.php">Nabití</a>
	</nav>
</header>

<div class="content">
	<h1>Svitky</h1>
	<div class="search">
		<form method="get">
			<input type="text" placeholder="Svitek" name="scroll">
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
						<span style="width:240px">Název</span>
						<span>Kouzlo</span>
						<span style="width:240px">Majitel</span>
					</div>';
			
			$sqlQuery = 'SELECT * FROM Svitek';
			$result = mysql_query($sqlQuery, $db);
			$found_any = FALSE;
			
			while ($row = mysql_fetch_array($result))
			{
				if (isset($_GET['search']))
				{
					if ($_GET['scroll'] == $row['nazev'] or $_GET['scroll'] == $row['nazev_kouz'] or $_GET['scroll'] == $row['jmeno_kouz'] or $_GET['scroll'] == "")
					{
						$found_any = TRUE;
						$form .= '<br><div class="table_content">
										<span style="width:240px">' .$row['nazev']. '</span>
										<a href="overview_spells.php?spell='.$row['nazev_kouz'].'&search=Hledat"><span>' .$row['nazev_kouz']. '</span></a>
										<span style="width:240px">'; 
										
										if ($row['jmeno_kouz'] == "")
											$form .= 'není';
										else
											$form .= '<a href="overview_wizards.php?wizard='.$row['jmeno_kouz'].'&search=Hledat">'.$row['jmeno_kouz'].'</a>';
										
						$form .= '</span></div>';
						// dalsi prava pro admina - editace a odstraneni
						if ($_SESSION['login'] == "admin")
						{
							$form .= '	<a href="overview_edit.php?table=Svitek" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['nazev'] .'\')">
											<img src="https://www.stud.fit.vutbr.cz/~xaubre02/pic/edit.png" Title="Upravit">
										</a>
										<a href="overview_delete.php?ozn=svitek&table=Svitek&item=id_svitku" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['nazev'] .'\')">
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
							<span style="width:240px">' .$row['nazev']. '</span>
							<a href="overview_spells.php?spell='.$row['nazev_kouz'].'&search=Hledat"><span>' .$row['nazev_kouz']. '</span></a>
							<span style="width:240px">'; 

					if ($row['jmeno_kouz'] == "")
						$form .= 'není';
					else
						$form .= '<a href="overview_wizards.php?wizard='.$row['jmeno_kouz'].'&search=Hledat">'.$row['jmeno_kouz'].'</a>';

					$form .= '</span></div>';
					// dalsi prava pro admina - editace a odstraneni
					if ($_SESSION['login'] == "admin")
					{
						$form .= '	<a href="overview_edit.php?table=Svitek" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['nazev'] .'\')">
										<img src="https://www.stud.fit.vutbr.cz/~xaubre02/pic/edit.png" Title="Upravit">
									</a>
									<a href="overview_delete.php?ozn=svitek&table=Svitek&item=id_svitku" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['nazev'] .'\')">
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