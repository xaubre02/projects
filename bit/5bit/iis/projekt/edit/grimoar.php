<?php
	// ziskani informaci
	$sqlQuery = 'SELECT * FROM Grimoar WHERE nazev_grim=\''. $_COOKIE['id'] . '\'';
	$grim = mysql_query($sqlQuery, $db);
	$grim = mysql_fetch_array($grim);
	
	//ziskani vsech elementu
	$sqlQuery = 'SELECT * FROM Element';
	$elem = mysql_query($sqlQuery, $db);
	
	//ziskani vsech kouzel
	$sqlQuery = 'SELECT * FROM Kouzlo';
	$spell = mysql_query($sqlQuery, $db);
	
	//ziskani vsech kouzelniku
	$sqlQuery = 'SELECT * FROM Kouzelnik';
	$wizard = mysql_query($sqlQuery, $db);
	
	//ziskani vsech kouzel, ktera jsou obsazena v grimoaru
	$sqlQuery = 'SELECT * FROM Grim_obsah WHERE nazev_grim="'. $_COOKIE['id'] .'"';
	$contain = mysql_query($sqlQuery, $db);
	
	if(isset($_POST['chgrim']))
	{
		$error = FALSE;
		$page = '<div class="text_input">
					<label for="nazev">Název:</label>';
			
		// zpracovani jmena
		if(strlen($_POST['nazev']) >= 4 and strlen($_POST['nazev']) <= 100)
		{
			// zjisteni, zdali zadany grimoar existuje
			$sqlQuery = 'SELECT nazev_grim FROM Grimoar WHERE nazev_grim = \'' . $_POST['nazev'] . '\'';
			$result = mysql_query($sqlQuery, $db);
			
			if (mysql_num_rows($result) != 0 and $_POST['nazev'] != $_COOKIE['id'])
			{
				$error = TRUE;
				$page .= '	<input type="text" name="nazev" value="'.$_POST['nazev'].'" class="bad-input">
						</div>
						<span class="err-msg">Zadané jméno již existuje!</span>';
			}
			else
			{
				$page .= '	<input type="text" name="nazev" value="'.$_POST['nazev'].'">
						</div>';
			}
		}
		else
		{
			$error = TRUE;
			$page .= '	<input type="text" name="nazev" value="'.$_POST['nazev'].'" class="bad-input">
					</div>
					<span class="err-msg">Jméno grimoáru musí obsahovat alespoň 4 znaky a nesmí být delší než 100 znaků.</span>';
		}
		
		// zpracovani many
		if (is_numeric($_POST['mana']) and $_POST['mana'] >= 0 and $_POST['mana'] <= 100000)
			$page.='<div class="text_input">
						<label for="mana">Mana:</label>
						<input type="text" name="mana" value="' .$_POST['mana']. '">
					</div>';
		else
		{
			$error = TRUE;
			$page .= '<div class="text_input">
						<label for="mana">Mana:</label>
						<input type="text" name="mana" value="' .$_POST['mana']. '" class="bad-input">
					</div>
					<span class="err-msg">Mana musí být celé číslo v rozsahu 0&nbsp-&nbsp100&nbsp000.</span>';
		}
		
		// zpracovani elementu
		$page.='<div class="text_input">
					<label for="elem">Element:</label>
					<div class="choice_array">';
					while ($row = mysql_fetch_array($elem))
					{
						$page .='<div class="choice">';
						if ($_POST['elem'] == $row['druh'])
							$page .= '<input type="radio" name="elem" checked="checked" value="'.$row['druh'].'"><span>'. $row['druh'] .'</span><br></div>';
						else
							$page .= '<input type="radio" name="elem" value="'.$row['druh'].'"><span>'. $row['druh'] .'</span><br></div>';
					}
		// zpracovani majitele
		$page .='		</div>
				</div>
				<div class="text_input">
					<label for="owner">Majitel:</label>
					<div class="choice_array">';
					$page .= '<div class="choice"><input type="radio" name="owner" value="není" '; 
					if ($_POST['owner'] == "není")
						$page .= 'checked="checked"';
					
					$page .= '><span>není</span><br></div>';
					while ($row = mysql_fetch_array($wizard))
					{
						$page .='<div class="choice">';
						if ($_POST['owner'] == $row['jmeno_kouz'])
							$page .= '<input type="radio" name="owner" checked="checked" value="'.$row['jmeno_kouz'].'"><span>'. $row['jmeno_kouz'] .'</span><br></div>';
						else
							$page .= '<input type="radio" name="owner" value="'.$row['jmeno_kouz'].'"><span>'. $row['jmeno_kouz'] .'</span><br></div>';
					}
		$page .='		</div>
					</div>';
		// zpracovani kouzel
		$page .= '<div class="text_input">
					<label for="spells">Kouzla:</label>
					<div class="choice_array">';
			$spell_err = FALSE;
			while ($row = mysql_fetch_array($spell))
			{
				if (!empty($_POST['check_list']))
				{
					if(in_array($row['nazev_kouz'], $_POST['check_list']))
					{
						$page .='	<div class="choice">
							<input type="checkbox" name="check_list[]" value="'.$row['nazev_kouz'].'" checked="checked"><span>'. $row['nazev_kouz'] .'</span><br></div>';
					}
					else
						$page .='	<div class="choice">
							<input type="checkbox" name="check_list[]" value="'.$row['nazev_kouz'].'"><span>'. $row['nazev_kouz'] .'</span><br></div>';
				}
				else
				{
					$error = TRUE;
					$spell_err = TRUE;
					$page .='	<div class="choice">
						<input type="checkbox" name="check_list[]" value="'.$row['nazev_kouz'].'"><span>'. $row['nazev_kouz'] .'</span><br></div>';
				}
			}
			if ($spell_err)
				$page .= '<span class="err-msg">Grimoár musí obsahovat alespoň jedno kouzlo.</span>';
			
			$page .= '</div> </div>
					<input type="hidden" name="table" value="'. $_POST['table'] .'">
					<input type="hidden" name="id" value="'. $_POST['id'] .'">
					<div class="button_choice">
						<a href="'. $_COOKIE['last_page'] .'" class="button">Zrušit</a>
						<input type="submit" value="Upravit" class="button" name="chgrim">
					</div>';
					
		if (!$error)
		{
			// odstraneni dosavadnich kouzel, ktera grimoar obsahuje
			$sqlQuery = 'DELETE FROM Grim_obsah WHERE nazev_grim=\''.$_COOKIE['id'].'\'';
			$result = mysql_query($sqlQuery, $db);

			// upraveni udaju v databazi
			$sqlQuery = 'UPDATE Grimoar SET nazev_grim=\''. $_POST['nazev'] .'\', mana=\''. $_POST['mana'] .'\', druh=\''. $_POST['elem'] .'\', jmeno_kouz=';
			if ($_POST['owner'] == "není")
				$sqlQuery .= "NULL";
			else
				$sqlQuery .= '\''. $_POST['owner'] .'\'';
			$sqlQuery .= ' WHERE nazev_grim=\'' . $_COOKIE['id'] . '\'';
			$result = mysql_query($sqlQuery, $db);

			// pridani novych kouzel, ktera grimoar obsahuje
			if (!empty($_POST['check_list']))
			{
				foreach($_POST['check_list'] as $new_spell)
				{
					$sqlQuery = 'INSERT INTO Grim_obsah (nazev_grim, nazev_kouz) VALUES (\''. $_POST['nazev'] .'\', \''. $new_spell .'\')';
					$result = mysql_query($sqlQuery, $db);
				}
			}
			
			echo '	<p>Změny byly úspěšně uloženy!</p>
					<a href="'. $_COOKIE['last_page'] .'" class="button">Rozumím</a>';
		}
		else
			echo $page;
	}
	// vychozi zobrazeni
	else
	{
		echo'	<div class="text_input">
					<label for="nazev">Název:</label>
					<input type="text" name="nazev" value="'.$grim['nazev_grim'].'">
				</div>
				<div class="text_input">
					<label for="mana">Mana:</label>
					<input type="text" name="mana" value="'.$grim['mana'].'">
				</div>
				<div class="text_input">
					<label for="elem">Element:</label>
					<div class="choice_array">';
					while ($row = mysql_fetch_array($elem))
					{
						echo'<div class="choice">';
						if ($grim['druh'] == $row['druh'])
							echo '<input type="radio" name="elem" checked="checked" value="'.$row['druh'].'"><span>'. $row['druh'] .'</span><br></div>';
						else
							echo '<input type="radio" name="elem" value="'.$row['druh'].'"><span>'. $row['druh'] .'</span><br></div>';
					}
		echo '		</div>
				</div>
				<div class="text_input">
					<label for="owner">Majitel:</label>
					<div class="choice_array">';
					echo '<div class="choice"><input type="radio" name="owner" value="není" '; 
					if ($grim['jmeno_kouz'] == "")
						echo 'checked="checked"';
					
					echo '><span>není</span><br></div>';
					while ($row = mysql_fetch_array($wizard))
					{
						echo'<div class="choice">';
						if ($grim['jmeno_kouz'] == $row['jmeno_kouz'])
							echo '<input type="radio" name="owner" checked="checked" value="'.$row['jmeno_kouz'].'"><span>'. $row['jmeno_kouz'] .'</span><br></div>';
						else
							echo '<input type="radio" name="owner" value="'.$row['jmeno_kouz'].'"><span>'. $row['jmeno_kouz'] .'</span><br></div>';
					}
		echo '		</div>
				</div>
				<div class="text_input">
					<label for="spells">Kouzla:</label>
					<div class="choice_array">';
					$obsah_kouzla = array();
					while ($spell_cont = mysql_fetch_array($contain))
					{
						$obsah_kouzla[] = $spell_cont;
					}
					
					while ($row = mysql_fetch_array($spell))
					{
						$contains = FALSE;
						foreach ($obsah_kouzla as $spell_cont)
						{
							if ($row['nazev_kouz'] == $spell_cont['nazev_kouz'])
							{
								$contains = TRUE;
								echo '	<div class="choice">
									<input type="checkbox" name="check_list[]" value="'.$row['nazev_kouz'].'" checked="checked"><span>'. $row['nazev_kouz'] .'</span><br></div>';
								break;
							}
						}
						if (!$contains)
							echo '	<div class="choice">
								<input type="checkbox" name="check_list[]" value="'.$row['nazev_kouz'].'"><span>'. $row['nazev_kouz'] .'</span><br></div>';
					}
		echo'		</div>
				</div>
		
				<input type="hidden" name="table" value="'. $_GET['table'] .'">
				<input type="hidden" name="id" value="'. $_COOKIE['id'] .'">
				<div class="button_choice">
					<a href="'. $_COOKIE['last_page'] .'" class="button">Zrušit</a>
					<input type="submit" value="Upravit" class="button" name="chgrim">
				</div>';
	}
?>