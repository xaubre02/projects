<?
	include("check_login.php");
	include("header.php");
	if (!isset($_GET['spellbook']))
		header('Location: overview_spellbooks.php');
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
	<h1><?echo$_GET['spellbook']?></h1>
	<div class="table">
		<?php
			include("connect_to_db.php");
			$form = '';
			
			$sqlQuery = 'SELECT * FROM Kouz_mel WHERE nazev_grim=\''.$_GET['spellbook']. '\'';
			$result = mysql_query($sqlQuery, $db);

			if (mysql_num_rows($result) == 0)
				$form .= '<div class="table_head"><span class="table_msg">Nikdo tento grimoár ještě nevlastnil</span></div>';
			
			else
			{
				$form.='<div class="table_head">
							<span>Majitel</span>
							<span>Od</span>
							<span>Do</span>
						</div>';
				while ($row = mysql_fetch_array($result))
				{
					$phpdate = strtotime( $row['mel_od'] );
					$mel_od = date( 'd.m. Y', $phpdate );
					$phpdate = strtotime( $row['mel_do'] );
					$mel_do = date( 'd.m. Y', $phpdate );
					
					$form .= '<br><div class="table_content">
									<span>' .$row['jmeno_kouz']. '</span>
									<span>' .$mel_od. '</span>
									<span>' .$mel_do. '</span>
							</div>';
				}
			}
			echo $form;
		?>
	</div>
</div>

<? include("footer.php"); ?>