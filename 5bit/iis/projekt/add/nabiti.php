<?php
	//ziskani vsech mist
	$sqlQuery = 'SELECT * FROM Ded_misto';
	$place = mysql_query($sqlQuery, $db);
	
	//ziskani vsech grimoaru
	$sqlQuery = 'SELECT * FROM Grimoar';
	$grim = mysql_query($sqlQuery, $db);
	
	//ziskani vsech kouzelniku
	$sqlQuery = 'SELECT * FROM Kouzelnik';
	$wizard = mysql_query($sqlQuery, $db);
	
	if(isset($_POST['addrech']))
	{
		$error = FALSE;
		$page = '<div class="text_input">
					<label for="grim">Grimoar:</label>
					<div class="choice_array">';
					while ($row = mysql_fetch_array($grim))
					{
						$page .='<div class="choice">';
						if ($_POST['grim'] == $row['nazev_grim'])
							$page .= '<input type="radio" name="grim" checked="checked" value="'.$row['nazev_grim'].'"><span>'. $row['nazev_grim'] .'</span><br></div>';
						else
							$page .= '<input type="radio" name="grim" value="'.$row['nazev_grim'].'"><span>'. $row['nazev_grim'] .'</span><br></div>';
					}
		$page .= '		</div>
				</div>
				<div class="text_input">
					<label for="wiz">Kouzelník:</label>
					<div class="choice_array">';
					while ($row = mysql_fetch_array($wizard))
					{
						$page .='<div class="choice">';
						if ($_POST['wiz'] == $row['jmeno_kouz'])
							$page .= '<input type="radio" name="wiz" checked="checked" value="'.$row['jmeno_kouz'].'"><span>'. $row['jmeno_kouz'] .'</span><br></div>';
						else
							$page .= '<input type="radio" name="wiz" value="'.$row['jmeno_kouz'].'"><span>'. $row['jmeno_kouz'] .'</span><br></div>';
					}
		$page .= '</div>
				</div>
				<div class="text_input">
					<label for="date">Datum:</label>
					<input id="datum_od" type="text" name="date" required value="'. $_POST['date'] .'">
				</div>
				<div class="text_input">
					<label for="place">Místo:</label>
					<div class="choice_array">';
				$place_err = FALSE;
				while ($row = mysql_fetch_array($place))
				{
					if (!empty($_POST['check_list']))
					{
						if(in_array($row['jmeno'], $_POST['check_list']))
						{
							$page .='	<div class="choice">
								<input type="checkbox" name="check_list[]" value="'.$row['jmeno'].'" checked="checked"><span>'. $row['jmeno'] .'</span><br></div>';
						}
						else
							$page .='	<div class="choice">
								<input type="checkbox" name="check_list[]" value="'.$row['jmeno'].'"><span>'. $row['jmeno'] .'</span><br></div>';
					}
					
					else
					{
						$error = TRUE;
						$place_err = TRUE;
						$page .='	<div class="choice">
							<input type="checkbox" name="check_list[]" value="'.$row['jmeno'].'"><span>'. $row['jmeno'] .'</span><br></div>';
					}
				}
				
				if ($place_err)
					$page .= '<span class="err-msg">Nabití musí být provedeno alespoň na jednom místě.</span>';
			
		$page .= 	'</div> </div>
					<input type="hidden" name="table" value="'. $_POST['table'] .'">
					<div class="button_choice">
						<a href="'. $_COOKIE['last_page'] .'" class="button">Zrušit</a>
						<input type="submit" value="Přidat" class="button" name="addrech">
					</div>';
					
		if (!$error)
		{
			// pridani udaju do databaze
			$sqlQuery = 'INSERT INTO Nabiti(datum, nazev_grim, jmeno_kouz) VALUES(\''. date_format(date_create_from_format('d.m. Y', $_POST['date']), 'Y-m-d') .'\', \''. $_POST['grim'] .'\', \''. $_POST['wiz'] .'\')';
			$result = mysql_query($sqlQuery, $db);

			// pridani mist, na kterych se provedlo nabiti
			if (!empty($_POST['check_list']))
			{
				// ziskani id aktualne pridaneho nabiti
				$sqlQuery = 'SELECT * FROM Nabiti ORDER BY id_nabiti DESC LIMIT 1';
				$id = mysql_query($sqlQuery, $db);
				$id = mysql_fetch_array($id);
				
				foreach($_POST['check_list'] as $place)
				{
					$sqlQuery = 'INSERT INTO Nab_ded_mista (Nabiti, misto) VALUES (\''. $id['id_nabiti'] .'\', \''. $place .'\')';
					$result = mysql_query($sqlQuery, $db);
				}
			}
			
			echo '	<p>Nabití bylo úspěšně přidáno do databáze!</p>
					<a href="'. $_COOKIE['last_page'] .'" class="button">Rozumím</a>';
		}
		else
			echo $page;
	}
	// vychozi zobrazeni
	else
	{
		echo'	<div class="text_input">
					<label for="grim">Grimoar:</label>
					<div class="choice_array">';
					while ($row = mysql_fetch_array($grim))
					{
						echo'<div class="choice">
								<input type="radio" name="grim" checked="checked" value="'.$row['nazev_grim'].'">
								<span>'. $row['nazev_grim'] .'</span><br>
							</div>';
					}
		echo '		</div>
				</div>
				<div class="text_input">
					<label for="wiz">Kouzelník:</label>
					<div class="choice_array">';
					while ($row = mysql_fetch_array($wizard))
					{
						echo'<div class="choice">
								<input type="radio" name="wiz" checked="checked" value="'.$row['jmeno_kouz'].'">
								<span>'. $row['jmeno_kouz'] .'</span><br>
							</div>';
					}
		echo '		</div>
				</div>
				<div class="text_input">
					<label for="date">Datum:</label>
					<input id="datum_od" type="text" name="date" required value="'. date_format(date_create_from_format('Y-m-d', date("Y-m-d")), 'd.m. Y') .'">
				</div>
				<div class="text_input">
					<label for="place">Místo:</label>
					<div class="choice_array">';
					while ($row = mysql_fetch_array($place))
						echo '	<div class="choice">
							<input type="checkbox" name="check_list[]" value="'.$row['jmeno'].'"><span>'. $row['jmeno'] .'</span><br></div>';
		echo'		</div>
				</div>
		
				<input type="hidden" name="table" value="'. $_GET['table'] .'">
				<div class="button_choice">
					<a href="'. $_COOKIE['last_page'] .'" class="button">Zrušit</a>
					<input type="submit" value="Přidat" class="button" name="addrech">
				</div>';
	}
?>