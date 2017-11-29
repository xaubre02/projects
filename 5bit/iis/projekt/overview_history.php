<?
	include("check_login.php");
	include("header.php");
	if (!isset($_GET['spellbook']))
		header('Location: overview_spellbooks.php');

	if (isset($_GET['add']))
	{
		setcookie("last_page", basename($_SERVER['PHP_SELF']));
		header('Location: overview_add.php?table=Kouz_mel&grimoar='.$_GET['spellbook']);
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
	<h1><?echo$_GET['spellbook']?></h1>
	<div class="search">
		<form <? echo 'action="overview_history.php?spellbook='. $_GET['spellbook'] .'"' ?> method="get">
			<input type="text" placeholder="Záznam" name="record" value="<? if(isset($_GET['search'])) echo $_GET['record'] ?>">
			<input type="submit" name="search" value="Hledat" class="button">
			<input type="hidden" name="spellbook" value=<? echo '"'. $_GET['spellbook'] .'"'?>>
			<?
				if ($_SESSION['login'] == "admin")
					echo '<input type="submit" name="add" value="Přidat" class="button" id="add">';
			?>
		</form>
	</div>
	<div class="table">
		<?php
			include("connect_to_db.php");
			$form = '';
			
			$sqlQuery = 'SELECT * FROM Kouz_mel WHERE nazev_grim=\''.$_GET['spellbook']. '\' ORDER BY mel_od';
			$result = mysql_query($sqlQuery, $db);

			if (mysql_num_rows($result) == 0)
				echo $form . '<div class="table_head"><span class="table_msg">Nikdo tento grimoár ještě nevlastnil</span></div>';
		
			else
			{
				$form.='<div class="table_head">
							<span style="width:200px">Majitel</span>
							<span>Od</span>
							<span>Do</span>
						</div>';
						
				$found_any = FALSE;
				
				while ($row = mysql_fetch_array($result))
				{
					if (isset($_GET['search']))
					{
						if ($_GET['record'] == $row['jmeno_kouz'] or $_GET['record'] == date_format(date_create_from_format('Y-m-d', $row['mel_od']), 'd.m. Y') or $_GET['record'] == date_format(date_create_from_format('Y-m-d', $row['mel_do']), 'd.m. Y') or $_GET['record'] == "")
						{
							$found_any = TRUE;
							$phpdate = strtotime( $row['mel_od'] );
							$mel_od = date( 'd.m. Y', $phpdate );
							$phpdate = strtotime( $row['mel_do'] );
							$mel_do = date( 'd.m. Y', $phpdate );
							
							$form .= '<br><div class="table_content">
											<a href="overview_wizards.php?wizard='.$row['jmeno_kouz'].'&search=Hledat"><span style="width:200px">' .$row['jmeno_kouz']. '</span></a>
											<span>' .$mel_od. '</span>
											<span>' .$mel_do. '</span>
									</div>';
							
							// dalsi prava pro admina - editace a odstraneni
							if ($_SESSION['login'] == "admin")
							{
								$form .= '	<a href="overview_edit.php?table=Kouz_mel" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['jmeno_kouz'].'&'.$row['nazev_grim'] .'\')">
												<img src="https://www.stud.fit.vutbr.cz/~xaubre02/pic/edit.png" Title="Upravit">
											</a>
											<a href="overview_delete.php?ozn=záznam historie vlastnictví&table=Kouz_mel&item=jmeno_kouz" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['jmeno_kouz'].'&'.$row['nazev_grim'] .'\')">
												<img src="https://www.stud.fit.vutbr.cz/~xaubre02/pic/delete.png" Title="Odstranit">
											</a>';
								setcookie("last_page", basename($_SERVER['PHP_SELF']));
							}
						}
					}
					else
					{
						$found_any = TRUE;
						$phpdate = strtotime( $row['mel_od'] );
						$mel_od = date( 'd.m. Y', $phpdate );
						$phpdate = strtotime( $row['mel_do'] );
						$mel_do = date( 'd.m. Y', $phpdate );
						
						$form .= '<br><div class="table_content">
										<a href="overview_wizards.php?wizard='.$row['jmeno_kouz'].'&search=Hledat"><span style="width:200px">' .$row['jmeno_kouz']. '</span></a>
										<span>' .$mel_od. '</span>
										<span>' .$mel_do. '</span>
								</div>';
						
						// dalsi prava pro admina - editace a odstraneni
						if ($_SESSION['login'] == "admin")
						{
							$form .= '	<a href="overview_edit.php?table=Kouz_mel" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['jmeno_kouz'].'&'.$row['nazev_grim'] .'\')">
											<img src="https://www.stud.fit.vutbr.cz/~xaubre02/pic/edit.png" Title="Upravit">
										</a>
										<a href="overview_delete.php?ozn=záznam historie vlastnictví&table=Kouz_mel&item=jmeno_kouz" class="edit_pic" onclick="set_cookie(\'id\', \''. $row['jmeno_kouz'].'&'.$row['nazev_grim'] .'\')">
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
			}
		?>
	</div>
</div>

<? include("footer.php"); ?>