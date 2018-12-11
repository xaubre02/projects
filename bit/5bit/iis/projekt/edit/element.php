<?php
	// ziskani informaci
	$sqlQuery = 'SELECT * FROM Element WHERE druh=\''. $_COOKIE['id'] . '\'';
	$elem = mysql_query($sqlQuery, $db);
	$elem = mysql_fetch_array($elem);
	
	if(isset($_POST['chelem']))
	{
		$error = FALSE;
		$page = '<div class="text_input">
					<label for="druh">Název:</label>';
			
		// zpracovani jmena
		if(strlen($_POST['druh']) >= 4 and strlen($_POST['druh']) <= 100)
		{
			// zjisteni, zdali zadany element existuje
			$sqlQuery = 'SELECT druh FROM Element WHERE druh = \'' . $_POST['druh'] . '\'';
			$result = mysql_query($sqlQuery, $db);
			
			if (mysql_num_rows($result) != 0 and $_POST['druh'] != $_COOKIE['id'])
			{
				$error = TRUE;
				$page .= '	<input type="text" name="druh" value="'.$_POST['druh'].'" class="bad-input">
						</div>
						<span class="err-msg">Zadané jméno již existuje!</span>';
			}
			else
			{
				$page .= '	<input type="text" name="druh" value="'.$_POST['druh'].'">
						</div>';
			}
		}
		else
		{
			$error = TRUE;
			$page .= '	<input type="text" name="druh" value="'.$_POST['druh'].'" class="bad-input">
					</div>
					<span class="err-msg">Jméno elementu musí obsahovat alespoň 4 znaky a nesmí být delší než 100 znaků.</span>';
		}
		
		// zpracovani specializace
		$page .= '	<div class="text_input">
						<label for="specializace">Specializace:</label>
						<div class="choice_array">
							<div class="choice">
								<input type="radio" name="specializace" value="obrana"'; 
							if ($_POST['specializace'] == "obrana")
								$page .= 'checked="checked"';
							
							$page .= '><span>obrana</span><br>
							</div>
							<div class="choice">
								<input type="radio" name="specializace" value="útok"'; 
							if ($_POST['specializace'] == "útok")
								$page .= 'checked="checked"';
							
							$page .= '><span>útok</span><br>
							</div>
							<div class="choice">
								<input type="radio" name="specializace" value="podpora"'; 
							if ($_POST['specializace'] == "podpora")
								$page .= 'checked="checked"';
							
							$page .= '><span>podpora</span><br>
							</div>
						</div>
					</div>';
			
		// zpracovani barvy
		$page .= '<div class="text_input">
					<label for="barva">Barva:</label>';
		if(strlen($_POST['barva']) >= 3 and strlen($_POST['barva']) <= 100)
			$page .= '	<input type="text" name="barva" value="'.$_POST['barva'].'">
					</div>';
		else
		{
			$error = TRUE;
			$page .= '	<input type="text" name="barva" value="'.$_POST['barva'].'" class="bad-input">
					</div>
					<span class="err-msg">Barva elementu musí obsahovat alespoň 3 znaky a nesmí být delší než 100 znaků.</span>';
		}
		
		$page .= '	<input type="hidden" name="table" value="'. $_POST['table'] .'">
					<input type="hidden" name="id" value="'. $_COOKIE['id'] .'">
					<input type="submit" value="Upravit" class="button" name="chelem">';
					
		if (!$error)
		{
			// upraveni udaju v databazi
			$sqlQuery = 'UPDATE Element SET druh=\''. $_POST['druh'] .'\', specializace=\''. $_POST['specializace'] .'\', barva=\''. $_POST['barva'] .'\' WHERE druh =\'' . $_COOKIE['id'] . '\'';
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
					<label for="druh">Název:</label>
					<input type="text" name="druh" value="'.$elem['druh'].'">
				</div>
				<div class="text_input">
					<label for="specializace">Specializace:</label>
					<div class="choice_array">
						<div class="choice">
							<input type="radio" name="specializace" value="obrana"';
						if ($elem['specializace'] == "obrana")
							echo 'checked="checked"';
						
						echo '><span>obrana</span><br>
						</div>
						<div class="choice">
							<input type="radio" name="specializace" value="útok"';
						if ($elem['specializace'] == "útok")
							echo 'checked="checked"';
						
						echo '><span>útok</span><br>
						</div>
						<div class="choice">
							<input type="radio" name="specializace" value="podpora"';
						if ($elem['specializace'] == "podpora")
							echo 'checked="checked"';
						
						echo '><span>podpora</span><br>
						</div>
					</div>
				</div>
				<div class="text_input">
					<label for="barva">Barva:</label>
					<input type="text" name="barva" value="'.$elem['barva'].'">
				</div>
				<input type="hidden" name="table" value="'. $_GET['table'] .'">
				<input type="hidden" name="id" value="'. $_COOKIE['id'] .'">
				<div class="button_choice">
					<a href="'. $_COOKIE['last_page'] .'" class="button">Zrušit</a>
					<input type="submit" value="Upravit" class="button" name="chelem">
				</div>';
	}
?>