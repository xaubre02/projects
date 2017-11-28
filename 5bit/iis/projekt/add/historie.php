<?php
	//ziskani vsech grimoaru
	$sqlQuery = 'SELECT * FROM Grimoar';
	$grim = mysql_query($sqlQuery, $db);
	
	//ziskani vsech kouzelniku
	$sqlQuery = 'SELECT * FROM Kouzelnik';
	$wizard = mysql_query($sqlQuery, $db);
	
	//ziskani historie vlastnictvi
	$sqlQuery = 'SELECT * FROM Kouz_mel';
	$history = mysql_query($sqlQuery, $db);
	
	if(isset($_POST['addrec']))
	{
		$error = FALSE;
		$owned = FALSE;
		
		// zpracovani data
		$datum_od = date_format(date_create_from_format('d.m. Y', $_POST['datum_od']), 'Y-m-d');
		$datum_do = date_format(date_create_from_format('d.m. Y', $_POST['datum_do']), 'Y-m-d');
		
		// kontrola, zdali v danem datu grimoar jiz nekdo nevlastnil
		while ($row = mysql_fetch_array($history))
		{
			if ( $_POST['grim'] == $row['nazev_grim'] and 	(
																($datum_od < $row['mel_od'] and $datum_do > $row['mel_od']) or 	// zacina pred a kryje se
																($datum_od < $row['mel_do'] and $datum_do > $row['mel_do']) or  // zacina po a kryje se 
																($datum_od == $row['mel_od'] and $datum_do == $row['mel_do'])   // kryje se presne na den
															)															
				) $owned = TRUE;
		}		
					
		$page = '<div class="text_input">
					<label for="grim">Grimoár:</label>
					<div class="choice_array">';
			while ($row = mysql_fetch_array($grim))
			{
				$page .='<div class="choice">';
				if ($_POST['grim'] == $row['nazev_grim'])
					$page .= '<input type="radio" name="grim" checked="checked" value="'.$row['nazev_grim'].'"><span>'. $row['nazev_grim'] .'</span><br></div>';
				else
					$page .= '<input type="radio" name="grim" value="'.$row['nazev_grim'].'"><span>'. $row['nazev_grim'] .'</span><br></div>';
			}
		$page .= '	</div>
				</div>
				<div class="text_input">
					<label for="owner">Majitel:</label>
					<div class="choice_array">';
					while ($row = mysql_fetch_array($wizard))
					{
						$page .='<div class="choice">';
						if ($_POST['owner'] == $row['jmeno_kouz'])
							$page .= '<input type="radio" name="owner" checked="checked" value="'.$row['jmeno_kouz'].'"><span>'. $row['jmeno_kouz'] .'</span><br></div>';
						else
							$page .= '<input type="radio" name="owner" value="'.$row['jmeno_kouz'].'"><span>'. $row['jmeno_kouz'] .'</span><br></div>';
					}
		$page .= '	</div>
				</div>
				<div class="text_input">
					<label for="datum_od">Vlastnil od:</label>';
		
		// kontrola data
		if ($datum_od > $datum_do)
		{	
			$error = TRUE;
			$page .='<input id="datum_od" type="text" name="datum_od" required value="'. $_POST['datum_od'] .'">
				</div>
				<div class="text_input">
					<label for="datum_do">Vlastnil do:</label>
					<input id="datum_do" type="text" name="datum_do" required value="'. $_POST['datum_do'] .'" class="bad-input">
				</div>
				<span class="err-msg">Datum konce vlastnictví nemůže být před jeho začátkem.</span>';
		}
		else
		{
			if ($owned)
			{	
				$error = TRUE;
				$page .=' <input id="datum_od" type="text" name="datum_od" required value="'. $_POST['datum_od'] .'" class="bad-input">
					</div>
					<div class="text_input">
						<label for="datum_do">Vlastnil do:</label>
						<input id="datum_do" type="text" name="datum_do" required value="'. $_POST['datum_do'] .'" class="bad-input">
					</div>
					<span class="err-msg">V zadaném období tento grimoár již někdo vlastnil.</span>';
			}
			else
				$page .='<input id="datum_od" type="text" name="datum_od" required value="'. $_POST['datum_od'] .'">
					</div>
					<div class="text_input">
						<label for="datum_do">Vlastnil do:</label>
						<input id="datum_do" type="text" name="datum_do" required value="'. $_POST['datum_do'] .'">
					</div>';
		}
		
		$page.='
				<input type="hidden" name="table" value="'. $_POST['table'] .'">
				<input type="hidden" name="spellbook" value="'. $_POST['spellbook'] .'">
				<div class="button_choice">
					<a href="'. $_COOKIE['last_page']. '?spellbook='. $_POST['spellbook'] .'" class="button">Zrušit</a>
					<input type="submit" value="Přidat" class="button" name="addrec">
				</div>';
				
					
		if (!$error)
		{
			// upraveni udaju v databazi
			$sqlQuery = 'INSERT INTO Kouz_mel(jmeno_kouz, nazev_grim, mel_od, mel_do) VALUES(\''. $_POST['owner'] .'\', \''. $_POST['grim'] .'\', \''. $datum_od .'\', \''. $datum_do .'\')';
			$result = mysql_query($sqlQuery, $db);

			echo '	<p>Záznam byl úspěšně uložen do databáze!</p>
					<a href="'. $_COOKIE['last_page']. '?spellbook='. $_POST['spellbook'] .'" class="button">Rozumím</a>';
		}
		else
			echo $page;
	}
	// vychozi zobrazeni
	else
	{			
		echo'	<div class="text_input">
					<label for="grim">Grimoár:</label>
					<div class="choice_array">';
					while ($row = mysql_fetch_array($grim))
					{
						echo'<div class="choice">';
						if ($_GET['grimoar'] == $row['nazev_grim'])
							echo '<input type="radio" name="grim" checked="checked" value="'.$row['nazev_grim'].'"><span>'. $row['nazev_grim'] .'</span><br></div>';
						else
							echo '<input type="radio" name="grim" value="'.$row['nazev_grim'].'"><span>'. $row['nazev_grim'] .'</span><br></div>';
					}
		echo '		</div>
				</div>
				<div class="text_input">
					<label for="owner">Majitel:</label>
					<div class="choice_array">';
					while ($row = mysql_fetch_array($wizard))
						echo'	<div class="choice">
									<input type="radio" name="owner" checked="checked" value="'.$row['jmeno_kouz'].'">
									<span>'. $row['jmeno_kouz'] .'</span><br>
								</div>';
		echo '		</div>
				</div>
				<div class="text_input">
					<label for="datum_od">Vlastnil od:</label>
					<input id="datum_od" type="text" name="datum_od" required>
				</div>
				<div class="text_input">
					<label for="datum_do">Vlastnil do:</label>
					<input id="datum_do" type="text" name="datum_do" required>
				</div>
				<input type="hidden" name="table" value="'. $_GET['table'] .'">
				<input type="hidden" name="spellbook" value="'. $_GET['grimoar'] .'">
				<div class="button_choice">
					<a href="'. $_COOKIE['last_page']. '?spellbook='. $_GET['grimoar'] .'" class="button">Zrušit</a>
					<input type="submit" value="Přidat" class="button" name="addrec">
				</div>';
	}
?>