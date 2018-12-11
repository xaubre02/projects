<?php
	//ziskani vsech elementu
	$sqlQuery = 'SELECT * FROM Element';
	$elem = mysql_query($sqlQuery, $db);
	
	if(isset($_POST['addplace']))
	{
		$error = FALSE;
		$page = '<div class="text_input">
					<label for="jmeno">Jméno:</label>';
			
		// zpracovani jmena
		if(strlen($_POST['jmeno']) >= 4 and strlen($_POST['jmeno']) <= 100)
		{
			// zjisteni, zdali zadane dedikovane misto existuje
			$sqlQuery = 'SELECT jmeno FROM Ded_misto WHERE jmeno = \'' . $_POST['jmeno'] . '\'';
			$result = mysql_query($sqlQuery, $db);
			
			if (mysql_num_rows($result) != 0)
			{
				$error = TRUE;
				$page .= '	<input type="text" name="jmeno" value="'.$_POST['jmeno'].'" class="bad-input">
						</div>
						<span class="err-msg">Zadané jméno již existuje!</span>';
			}
			else
			{
				$page .= '	<input type="text" name="jmeno" value="'.$_POST['jmeno'].'">
						</div>';
			}
		}
		else
		{
			$error = TRUE;
			$page .= '	<input type="text" name="jmeno" value="'.$_POST['jmeno'].'" class="bad-input">
					</div>
					<span class="err-msg">Jméno dedikovaného místa musí obsahovat alespoň 4 znaky a nesmí být delší než 100 znaků.</span>';
		}

		// zpracovani elementu
		$page.='<div class="text_input">
					<label for="element">Element:</label>
					<div class="choice_array">';
					while ($row = mysql_fetch_array($elem))
					{
						$page .='<div class="choice">';
						if ($_POST['druh'] == $row['druh'])
							$page .= '<input type="radio" name="druh" checked="checked" value="'.$row['druh'].'"><span>'. $row['druh'] .'</span><br></div>';
						else
							$page .= '<input type="radio" name="druh" value="'.$row['druh'].'"><span>'. $row['druh'] .'</span><br></div>';
					}
		$page .= 	'</div>
				</div>';

		// zpracovani procent
		if (is_numeric($_POST['procento']) and $_POST['procento'] >= 0 and $_POST['procento'] <= 100)
			$page.='<div class="text_input">
						<label for="procento">Procent:</label>
						<input type="text" name="procento" value="' .$_POST['procento']. '">
					</div>';
		else
		{
			$error = TRUE;
			$page .= '<div class="text_input">
						<label for="procento">Procent:</label>
						<input type="text" name="procento" value="' .$_POST['procento']. '" class="bad-input">
					</div>
					<span class="err-msg">Procento musí být celé číslo v rozsahu 0&nbsp-&nbsp100.</span>';
		}

		$page .= '	<input type="hidden" name="table" value="'. $_POST['table'] .'">
					<div class="button_choice">
						<a href="'. $_COOKIE['last_page'] .'" class="button">Zrušit</a>
						<input type="submit" value="Přidat" class="button" name="addplace">
					</div>';
					
		if (!$error)
		{
			// pridani udaju do databazi
			$sqlQuery = 'INSERT INTO Ded_misto(jmeno, druh, procento) VALUES(\''. $_POST['jmeno'] .'\', \''. $_POST['druh'] .'\', \''. $_POST['procento'] .'\')';
			$result = mysql_query($sqlQuery, $db);
				
			echo '	<p>Dedikované místo bylo úspěně uloženo do databáze!</p>
					<a href="'. $_COOKIE['last_page'] .'" class="button">Rozumím</a>';
		}
		else
			echo $page;
	}
	// vychozi zobrazeni
	else
	{
		echo'	<div class="text_input">
					<label for="jmeno">Jméno:</label>
					<input type="text" name="jmeno" value="'.$_GET['jmeno'].'">
				</div>
				<div class="text_input">
					<label for="element">Element:</label>
					<div class="choice_array">';
					while ($row = mysql_fetch_array($elem))
						echo'<div class="choice"><input type="radio" name="druh" checked="checked" value="'.$row['druh'].'"><span>'. $row['druh'] .'</span><br></div>';

		echo '		</div>
				</div>
				<div class="text_input">
					<label for="procento">Procent:</label>
					<input type="text" name="procento">
				</div>
				<input type="hidden" name="table" value="'. $_GET['table'] .'">
				<div class="button_choice">
					<a href="'. $_COOKIE['last_page'] .'" class="button">Zrušit</a>
					<input type="submit" value="Přidat" class="button" name="addplace">
				</div>';
	}
?>