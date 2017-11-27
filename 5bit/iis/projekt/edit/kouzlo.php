<?php
	// ziskani informaci
	$sqlQuery = 'SELECT * FROM Kouzlo WHERE nazev_kouz=\''. $_COOKIE['id'] .'\'';
	$spell = mysql_query($sqlQuery, $db);
	$spell = mysql_fetch_array($spell);

	//ziskani vsech elementu
	$sqlQuery = 'SELECT * FROM Element';
	$elem = mysql_query($sqlQuery, $db);
	$elem2 = mysql_query($sqlQuery, $db);
	
	//ziskani vsech vedlejsich elementu daneho kouzla
	$sqlQuery = 'SELECT * FROM Ma_ved_elem WHERE nazev_kouz=\''. $_COOKIE['id'] .'\'';
	$sec = mysql_query($sqlQuery, $db);
	
	if(isset($_POST['chspell']))
	{
		$error = FALSE;
		$page = '<div class="text_input">
					<label for="nazev">Název:</label>';
			
		// zpracovani jmena
		if(strlen($_POST['nazev']) >= 4 and strlen($_POST['nazev']) <= 100)
		{
			// zjisteni, zdali zadane kouzlo existuje
			$sqlQuery = 'SELECT nazev_kouz FROM Kouzlo WHERE nazev_kouz= \'' . $_POST['nazev'] . '\'';
			$result = mysql_query($sqlQuery, $db);
			
			if (mysql_num_rows($result) != 0 and $_POST['nazev'] != $_COOKIE['id'])
			{
				$error = TRUE;
				$page .= '	<input type="text" name="nazev" value="'.$_POST['nazev'].'" class="bad-input">
						</div>
						<span class="err-msg">Zadaný název již existuje!</span>';
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
					<span class="err-msg">Název kouzla musí obsahovat alespoň 4 znaky a nesmí být delší než 100 znaků.</span>';
		}
		
		// zpracování obtiznosti
		$page.='<div class="text_input">
					<label for="diff">Obtížnost:</label>
					<div class="choice_array">
						<div class="choice">
							<input type="radio" name="diff" value="snadná"';
						if ($_POST['diff'] == "snadná")
							$page.= 'checked="checked"';
						
						$page.= '><span>snadná</span><br>
						</div>
						<div class="choice">
							<input type="radio" name="diff" value="střední"';
						if ($_POST['diff'] == "střední")
							$page.= 'checked="checked"';
						
						$page.= '><span>střední</span><br>
						</div>
						<div class="choice">
							<input type="radio" name="diff" value="těžká"';
						if ($_POST['diff'] == "těžká")
							$page.= 'checked="checked"';
						
						$page.= '><span>těžká</span><br>
						</div>
					</div>
				</div>
				<div class="text_input">
					<label for="power">Síla:</label>
					<div class="choice_array">
						<div class="choice">
							<input type="radio" name="power" value="velmi slabé"';
						if ($_POST['power'] == "velmi slabé")
							$page.= 'checked="checked"';
						
						$page.= '><span>velmi slabé</span><br>
						</div>
						<div class="choice">
							<input type="radio" name="power" value="slabé"';
						if ($_POST['power'] == "slabé")
							$page.= 'checked="checked"';
						
						$page.= '><span>slabé</span><br>
						</div>
						<div class="choice">
							<input type="radio" name="power" value="silné"';
						if ($_POST['power'] == "silné")
							$page.= 'checked="checked"';
						
						$page.= '><span>silné</span><br>
						</div>
						<div class="choice">
							<input type="radio" name="power" value="velmi silné"';
						if ($_POST['power'] == "velmi silné")
							$page.= 'checked="checked"';
						
						$page.= '><span>velmi silné</span><br>
						</div>
					</div>
				</div>
				<div class="text_input">
					<label for="typ">Typ:</label>
					<div class="choice_array">
						<div class="choice">
							<input type="radio" name="typ" value="obranné"';
						if ($_POST['typ'] == "obranné")
							$page.= 'checked="checked"';
						
						$page.= '><span>obranné</span><br>
						</div>
						<div class="choice">
							<input type="radio" name="typ" value="útočné"';
						if ($_POST['typ'] == "útočné")
							$page.= 'checked="checked"';
						
						$page.= '><span>útočné</span><br>
						</div>
					</div>
				</div>
				<div class="text_input">
					<label for="elem">Hlavní element:</label>
					<div class="choice_array">';
						while ($row = mysql_fetch_array($elem))
						{
							$page.='<div class="choice">';
							if ($_POST['elem'] == $row['druh'])
								$page.= '<input type="radio" name="elem" checked="checked" value="'.$row['druh'].'"><span>'. $row['druh'] .'</span><br></div>';
							else
								$page.= '<input type="radio" name="elem" value="'.$row['druh'].'"><span>'. $row['druh'] .'</span><br></div>';
						}
		$typ = $_POST['typ'];
		$main_elem = $_POST['elem'];
		$sec_elem_err = FALSE;
		$main_elem_err = FALSE;
		$page.= '		</div>
				</div>
				<div class="text_input">
					<label for="check_list">Vedlejší elementy:</label>
					<div class="choice_array">';
						while ($row = mysql_fetch_array($elem2))
						{
							$contains = FALSE;
							if (!empty($_POST['check_list']))
							{
								foreach ($_POST['check_list'] as $el)
								{
									if ($row['druh'] == $el)
									{
										if ($typ == "obranné")
										{
											if($main_elem != $el)
											{
												$contains = TRUE;
												$page.= '	<div class="choice">
													<input type="checkbox" name="check_list[]" value="'.$row['druh'].'" checked="checked"><span>'. $row['druh'] .'</span><br></div>';
												break;
											}
											else
											{
												$error = TRUE;
												$main_elem_err = TRUE;
											}
										}
										else
										{	
											$error = TRUE;
											$sec_elem_err = TRUE;
											break;
										}
									}
								}
							}
							if (!$contains)
								$page.= '	<div class="choice">
											<input type="checkbox" name="check_list[]" value="'.$row['druh'].'"><span>'. $row['druh'] .'</span><br>
										</div>';
						}
		$page.='	</div>
				</div>';
			
		if ($main_elem_err)
			$page .= '<span class="err-msg">Tento element je hlavním elementem kouzla.</span>';
		
		else if ($sec_elem_err)
			$page .= '<span class="err-msg">Pouze obranná kouzla mohou mít vedlejší elementy.</span>';
		
		$page.='<input type="hidden" name="table" value="'. $_POST['table'] .'">
				<input type="hidden" name="id" value="'. $_POST['id'] .'">
				<div class="button_choice">
					<a href="'. $_COOKIE['last_page'] .'" class="button">Zrušit</a>
					<input type="submit" value="Upravit" class="button" name="chspell">
				</div>';
					
		if (!$error)
		{
			// odstraneni dosavadnich vedlejsich elementu
			$sqlQuery = 'DELETE FROM Ma_ved_elem WHERE nazev_kouz=\''.$_COOKIE['id'].'\'';
			$result = mysql_query($sqlQuery, $db);
			
			// upraveni udaju v databazi
			$sqlQuery = 'UPDATE Kouzlo SET nazev_kouz=\''. $_POST['nazev'] .'\', typ=\''. $_POST['typ'] .'\', obtiznost=\''. $_POST['diff'] .'\', sila=\''. $_POST['power'] .'\', druh=\''. $_POST['elem'] .'\' WHERE nazev_kouz =\'' . $_COOKIE['id'] . '\'';
			$result = mysql_query($sqlQuery, $db);
			
			// pridani novych vedlejsich elementu
			if (!empty($_POST['check_list']))
			{
				foreach($_POST['check_list'] as $new_elem)
				{
					$sqlQuery = 'INSERT INTO Ma_ved_elem (nazev_kouz, druh) VALUES (\''. $_POST['nazev'] .'\', \''. $new_elem .'\')';
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
					<input type="text" name="nazev" value="'.$spell['nazev_kouz'].'">
				</div>
				<div class="text_input">
					<label for="diff">Obtížnost:</label>
					<div class="choice_array">
						<div class="choice">
							<input type="radio" name="diff" value="snadná"';
						if ($spell['obtiznost'] == "snadná")
							echo 'checked="checked"';
						
						echo '><span>snadná</span><br>
						</div>
						<div class="choice">
							<input type="radio" name="diff" value="střední"';
						if ($spell['obtiznost'] == "střední")
							echo 'checked="checked"';
						
						echo '><span>střední</span><br>
						</div>
						<div class="choice">
							<input type="radio" name="diff" value="těžká"';
						if ($spell['obtiznost'] == "těžká")
							echo 'checked="checked"';
						
						echo '><span>těžká</span><br>
						</div>
					</div>
				</div>
				<div class="text_input">
					<label for="power">Síla:</label>
					<div class="choice_array">
						<div class="choice">
							<input type="radio" name="power" value="velmi slabé"';
						if ($spell['sila'] == "velmi slabé")
							echo 'checked="checked"';
						
						echo '><span>velmi slabé</span><br>
						</div>
						<div class="choice">
							<input type="radio" name="power" value="slabé"';
						if ($spell['sila'] == "slabé")
							echo 'checked="checked"';
						
						echo '><span>slabé</span><br>
						</div>
						<div class="choice">
							<input type="radio" name="power" value="silné"';
						if ($spell['sila'] == "silné")
							echo 'checked="checked"';
						
						echo '><span>silné</span><br>
						</div>
						<div class="choice">
							<input type="radio" name="power" value="velmi silné"';
						if ($spell['sila'] == "velmi silné")
							echo 'checked="checked"';
						
						echo '><span>velmi silné</span><br>
						</div>
					</div>
				</div>
				<div class="text_input">
					<label for="typ">Typ:</label>
					<div class="choice_array">
						<div class="choice">
							<input type="radio" name="typ" value="obranné"';
						if ($spell['typ'] == "obranné")
							echo 'checked="checked"';
						
						echo '><span>obranné</span><br>
						</div>
						<div class="choice">
							<input type="radio" name="typ" value="útočné"';
						if ($spell['typ'] == "útočné")
							echo 'checked="checked"';
						
						echo '><span>útočné</span><br>
						</div>
					</div>
				</div>
				<div class="text_input">
					<label for="elem">Hlavní element:</label>
					<div class="choice_array">';
						while ($row = mysql_fetch_array($elem))
						{
							echo'<div class="choice">';
							if ($spell['druh'] == $row['druh'])
								echo '<input type="radio" name="elem" checked="checked" value="'.$row['druh'].'"><span>'. $row['druh'] .'</span><br></div>';
							else
								echo '<input type="radio" name="elem" value="'.$row['druh'].'"><span>'. $row['druh'] .'</span><br></div>';
						}
		echo '		</div>
				</div>
				<div class="text_input">
					<label for="check_list">Vedlejší elementy:</label>
					<div class="choice_array">';
						$ved_elem = array();
						while ($row = mysql_fetch_array($sec))
						{
							$ved_elem[] = $row;
						}
						
						while ($row = mysql_fetch_array($elem2))
						{
							$contains = FALSE;
							foreach ($ved_elem as $el)
							{
								if ($row['druh'] == $el['druh'])
								{
									$contains = TRUE;
									echo '	<div class="choice">
										<input type="checkbox" name="check_list[]" value="'.$row['druh'].'" checked="checked"><span>'. $row['druh'] .'</span><br></div>';
									break;
								}
							}
							if (!$contains)
								echo '	<div class="choice">
											<input type="checkbox" name="check_list[]" value="'.$row['druh'].'"><span>'. $row['druh'] .'</span><br>
										</div>';
						}
		echo'		</div>
				</div>
				<input type="hidden" name="table" value="'. $_GET['table'] .'">
				<input type="hidden" name="id" value="'. $_COOKIE['id'] .'">
				<div class="button_choice">
					<a href="'. $_COOKIE['last_page'] .'" class="button">Zrušit</a>
					<input type="submit" value="Upravit" class="button" name="chspell">
				</div>';
	}
?>