<?php
	// ziskani informaci
	$sqlQuery = 'SELECT * FROM Svitek WHERE nazev=\''. $_COOKIE['id'] . '\'';
	$scroll = mysql_query($sqlQuery, $db);
	$scroll = mysql_fetch_array($scroll);
	
	//ziskani vsech kouzel
	$sqlQuery = 'SELECT * FROM Kouzlo';
	$spell = mysql_query($sqlQuery, $db);
	
	//ziskani vsech kouzelniku
	$sqlQuery = 'SELECT * FROM Kouzelnik';
	$wizard = mysql_query($sqlQuery, $db);
	
	if(isset($_POST['chscroll']))
	{
		$error = FALSE;
		$page = '<div class="text_input">
					<label for="nazev">Název:</label>';
			
		// zpracovani jmena
		if(strlen($_POST['nazev']) >= 4 and strlen($_POST['nazev']) <= 100)
		{
			// zjisteni, zdali zadany svitek existuje
			$sqlQuery = 'SELECT nazev FROM Svitek WHERE nazev = \'' . $_POST['nazev'] . '\'';
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
					<span class="err-msg">Jméno svitku musí obsahovat alespoň 4 znaky a nesmí být delší než 100 znaků.</span>';
		}
		
		// zpracování kouzel
		$page.='<div class="text_input">
					<label for="spell">Kouzlo:</label>
					<div class="choice_array">';
					while ($row = mysql_fetch_array($spell))
					{
						// svitek může obsahovat pouze útočná kouzla
						if($row['typ'] == "útočné")
						{
							$page .='<div class="choice">';
							if ($_POST['spell'] == $row['nazev_kouz'])
								$page .= '<input type="radio" name="spell" checked="checked" value="'.$row['nazev_kouz'].'"><span>'. $row['nazev_kouz'] .'</span><br></div>';
							else
								$page .= '<input type="radio" name="spell" value="'.$row['nazev_kouz'].'"><span>'. $row['nazev_kouz'] .'</span><br></div>';
						}
					}
		// zpracování majitele
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
					</div>
					<input type="hidden" name="table" value="'. $_POST['table'] .'">
					<input type="hidden" name="id" value="'. $_POST['id'] .'">
					<div class="button_choice">
						<a href="'. $_COOKIE['last_page'] .'" class="button">Zrušit</a>
						<input type="submit" value="Upravit" class="button" name="chscroll">
					</div>';
					
		if (!$error)
		{
			// upraveni udaju v databazi
			$sqlQuery = 'UPDATE Svitek SET nazev=\''. $_POST['nazev'] .'\', nazev_kouz=\''. $_POST['spell'] .'\', jmeno_kouz=';
			if ($_POST['owner'] == "není")
				$sqlQuery .= "NULL";
			else
				$sqlQuery .= '\''. $_POST['owner'] .'\'';
			$sqlQuery .= ' WHERE nazev =\'' . $_COOKIE['id'] . '\'';
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
					<label for="nazev">Název:</label>
					<input type="text" name="nazev" value="'.$scroll['nazev'].'">
				</div>
				<div class="text_input">
					<label for="spell">Kouzlo:</label>
					<div class="choice_array">';
					while ($row = mysql_fetch_array($spell))
					{
						if($row['typ'] == "útočné")
						{
							echo'<div class="choice">';
							if ($scroll['nazev_kouz'] == $row['nazev_kouz'])
								echo '<input type="radio" name="spell" checked="checked" value="'.$row['nazev_kouz'].'"><span>'. $row['nazev_kouz'] .'</span><br></div>';
							else
								echo '<input type="radio" name="spell" value="'.$row['nazev_kouz'].'"><span>'. $row['nazev_kouz'] .'</span><br></div>';
						}
					}
		echo '		</div>
				</div>
				<div class="text_input">
					<label for="owner">Majitel:</label>
					<div class="choice_array">';
					echo '<div class="choice"><input type="radio" name="owner" value="není" '; 
					if ($scroll['jmeno_kouz'] == "")
						echo 'checked="checked"';
					
					echo '><span>není</span><br></div>';
					while ($row = mysql_fetch_array($wizard))
					{
						echo'<div class="choice">';
						if ($scroll['jmeno_kouz'] == $row['jmeno_kouz'])
							echo '<input type="radio" name="owner" checked="checked" value="'.$row['jmeno_kouz'].'"><span>'. $row['jmeno_kouz'] .'</span><br></div>';
						else
							echo '<input type="radio" name="owner" value="'.$row['jmeno_kouz'].'"><span>'. $row['jmeno_kouz'] .'</span><br></div>';
					}
		echo '		</div>
				</div>
				<input type="hidden" name="table" value="'. $_GET['table'] .'">
				<input type="hidden" name="id" value="'. $_COOKIE['id'] .'">
				<div class="button_choice">
					<a href="'. $_COOKIE['last_page'] .'" class="button">Zrušit</a>
					<input type="submit" value="Upravit" class="button" name="chscroll">
				</div>';
	}
?>