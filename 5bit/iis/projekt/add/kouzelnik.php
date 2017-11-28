<?php
	$sqlQuery = 'SELECT druh FROM Element';
	$elem = mysql_query($sqlQuery, $db);
	
	$sqlQuery = 'SELECT nazev_kouz FROM Kouzlo';
	$kouz = mysql_query($sqlQuery, $db);
	
	$sqlQuery = 'SELECT nazev_kouz FROM Kouz_umi WHERE jmeno_kouz=\'' . $_COOKIE['id'] . '\'';
	$umi = mysql_query($sqlQuery, $db);
	
	if(isset($_POST['addwiz']))
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
			
			if (mysql_num_rows($result) != 0)
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
		
		$page .= '<div class="text_input">
					<label for="pwd_new0">Heslo:</label>';
		// zpracovani hesla
		$pwd0 = $_POST['pwd_new0'];
		$pwd1 = $_POST['pwd_new1'];
		if (strlen($pwd0) < 6)
		{
			$error = TRUE;
			$page .= '	<input type="password" name="pwd_new0" value="' . $_POST['pwd_new0'] . '" class="bad-input"></div>
					<span class="err-msg">Heslo musí obsahovat alespoň 6 znaků.</span>';
		}
		else
			$page .= '	<input type="password" name="pwd_new0" value="' . $_POST['pwd_new0'] . '"></div>';
			
		$page .='<div class="text_input">
					<label for="pwd_new1">Potvrdit heslo:</label>';

		if ($pwd0 != $pwd1)
		{
			$error = TRUE;
			$page .= '<input type="password" name="pwd_new1" class="bad-input">
				</div><span class="err-msg">Zadaná hesla se neshodují.</span>';
		}
		else
			$page .= '<input type="password" name="pwd_new1"></div>';
		
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
		$page .= '</div> </div>
				<input type="hidden" name="table" value="'. $_POST['table'] .'">
				<div class="button_choice">
					<a href="'. $_COOKIE['last_page'] .'" class="button">Zrušit</a>
					<input type="submit" value="Přidat" class="button" name="addwiz">
				</div>';
					
		if (!$error)
		{
			// pridani udaju do databaze
			$sqlQuery = 'INSERT INTO Kouzelnik(jmeno_kouz, heslo, mana, uroven, synergie) VALUES(\''. $_POST['username'] .'\', \''. md5($_POST['pwd_new0']) .'\', \''. $mana .'\', \''. $level .'\', ';
			if ($_POST['synergy'] == "none")
				$sqlQuery .= "NULL".')';
			else
				$sqlQuery .= '\''. $_POST['synergy'] . '\')';
			$result = mysql_query($sqlQuery, $db);
			
			// pridani kouzel, ktere kouzelnik umi
			if (!empty($_POST['check_list']))
			{
				foreach($_POST['check_list'] as $new_spell)
				{
					$sqlQuery = 'INSERT INTO Kouz_umi (jmeno_kouz, nazev_kouz) VALUES (\''. $_POST['username'] .'\', \''. $new_spell .'\')';
					$result = mysql_query($sqlQuery, $db);
				}
			}

			echo '	<p>Kouzelník byl úspěně uložen do databáze!</p>
					<a href="'. $_COOKIE['last_page'] .'" class="button">Rozumím</a>';
		}
		else
			echo $page;
		
	}
	// vyhozi zobrazeni
	else
	{
		echo'	<div class="text_input">
					<label for="username">Jméno:</label>
					<input type="text" name="username" value="'.$_GET['jmeno_kouz'].'">
				</div>
				<div class="text_input">
					<label for="pwd_new0">Heslo:</label>
					<input type="password" name="pwd_new0">
				</div>
				<div class="text_input">
					<label for="pwd_new1">Potvrdit heslo:</label>
					<input type="password" name="pwd_new1">
				</div>
				<div class="text_input">
					<label for="mana">Mana:</label>
					<input type="text" name="mana">
				</div>
				<div class="text_input">
					<label for="level">Úroveň:</label>
					<input type="text" name="level">
				</div>
				<div class="text_input">
					<label for="synergy">Synergie:</label>
					<div class="choice_array">
						<div class="choice">
							<input type="radio" name="synergy" checked="checked" value="none"><span>Žádná</span><br>
						</div>';
						while ($row = mysql_fetch_array($elem))
							echo '	<div class="choice">
										<input type="radio" name="synergy" checked="checked" value="'.$row['druh'].'">
										<span>'. $row['druh'] .'</span><br>
									</div>';

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
						echo '	<div class="choice">
									<input type="checkbox" name="check_list[]" value="'.$spell['nazev_kouz'].'">
									<span>'. $spell['nazev_kouz'] .'</span><br>
								</div>';
							
		echo'		</div>
				</div>
				<input type="hidden" name="table" value="'. $_GET['table'] .'">
				<div class="button_choice">
					<a href="'. $_COOKIE['last_page'] .'" class="button">Zrušit</a>
					<input type="submit" value="Přidat" class="button" name="addwiz">
				</div>';
	}
?>