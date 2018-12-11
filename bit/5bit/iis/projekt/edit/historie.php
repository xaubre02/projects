<?php
	// ziskani informaci
	$keys = explode("&", $_COOKIE['id']);
	$sqlQuery = 'SELECT * FROM Kouz_mel WHERE jmeno_kouz=\''.$keys[0].'\' AND nazev_grim=\''. $keys[1] .'\'';
	$record = mysql_query($sqlQuery, $db);
	$record = mysql_fetch_array($record);
	
	//ziskani vsech grimoaru
	$sqlQuery = 'SELECT * FROM Grimoar';
	$grim = mysql_query($sqlQuery, $db);
	
	//ziskani vsech kouzelniku
	$sqlQuery = 'SELECT * FROM Kouzelnik';
	$wizard = mysql_query($sqlQuery, $db);
	
	//ziskani historie vlastnictvi
	$sqlQuery = 'SELECT * FROM Kouz_mel';
	$history = mysql_query($sqlQuery, $db);
	
	if(isset($_POST['chrec']))
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
				<input type="hidden" name="id" value="'. $_POST['id'] .'">
				<div class="button_choice">
					<a href="'. $_COOKIE['last_page']. '?spellbook='. $record['nazev_grim'] .'" class="button">Zrušit</a>
					<input type="submit" value="Upravit" class="button" name="chrec">
				</div>';
				
					
		if (!$error)
		{
			// upraveni udaju v databazi
			$sqlQuery = 'UPDATE Kouz_mel SET jmeno_kouz=\''. $_POST['owner'] .'\', nazev_grim=\''. $_POST['grim'] .'\', mel_od=\''. $datum_od .'\', mel_do=\''. $datum_do .'\' WHERE jmeno_kouz =\'' . $keys[0] . '\' AND nazev_grim =\'' . $keys[1] . '\'';
			$result = mysql_query($sqlQuery, $db);

			echo '	<p>Změny byly úspěšně uloženy!</p>
					<a href="'. $_COOKIE['last_page']. '?spellbook='. $record['nazev_grim'] .'" class="button">Rozumím</a>';
		}
		else
			echo $page;
	}
	// vychozi zobrazeni
	else
	{
		$phpdate = strtotime( $record['mel_od'] );
		$mel_od = date( 'd.m. Y', $phpdate );
		$phpdate = strtotime( $record['mel_do'] );
		$mel_do = date( 'd.m. Y', $phpdate );
					
		echo'	<div class="text_input">
					<label for="grim">Grimoár:</label>
					<div class="choice_array">';
					while ($row = mysql_fetch_array($grim))
					{
						echo'<div class="choice">';
						if ($record['nazev_grim'] == $row['nazev_grim'])
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
					{
						echo'<div class="choice">';
						if ($record['jmeno_kouz'] == $row['jmeno_kouz'])
							echo '<input type="radio" name="owner" checked="checked" value="'.$row['jmeno_kouz'].'"><span>'. $row['jmeno_kouz'] .'</span><br></div>';
						else
							echo '<input type="radio" name="owner" value="'.$row['jmeno_kouz'].'"><span>'. $row['jmeno_kouz'] .'</span><br></div>';
					}
		echo '		</div>
				</div>
				<div class="text_input">
					<label for="datum_od">Vlastnil od:</label>
					<input id="datum_od" type="text" name="datum_od" required value="'. date_format(date_create_from_format('Y-m-d', $record['mel_od']), 'd.m. Y') .'">
				</div>
				<div class="text_input">
					<label for="datum_do">Vlastnil do:</label>
					<input id="datum_do" type="text" name="datum_do" required value="'. date_format(date_create_from_format('Y-m-d', $record['mel_do']), 'd.m. Y') .'">
				</div>
				<input type="hidden" name="table" value="'. $_GET['table'] .'">
				<input type="hidden" name="id" value="'. $_COOKIE['id'] .'">
				<div class="button_choice">
					<a href="'. $_COOKIE['last_page']. '?spellbook='. $record['nazev_grim'] .'" class="button">Zrušit</a>
					<input type="submit" value="Upravit" class="button" name="chrec">
				</div>';
	}
?>