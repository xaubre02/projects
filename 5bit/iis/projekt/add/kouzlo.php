<?php
	//ziskani vsech elementu
	$sqlQuery = 'SELECT * FROM Element';
	$elem = mysql_query($sqlQuery, $db);
	$elem2 = mysql_query($sqlQuery, $db);
	
	if(isset($_POST['addspell']))
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
				<div class="button_choice">
					<a href="'. $_COOKIE['last_page'] .'" class="button">Zrušit</a>
					<input type="submit" value="Přidat" class="button" name="addspell">
				</div>';
					
		if (!$error)
		{
			// upraveni udaju v databazi
			$sqlQuery = 'INSERT INTO Kouzlo(nazev_kouz, typ, obtiznost, sila, druh) VALUES(\''. $_POST['nazev'] .'\', \''. $_POST['typ'] .'\', \''. $_POST['diff'] .'\', \''. $_POST['power'] .'\', \''. $_POST['elem'] .'\')';
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
					<input type="text" name="nazev" value="'.$_GET['nazev_kouz'].'">
				</div>
				<div class="text_input">
					<label for="diff">Obtížnost:</label>
					<div class="choice_array">
						<div class="choice">
							<input type="radio" name="diff" value="snadná" checked="checked"><span>snadná</span><br>
						</div>
						<div class="choice">
							<input type="radio" name="diff" value="střední"><span>střední</span><br>
						</div>
						<div class="choice">
							<input type="radio" name="diff" value="těžká"><span>těžká</span><br>
						</div>
					</div>
				</div>
				<div class="text_input">
					<label for="power">Síla:</label>
					<div class="choice_array">
						<div class="choice">
							<input type="radio" name="power" value="velmi slabé" checked="checked"><span>velmi slabé</span><br>
						</div>
						<div class="choice">
							<input type="radio" name="power" value="slabé"><span>slabé</span><br>
						</div>
						<div class="choice">
							<input type="radio" name="power" value="silné"><span>silné</span><br>
						</div>
						<div class="choice">
							<input type="radio" name="power" value="velmi silné"><span>velmi silné</span><br>
						</div>
					</div>
				</div>
				<div class="text_input">
					<label for="typ">Typ:</label>
					<div class="choice_array">
						<div class="choice">
							<input type="radio" name="typ" value="obranné" checked="checked"><span>obranné</span><br>
						</div>
						<div class="choice">
							<input type="radio" name="typ" value="útočné"><span>útočné</span><br>
						</div>
					</div>
				</div>
				<div class="text_input">
					<label for="elem">Hlavní element:</label>
					<div class="choice_array">';
						while ($row = mysql_fetch_array($elem))
						{
							echo'<div class="choice">
									<input type="radio" name="elem" checked="checked" value="'.$row['druh'].'">
									<span>'. $row['druh'] .'</span><br>
								</div>';
						}
		echo '		</div>
				</div>
				<div class="text_input">
					<label for="check_list">Vedlejší elementy:</label>
					<div class="choice_array">';
						while ($row = mysql_fetch_array($elem2))
							echo '	<div class="choice">
										<input type="checkbox" name="check_list[]" value="'.$row['druh'].'"><span>'. $row['druh'] .'</span><br>
									</div>';

		echo'		</div>
				</div>
				<input type="hidden" name="table" value="'. $_GET['table'] .'">
				<div class="button_choice">
					<a href="'. $_COOKIE['last_page'] .'" class="button">Zrušit</a>
					<input type="submit" value="Přidat" class="button" name="addspell">
				</div>';
	}
?>