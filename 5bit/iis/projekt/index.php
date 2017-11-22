<?php
	ini_set("default_charset", "utf-8");
	session_start();
	
	include("header.php"); 
		
	// pokud je uzivatel jiz prihlasen, presmeruj jej na hlavni stranku
	if (isset($_SESSION["login"]))
	{
		header('Location: home.php');
	}
	
	// nastaveni cookies pro zobrazeni posledniho formulare
	$form_id = 0;
	if (isset($_COOKIE['disp_form']))
	{
		$form_id = $_COOKIE['disp_form'];
	}

	if ($form_id == 0)
		echo '<div id="login">';
	else
		echo '<div id="login" class="hide">';
?> 
	<img src="https://www.stud.fit.vutbr.cz/~xaubre02/pic/logo.png" alt="Logo">
	<form action="<?= $_SERVER['PHP_SELF']?>" method="post">
		<div class="form-part">
			<label for="username">Username:</label>
			
<?php
	//Prihlasovaci formular
	if(isset($_POST['log']))
	{
		$login = $_POST['username']; 
		$pwd = $_POST['password']; 
		
		if ($login == 'admin' && $pwd == 'root')
		{
			$_SESSION["login"] = $login;
			$_SESSION["time"] = time();
			header('Location: home.php');
		}
		// pripojeni k databazi a zjisteni, zdali dany kouzelnik existuje
		include("connect_to_db.php");
		$sqlQuery = 'SELECT heslo FROM Kouzelnik WHERE jmeno_kouz = \'' . $login . '\'';
		$result = mysql_query($sqlQuery, $db);
		mysql_close($db);
		
		if ($login == 'admin' or mysql_num_rows($result) != 0)
		{
			echo '<input type="text" name="username" value="' . $login . '">
			</div>';
			
			$heslo = mysql_fetch_array($result);
			$heslo = $heslo[0];
			
			// kontrola hesla
			if ($pwd == $heslo and $heslo != "")
			{
				$_SESSION["login"] = $login;
				$_SESSION["time"] = time();
				header('Location: home.php');
			}
			
			// nespravne heslo
			else
			{
				echo '<div class="form-part">
					<label for="password">Password:</label>
					<input type="password" name="password" class="bad-input">
				</div>
				<span class="err-msg">Nesprávné heslo.</span>';
			}
		}
		// neexistujici uzivatel
		else
		{
			echo '<input type="text" name="username" class="bad-input">
			</div>
			<span class="err-msg">Nesprávné nebo neexistující uživatelské jméno.</span>
			
			<div class="form-part">
				<label for="password">Password:</label>
				<input type="password" name="password">
			</div>';
		}
	}
	// vychozi zobrazeni stranky
	else
	{
		echo '<input type="text" name="username">
		</div>
		
		<div class="form-part">
			<label for="password">Password:</label>
			<input type="password" name="password">
		</div>';
	}
?>	
		<div class="confirm">
			<span class="button" onclick="display_form(0)">Registrovat</span>
			<input class="button" type="submit" name="log" value="Přihlásit">
		</div>
	</form>
</div>

<?php // Zobrazeni registracni formulare
	$form_id = 0;
	if (isset($_COOKIE['disp_form']))
	{
		$form_id = $_COOKIE['disp_form'];
	}

	if ($form_id == 1)
		echo '<div id="register">';
	else
		echo '<div id="register" class="hide">';
?>

	<img src="https://www.stud.fit.vutbr.cz/~xaubre02/pic/logo.png" alt="Logo">
	<form action="<?= $_SERVER['PHP_SELF']?>" method="post">

<?php
	// Registracni formular
	if(isset($_POST['reg']))
	{
		$page = '<div class="form-part">
			<label for="username">Uživatelské jméno:</label>';
			
		$error = FALSE;
		// zpracovani uzivatelskeho jmena
		if(strlen($_POST['username']) >= 4)
		{
			// pripojeni k databazi a zjisteni, zdali dany kouzelnik existuje
			include("connect_to_db.php");
			$sqlQuery = 'SELECT jmeno_kouz FROM Kouzelnik WHERE jmeno_kouz = \'' . $_POST['username'] . '\'';
			$result = mysql_query($sqlQuery, $db);
			mysql_close($db);
			
			if (mysql_num_rows($result) != 0)
			{
				$error = TRUE;
				$page .= '<input type="text" name="username" value="' .$_POST['username']. '" class="bad-input">
				</div>
				<span class="err-msg">Zadané uživatelské jméno již existuje.</span>';
			}
			else
			{
				$page .= '<input type="text" name="username" value="' .$_POST['username']. '">
				</div>';
			}
		}
		else
		{
			$error = TRUE;
			$page .= '<input type="text" name="username" class="bad-input">
			</div>
			<span class="err-msg">Uživatelské jméno musí obsahovat alespoň 4 znaky.</span>';
		}
		
		// zpracovani many - Mana musí být celé číslo v rozsahu 0 - 100 000
		$mana = $_POST['mana'];
		if (is_numeric($mana) and $mana >= 0 and $mana <= 100000)
		{
			$page .= '<div class="form-part">
				<label for="mana">Mana:</label>
				<input type="text" name="mana" value="' .$_POST['mana']. '">
			</div>';
		}
		else
		{
			$error = TRUE;
			$page .= '<div class="form-part">
				<label for="mana">Mana:</label>
				<input type="text" name="mana" value="' .$_POST['mana']. '" class="bad-input">
			</div>
			<span class="err-msg">Mana musí být celé číslo v rozsahu 0 - 100 000.</span>';
		}
		
		// zpracovani dosazene urovne kouzleni
		$level = $_POST['level'];
		if ($level != "SS" and $level != "S" and $level != "A" and $level != "B" and $level != "C" and $level != "D" and $level != "E")
		{
			$error = TRUE;
			$page .= '<div class="form-part">
				<label for="level">Úroveň:</label>
				<input type="text" name="level" value="' .$_POST['level']. '" class="bad-input">
			</div>
			<span class="err-msg">Dostupné dosažené úrovně v kouzlení: SS, S, A, B, C, D, E.</span>';
		}
		else
		{
			$page .= '<div class="form-part">
				<label for="level">Úroveň:</label>
				<input type="text" name="level" value="' .$_POST['level']. '">
			</div>';
		}
		// zpracovani hesla
		$pwd0 = $_POST['pwd0'];
		$pwd1 = $_POST['pwd1'];
		if (strlen($pwd0) >= 6)
		{
			$page .= '<div class="form-part">
				<label for="pwd0">Heslo:</label>
				<input type="password" name="pwd0" value="' .$pwd0. '">
			</div>';
		}
		else
		{
			$error = TRUE;
			$page .= '<div class="form-part">
				<label for="pwd0">Heslo:</label>
				<input type="password" name="pwd0" class="bad-input">
			</div>
			<span class="err-msg">Heslo musí obsahovat alespoň 6 znaků.</span>';
		}
		
		if ($pwd0 != $pwd1)
		{
			$error = TRUE;
			$page .= '<div class="form-part">
				<label for="pwd1">Zopakovat heslo:</label>
				<input type="password" name="pwd1" class="bad-input">
			</div>
			<span class="err-msg">Zadaná hesla se neshodují.</span>';
		}
		else
		{
			$page .= '<div class="form-part">
				<label for="pwd1">Zopakovat heslo:</label>
				<input type="password" name="pwd1" value="' .$pwd1. '">
			</div>';
		}
		
		if($error)
		{
			echo $page.'<div class="confirm">
							<span class="button" onclick="display_form(1)">Přihlásit</span>
							<input class="button" type="submit" name="reg" value="Registrovat">
						</div>';
		}
		else
		{
			echo '	<div id="reg-succ">
						<p>Registrace proběhla úspěšně!</p>
						<span class="button" onclick="display_form(2)">Rozumím</span>
					</div>';
		}
	}
	// vychozi zobrazeni registracniho formulare
	else
	{
		echo '<div class="form-part">
			<label for="username">Uživatelské jméno:</label>
			<input type="text" name="username">
		</div>
		
		<div class="form-part">
			<label for="mana">Mana:</label>
			<input type="text" name="mana">
		</div>
		
		<div class="form-part">
			<label for="level">Úroveň:</label>
			<input type="text" name="level">
		</div>
		
		<div class="form-part">
			<label for="pwd0">Heslo:</label>
			<input type="password" name="pwd0">
		</div>
		
		<div class="form-part">
			<label for="pwd1">Zopakovat heslo:</label>
			<input type="password" name="pwd1">
		</div>
		
		<div class="confirm">
			<span class="button" onclick="display_form(1)">Přihlásit</span>
			<input class="button" type="submit" name="reg" value="Registrovat">
		</div>';
	}
?>
	</form>
</div>

<?php // Zobrazeni zprav o odhlaseni nebo zamitnutem pristupu.
	$form_id = 0;
	if (isset($_COOKIE['disp_form']))
	{
		$form_id = $_COOKIE['disp_form'];
	}

	if ($form_id == 3 or $form_id == 4 or $form_id == 5)
	{	
		echo '<div id="logout_msg">
				<img src="https://www.stud.fit.vutbr.cz/~xaubre02/pic/logo.png" alt="Logo">
				<form>
					<div id="reg-succ">';
		if ($form_id == 3) // pristup zamitnut
			echo '			<p>Přístup zamítnut!<br>Musíte se příhlásit.</p>';
		
		else if ($form_id == 4) // automaticke odhlaseni
			echo '          <p>Byli jste odhlášeni z důvodu neaktivity.</p>';
		
		else			// odhlaseni uzivatelem
			echo '          <p>Odhlášení proběhlo úspěšně.</p>';
		
		echo 			'<span class="button" onclick="display_form(3)">Rozumím</span>
					</div>
				</form>';
	}
	else
		echo '<div id="register" class="hide">';
?>
</div>

</div>	
</body>
</html>