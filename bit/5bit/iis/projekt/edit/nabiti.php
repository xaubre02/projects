<?php
	// ziskani informaci
	$sqlQuery = 'SELECT * FROM Nabiti WHERE id_nabiti=\''. $_COOKIE['id'] . '\'';
	$recharge = mysql_query($sqlQuery, $db);
	$recharge = mysql_fetch_array($recharge);
	
	//ziskani vsech mist
	$sqlQuery = 'SELECT * FROM Ded_misto';
	$place = mysql_query($sqlQuery, $db);
	
	//ziskani vsech grimoaru
	$sqlQuery = 'SELECT * FROM Grimoar';
	$grim = mysql_query($sqlQuery, $db);
	
	//ziskani vsech kouzelniku
	$sqlQuery = 'SELECT * FROM Kouzelnik';
	$wizard = mysql_query($sqlQuery, $db);
	
	//ziskani vsech mist, ktera jsou obsazena v nabiti
	$sqlQuery = 'SELECT * FROM Nab_ded_mista WHERE Nabiti="'. $_COOKIE['id'] .'"';
	$contain = mysql_query($sqlQuery, $db);
	
	if(isset($_POST['chrech']))
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
						<input type="submit" value="Upravit" class="button" name="chrech">
					</div>';
					
		if (!$error)
		{
			// odstraneni dosavadnich mist, ktera nabiti obsahuje
			$sqlQuery = 'DELETE FROM Nab_ded_mista WHERE Nabiti=\''.$_COOKIE['id'].'\'';
			$result = mysql_query($sqlQuery, $db);

			// upraveni udaju v databazi
			$sqlQuery = 'UPDATE Nabiti SET datum=\''. date_format(date_create_from_format('d.m. Y', $_POST['date']), 'Y-m-d') .'\', nazev_grim=\''. $_POST['grim'] .'\', jmeno_kouz=\''. $_POST['wiz'] .'\' WHERE id_nabiti=\'' . $_COOKIE['id'] . '\'';
			$result = mysql_query($sqlQuery, $db);

			// pridani mist, na kterych se provedlo nabiti
			if (!empty($_POST['check_list']))
			{
				foreach($_POST['check_list'] as $place)
				{
					$sqlQuery = 'INSERT INTO Nab_ded_mista (Nabiti, misto) VALUES (\''. $_COOKIE['id'] .'\', \''. $place .'\')';
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
					<label for="grim">Grimoar:</label>
					<div class="choice_array">';
					while ($row = mysql_fetch_array($grim))
					{
						echo'<div class="choice">';
						if ($recharge['nazev_grim'] == $row['nazev_grim'])
							echo '<input type="radio" name="grim" checked="checked" value="'.$row['nazev_grim'].'"><span>'. $row['nazev_grim'] .'</span><br></div>';
						else
							echo '<input type="radio" name="grim" value="'.$row['nazev_grim'].'"><span>'. $row['nazev_grim'] .'</span><br></div>';
					}
		echo '		</div>
				</div>
				<div class="text_input">
					<label for="wiz">Kouzelník:</label>
					<div class="choice_array">';
					while ($row = mysql_fetch_array($wizard))
					{
						echo'<div class="choice">';
						if ($recharge['jmeno_kouz'] == $row['jmeno_kouz'])
							echo '<input type="radio" name="wiz" checked="checked" value="'.$row['jmeno_kouz'].'"><span>'. $row['jmeno_kouz'] .'</span><br></div>';
						else
							echo '<input type="radio" name="wiz" value="'.$row['jmeno_kouz'].'"><span>'. $row['jmeno_kouz'] .'</span><br></div>';
					}
		echo '		</div>
				</div>
				<div class="text_input">
					<label for="date">Datum:</label>
					<input id="datum_od" type="text" name="date" required value="'. date_format(date_create_from_format('Y-m-d', $recharge['datum']), 'd.m. Y') .'">
				</div>
				<div class="text_input">
					<label for="place">Místo:</label>
					<div class="choice_array">';
					$mista_dob = array();
					while ($misto = mysql_fetch_array($contain))
					{
						$mista_dob[] = $misto;
					}
					
					while ($row = mysql_fetch_array($place))
					{
						$contains = FALSE;
						foreach ($mista_dob as $misto)
						{
							if ($row['jmeno'] == $misto['misto'])
							{
								$contains = TRUE;
								echo '	<div class="choice">
									<input type="checkbox" name="check_list[]" value="'.$row['jmeno'].'" checked="checked"><span>'. $row['jmeno'] .'</span><br></div>';
								break;
							}
						}
						if (!$contains)
							echo '	<div class="choice">
								<input type="checkbox" name="check_list[]" value="'.$row['jmeno'].'"><span>'. $row['jmeno'] .'</span><br></div>';
					}
		echo'		</div>
				</div>
		
				<input type="hidden" name="table" value="'. $_GET['table'] .'">
				<div class="button_choice">
					<a href="'. $_COOKIE['last_page'] .'" class="button">Zrušit</a>
					<input type="submit" value="Upravit" class="button" name="chrech">
				</div>';
	}
?>