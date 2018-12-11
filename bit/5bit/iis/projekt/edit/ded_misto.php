<?php
	// ziskani informaci
	$sqlQuery = 'SELECT * FROM Ded_misto WHERE jmeno=\''. $_COOKIE['id'] . '\'';
	$place = mysql_query($sqlQuery, $db);
	$place = mysql_fetch_array($place);
	
	//ziskani vsech elementu
	$sqlQuery = 'SELECT * FROM Element';
	$elem = mysql_query($sqlQuery, $db);
	
	if(isset($_POST['chdedp']))
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
			
			if (mysql_num_rows($result) != 0 and $_POST['jmeno'] != $_COOKIE['id'])
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
					<input type="hidden" name="id" value="'. $_POST['id'] .'">
					<div class="button_choice">
						<a href="'. $_COOKIE['last_page'] .'" class="button">Zrušit</a>
						<input type="submit" value="Upravit" class="button" name="chdedp">
					</div>';
					
		if (!$error)
		{
			// upraveni udaju v databazi
			$sqlQuery = 'UPDATE Ded_misto SET jmeno=\''. $_POST['jmeno'] .'\', druh=\''. $_POST['druh'] .'\', procento=\''. $_POST['procento'] .'\' WHERE jmeno =\'' . $_COOKIE['id'] . '\'';
			$result = mysql_query($sqlQuery, $db);
				
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
					<label for="jmeno">Jméno:</label>
					<input type="text" name="jmeno" value="'.$place['jmeno'].'">
				</div>
				<div class="text_input">
					<label for="element">Element:</label>
					<div class="choice_array">';
					while ($row = mysql_fetch_array($elem))
					{
						echo'<div class="choice">';
						if ($place['druh'] == $row['druh'])
							echo '<input type="radio" name="druh" checked="checked" value="'.$row['druh'].'"><span>'. $row['druh'] .'</span><br></div>';
						else
							echo '<input type="radio" name="druh" value="'.$row['druh'].'"><span>'. $row['druh'] .'</span><br></div>';
					}
		echo '		</div>
				</div>
				<div class="text_input">
					<label for="procento">Procent:</label>
					<input type="text" name="procento" value="'.$place['procento'].'">
				</div>
				<input type="hidden" name="table" value="'. $_GET['table'] .'">
				<input type="hidden" name="id" value="'. $_COOKIE['id'] .'">
				<div class="button_choice">
					<a href="'. $_COOKIE['last_page'] .'" class="button">Zrušit</a>
					<input type="submit" value="Upravit" class="button" name="chdedp">
				</div>';
	}
?>