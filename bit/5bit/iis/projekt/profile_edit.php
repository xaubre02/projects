<?
	include("check_login.php");
	include("header.php"); 
?>

<header>
	<nav class="main-menu">
		<a href="home.php">Home</a>
		<a href="overview_wizards.php">Přehled</a>
		<a href="#" class="selected"><? echo $_SESSION['login']?></a>
		<form method="get">
			<input type="submit" name="logout" value="Odhlásit se">
		</form>
	</nav>
	<nav class="sub-menu">
		<a href="profile_overview.php">Přehled</a>
		<a href="#" class="selected">Upravit profil</a>
		<a href="profile_chpwd.php">Změnit heslo</a>
	</nav>
</header>

<div class="content">
	<?
		if ($_SESSION['login'] == "admin")
		{
			echo '<h1>Účet administrátora nelze upravovat</h1></div>';
			include("footer.php");
			return;
		}
	?>
	<h1>Upravit profil</h1>
	<form action="profile_edit.php" method="post">
		<?php
		// pripojeni k databazi a ziskani informaci o aktualnim uzivateli
		include("connect_to_db.php");
		$sqlQuery = 'SELECT * FROM Kouzelnik WHERE jmeno_kouz=\'' . $_SESSION['login'] . '\'';
		$wiz = mysql_query($sqlQuery, $db);
		$wiz = mysql_fetch_array($wiz);
		
		$sqlQuery = 'SELECT druh FROM Element';
		$elem = mysql_query($sqlQuery, $db);
		
		$sqlQuery = 'SELECT nazev_kouz FROM Kouzlo';
		$kouz = mysql_query($sqlQuery, $db);
		
		$sqlQuery = 'SELECT nazev_kouz FROM Kouz_umi WHERE jmeno_kouz=\'' . $_SESSION['login'] . '\'';
		$umi = mysql_query($sqlQuery, $db);
		
		if(isset($_POST['chprof']))
		{
			$error = FALSE;
			$page = '<div class="text_input">
						<label for="username">Jméno:</label>';
			
			// zpracovani uzivatelskeho jmena
			if(strlen($_POST['username']) >= 4 and strlen($_POST['username']) <= 100)
			{
				// zjisteni, zdali zadany kouzelnik existuje
				$sqlQuery = 'SELECT jmeno_kouz FROM Kouzelnik WHERE jmeno_kouz = \'' . $_POST['username'] . '\'';
				$result = mysql_query($sqlQuery, $db);
				
				if ((mysql_num_rows($result) != 0 and $_POST['username'] != $_SESSION['login']) or $_POST['username'] == "admin")
				{
					$error = TRUE;
					$page .= '	<input type="text" name="username" value="'.$_POST['username'].'" class="bad-input">
							</div>
							<span class="err-msg">Zadané uživatelské jméno již existuje.</span>';
				}
				else
				{
					$page .= '	<input type="text" name="username" value="'.$_POST['username'].'">
							</div>';
				}
			}
			else
			{
				$error = TRUE;
				$page .= '	<input type="text" name="username" value="'.$_POST['username'].'" class="bad-input">
						</div>
						<span class="err-msg">Uživatelské jméno musí obsahovat alespoň 4 znaky a nesmí být delší než 100 znaků.</span>';
			}
			
			// zpracovani many - Mana musí být celé číslo v rozsahu 0 - 100 000
			$mana = $_POST['mana'];
			if ( $mana == "" or (is_numeric($mana) and $mana >= 0 and $mana <= 100000))
			{
				// defaultní hodnota
				if ( $mana == "")
					$mana = 0;
				
				$page .= '<div class="text_input">
							<label for="mana">Mana:</label>
							<input type="text" name="mana" value="'.$_POST['mana'].'">
						</div>';
			}
			else
			{
				$error = TRUE;
				$page .= '<div class="text_input">
							<label for="mana">Mana:</label>
							<input type="text" name="mana" value="'.$_POST['mana'].'" class="bad-input">
						</div>
				<span class="err-msg">Mana musí být celé číslo v rozsahu 0&nbsp-&nbsp100&nbsp000.</span>';
			}
			
			// zpracovani dosazene urovne kouzleni
			$level = $_POST['level'];
			if ($level != "" and $level != "SS" and $level != "S" and $level != "A" and $level != "B" and $level != "C" and $level != "D" and $level != "E")
			{
				$error = TRUE;
				$page.='<div class="text_input">
							<label for="level">Úroveň:</label>
							<input type="text" name="level" value="'.$_POST['level'].'" class="bad-input">
						</div>
						<span class="err-msg">Dostupné dosažené úrovně kouzlení: SS, S, A, B, C, D, E.</span>';
			}
			else
			{
				// defaultní hodnota
				if ( $level == "")
					$level = "E";
				
				$page.='<div class="text_input">
							<label for="level">Úroveň:</label>
							<input type="text" name="level" value="'.$_POST['level'].'">
						</div>';
			}
			
			// zpracovani synergie
			$page .= '<div class="text_input">
						<label for="synergy">Synergie:</label>
						<div class="choice_array">
							<div class="choice">
								<input type="radio" name="synergy" checked="checked"><span>Žádná</span><br>
							</div>';
						while ($row = mysql_fetch_array($elem))
						{
							$page .= '	<div class="choice">';
							if ($_POST['synergy'] == $row['druh'])
								$page .= '<input type="radio" name="synergy" checked="checked" value="'.$row['druh'].'"><span>'. $row['druh'] .'</span><br></div>';
							else
								$page .= '<input type="radio" name="synergy" value="'.$row['druh'].'"><span>'. $row['druh'] .'</span><br></div>';
						}
			$page .= '</div> </div>';
			
			// zpracovani kouzel
			$page .= '<div class="text_input">
						<label for="spells">Kouzla:</label>
						<div class="choice_array">';
			while ($spell = mysql_fetch_array($kouz))
			{
				if (!empty($_POST['check_list']))
				{
					if(in_array($spell['nazev_kouz'], $_POST['check_list']))
					{
						$page .='	<div class="choice">
							<input type="checkbox" name="check_list[]" value="'.$spell['nazev_kouz'].'" checked="checked"><span>'. $spell['nazev_kouz'] .'</span><br></div>';
					}
					else
						$page .='	<div class="choice">
							<input type="checkbox" name="check_list[]" value="'.$spell['nazev_kouz'].'"><span>'. $spell['nazev_kouz'] .'</span><br></div>';
				}
				else
					$page .='	<div class="choice">
						<input type="checkbox" name="check_list[]" value="'.$spell['nazev_kouz'].'"><span>'. $spell['nazev_kouz'] .'</span><br></div>';
			}
			$page .= '</div> </div> <input type="submit" value="Upravit" class="button" name="chprof">';
						
			if (!$error)
			{	
				// odstraneni dosavadnich kouzel, ktere kouzelnik umi
				$sqlQuery = 'DELETE FROM Kouz_umi WHERE jmeno_kouz=\''.$_SESSION['login'].'\'';
				$result = mysql_query($sqlQuery, $db);
				
				// upraveni udaju v databazi
				$sqlQuery = 'UPDATE Kouzelnik SET jmeno_kouz=\''. $_POST['username'] .'\', mana=\''. $_POST['mana'] .'\', uroven=\''. $_POST['level'] .'\', synergie=';
				if ($_POST['synergy'] == "none")
					$sqlQuery .= "NULL";
				else
					$sqlQuery .= '\''. $_POST['synergy'] . '\'';
				
				$sqlQuery .= ' WHERE jmeno_kouz = \'' . $_SESSION['login'] . '\'';
				$result = mysql_query($sqlQuery, $db);
				
				// pridani novych kouzel, ktere kouzelnik umi
				if (!empty($_POST['check_list']))
				{
					foreach($_POST['check_list'] as $new_spell)
					{
						$sqlQuery = 'INSERT INTO Kouz_umi (jmeno_kouz, nazev_kouz) VALUES (\''. $_POST['username'] .'\', \''. $new_spell .'\')';
						$result = mysql_query($sqlQuery, $db);
					}
				}
				
				$_SESSION['login'] = $_POST['username'];

				echo '	<p>Změny byly úspěšně uloženy!</p>
						<a href="profile_overview.php" class="button">Rozumím</a>';
			}
			else
				echo $page;
			
			mysql_close($db);
		}
		else
		{	// vyhozi zobrazeni formulare
			echo'	<div class="text_input">
						<label for="username">Jméno:</label>
						<input type="text" name="username" value="'.$wiz['jmeno_kouz'].'">
					</div>
					<div class="text_input">
						<label for="mana">Mana:</label>
						<input type="text" name="mana" value="'.$wiz['mana'].'">
					</div>
					<div class="text_input">
						<label for="level">Úroveň:</label>
						<input type="text" name="level" value="'.$wiz['uroven'].'">
					</div>
					<div class="text_input">
						<label for="synergy">Synergie:</label>
						<div class="choice_array">
							<div class="choice">
								<input type="radio" name="synergy" checked="checked" value="none"><span>Žádná</span><br>
							</div>';
							while ($row = mysql_fetch_array($elem))
							{
								echo '	<div class="choice">';
								if ($wiz['synergie'] == $row['druh'])
									echo '<input type="radio" name="synergy" checked="checked" value="'.$row['druh'].'"><span>'. $row['druh'] .'</span><br></div>';
								else
									echo '<input type="radio" name="synergy" value="'.$row['druh'].'"><span>'. $row['druh'] .'</span><br></div>';
							}
			echo'			</div>
					</div>
					<div class="text_input">
						<label for="spells">Kouzla:</label>
						<div class="choice_array">';
						
						$umi_kouzla = array();
						while ($spell_learned = mysql_fetch_array($umi))
						{
							$umi_kouzla[] = $spell_learned;
						}
						
						while ($spell = mysql_fetch_array($kouz))
						{
							$contains = FALSE;
							foreach ($umi_kouzla as $spell_learned)
							{
								if ($spell['nazev_kouz'] == $spell_learned['nazev_kouz'])
								{
									$contains = TRUE;
									echo '	<div class="choice">
										<input type="checkbox" name="check_list[]" value="'.$spell['nazev_kouz'].'" checked="checked"><span>'. $spell['nazev_kouz'] .'</span><br></div>';
									break;
								}
							}
							if (!$contains)
								echo '	<div class="choice">
									<input type="checkbox" name="check_list[]" value="'.$spell['nazev_kouz'].'"><span>'. $spell['nazev_kouz'] .'</span><br></div>';
						}
			echo'		</div>
					</div>
					<input type="submit" value="Upravit" class="button" name="chprof">';
		}
		?>
	</form>
</div>

<? include("footer.php"); ?>