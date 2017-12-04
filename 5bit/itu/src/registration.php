<?php include("header.php");?>

<body>

<?php include("side_menu.php");?>

<?php include("header_bar.php");?>

		<form class="form-basic" method="post">
			<div class="col-sm-12 well">
				<div class="form-title-row">
					<h1>Registrační formulář</h1>
				</div>
				<div class="row">
					<div class="col-sm-12">

<?php

	// First attempt to add employee form.
	if (!isset($_POST["name"]) or !isset($_POST["surname"]) or !isset($_POST["email"]) or !isset($_POST["phone"]) or !isset($_POST["password1"])
		or !isset($_POST["password2"]) or !isset($_POST["street"]) or !isset($_POST["city"]) or !isset($_POST["zip"])) {
		echo '<div class="row">
				<div class="col-sm-6 form-group">
					<label>Jméno <span style="color:red;">*</span></label>
					<input name="name" type="text" placeholder="Vaše jméno" class="form-control" />
				</div>
				<div class="col-sm-6 form-group">
					<label>Příjmení <span style="color:red;">*</span></label>
					<input name="surname" type="text" placeholder="Vaše příjmení" class="form-control"/>
				</div>
			</div>
			<div class="row">
				<div class="col-sm-6 form-group">
					<label>Email <span style="color:red;">*</span></label>
					<input name="email" type="email" placeholder="Váš email" class="form-control"/>
				</div>
				<div class="col-sm-6 form-group">
					<label>Telefon <span style="color:red;">*</span></label>
					<input name="phone" type="text" maxlength="9" placeholder="Váš telefon (777888999)" class="form-control"/>
				</div>
			</div>
			<div class="row">
				<div class="col-sm-6 form-group">
					<label>Heslo <span style="color:red;">*</span></label>
					<input name="password1" type="password" placeholder="Váše heslo" class="form-control"/>
				</div>
				<div class="col-sm-6 form-group">
					<label>Heslo ještě jednou <span style="color:red;">*</span></label>
					<input name="password2" type="password" placeholder="Váše heslo znova" class="form-control"/>
				</div>
			</div>
			<div class="row">
				<div class="col-sm-4 form-group">
					<label>Ulice</label>
					<input name="street" type="text" placeholder="Ulice (nepovinné)" class="form-control"/>
				</div>
				<div class="col-sm-4 form-group">
					<label>Město</label>
					<input name="city" type="text" placeholder="Město (nepovinné)" class="form-control"/>
				</div>
				<div class="col-sm-4 form-group">
					<label>PSČ</label>
					<input name="zip" type="text" maxlength="5" placeholder="PSČ (nepovinné)" class="form-control"/>
				</div>
			</div>';
	}
	// Information from registration form is processed.
	else {
		include 'db_connect.php';

		$form = '<div class="row">';
		$error = false;

		// Process name
		if (empty($_POST["name"])) {
			$error = true;
			$form .= '<div class="col-sm-6 form-group has-error has-feedback">
					<label>Jméno <span style="color:red;">*</span></label>
					<input name="name" type="text" placeholder="Vaše jméno" class="form-control" value="' . $_POST["name"] . '"/>
				</div>';
		}
		else {
			$form .= '<div class="col-sm-6 form-group">
					<label>Jméno <span style="color:red;">*</span></label>
					<input name="name" type="text" placeholder="Vaše jméno" class="form-control" value="' . $_POST["name"] . '"/>
				</div>';
		}

		// Process surname
		if (empty($_POST["surname"])) {
			$error = true;
			$form .= '<div class="col-sm-6 form-group has-error has-feedback">
					<label>Příjmení <span style="color:red;">*</span></label>
					<input name="surname" type="text" placeholder="Vaše příjmení" class="form-control" value="' . $_POST["surname"] . '"/>
				</div>';
		}
		else {
			$form .= '<div class="col-sm-6 form-group">
					<label>Příjmení <span style="color:red;">*</span></label>
					<input name="surname" type="text" placeholder="Vaše příjmení" class="form-control" value="' . $_POST["surname"] . '"/>
				</div>';
		}

		$form .= '</div>
				<div class="row">';

		// Process email
		if (empty($_POST["email"])) {
			$error = true;
			$form .= '<div class="col-sm-6 form-group has-error has-feedback">
					<label>Email <span style="color:red;">*</span></label>
					<input name="email" type="email" placeholder="Váš email" class="form-control" value="' . $_POST["email"] . '"/>
				</div>';
		}
		else {
			$sqlQuery = 'SELECT email FROM Uzivatel WHERE email=\'' . $_POST["email"] . '\'';
			$result = mysql_query($sqlQuery, $db);

			if (mysql_num_rows($result) != 0) {
				$error = true;

				echo '<div class="alert alert-danger alert-dismissable fade in">
						<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						Uživatel se zadaným emailem již existuje.
					</div>';

				$form .= '<div class="col-sm-6 form-group has-error has-feedback">
						<label>Email <span style="color:red;">*</span></label>
						<input name="email" type="email" placeholder="Váš email" class="form-control" value="' . $_POST["email"] . '"/>
					</div>';
			}
			else {
				$form .= '<div class="col-sm-6 form-group">
						<label>Email <span style="color:red;">*</span></label>
						<input name="email" type="email" placeholder="Váš email" class="form-control" value="' . $_POST["email"] . '"/>
					</div>';
			}
		}

		// Process phone number
		if (empty($_POST["phone"]) or (strlen($_POST["phone"]) != 9)) {
			$error = true;
			$form .= '<div class="col-sm-6 form-group has-error has-feedback">
					<label>Telefon <span style="color:red;">*</span></label>
					<input name="phone" type="text" maxlength="9" placeholder="Váš telefon (777888999)" class="form-control" value="' . $_POST["phone"] . '"/>
				</div>';
		}
		else {
			if (!is_numeric($_POST["phone"]) or (strpos($_POST["phone"], '.') !== false)) {
				$error = true;

				echo '<div class="alert alert-danger alert-dismissable fade in">
						<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						Telefoní číslo není ve správném formátu.
					</div>';

				$form .= '<div class="col-sm-6 form-group has-error has-feedback">
						<label>Telefon <span style="color:red;">*</span></label>
						<input name="phone" type="text" maxlength="9" placeholder="Váš telefon (777888999)" class="form-control" value="' . $_POST["phone"] . '"/>
					</div>';
			}
			else {
				$form .= '<div class="col-sm-6 form-group">
						<label>Telefon <span style="color:red;">*</span></label>
						<input name="phone" type="text" maxlength="9" placeholder="Váš telefon (777888999)" class="form-control" value="' . $_POST["phone"] . '"/>
					</div>';
			}
		}

		$form .= '</div>
				<div class="row">';

		// Process password
		if (empty($_POST["password1"]) or empty($_POST["password2"])) {
			$error = true;
			$form .= '<div class="col-sm-6 form-group has-error has-feedback">
					<label>Heslo <span style="color:red;">*</span></label>
					<input name="password1" type="password" placeholder="Váše heslo" class="form-control"/>
				</div>
				<div class="col-sm-6 form-group has-error has-feedback">
					<label>Heslo ještě jednou <span style="color:red;">*</span></label>
					<input name="password2" type="password" placeholder="Váše heslo znova" class="form-control"/>
				</div>';
		}
		else {
			if ($_POST["password1"] != $_POST["password2"]) { // passwords are not same
				$error = true;

				echo '<div class="alert alert-danger alert-dismissable fade in">
						<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						Hesla se neshodují.
					</div>';

				$form .= '<div class="col-sm-6 form-group has-error has-feedback">
						<label>Heslo <span style="color:red;">*</span></label>
						<input name="password1" type="password" placeholder="Váše heslo" class="form-control"/>
					</div>
					<div class="col-sm-6 form-group has-error has-feedback">
						<label>Heslo ještě jednou <span style="color:red;">*</span></label>
						<input name="password2" type="password" placeholder="Váše heslo znova" class="form-control"/>
					</div>';
			}
			else {
				$form .= '<div class="col-sm-6 form-group">
						<label>Heslo <span style="color:red;">*</span></label>
						<input name="password1" type="password" placeholder="Váše heslo" class="form-control"/>
					</div>
					<div class="col-sm-6 form-group">
						<label>Heslo ještě jednou <span style="color:red;">*</span></label>
						<input name="password2" type="password" placeholder="Váše heslo znova" class="form-control"/>
					</div>';
			}
		}

		$form .= '</div>
				<div class="row">
					<div class="col-sm-4 form-group">
						<label>Ulice</label>
						<input name="street" type="text" placeholder="Ulice (nepovinné)" class="form-control" value="' . $_POST["street"] . '"/>
					</div>
					<div class="col-sm-4 form-group">
						<label>Město</label>
						<input name="city" type="text" placeholder="Město (nepovinné)" class="form-control" value="' . $_POST["city"] . '"/>
					</div>';

		if (empty($_POST["zip"])) {
			$form .= '<div class="col-sm-4 form-group">
						<label>PSČ</label>
						<input name="zip" type="text" maxlength="5" placeholder="PSČ (nepovinné)" class="form-control" value="' . $_POST["zip"] . '"/>
					</div>
				</div>';
		}
		else {
			if ((strlen($_POST["zip"]) != 5) or !is_numeric($_POST["zip"]) or (strpos($_POST["zip"], '.') !== false)) {
				$error = true;

				echo '<div class="alert alert-danger alert-dismissable fade in">
						<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						PSČ je ve špatném formátu.
					</div>';

				$form .= '<div class="col-sm-4 form-group has-error has-feedback">
							<label>PSČ</label>
							<input name="zip" type="text" maxlength="5" placeholder="PSČ (nepovinné)" class="form-control" value="' . $_POST["zip"] . '"/>
						</div>
					</div>';
			}
			else {
				$form .= '<div class="col-sm-4 form-group">
							<label>PSČ</label>
							<input name="zip" type="text" maxlength="5" placeholder="PSČ (nepovinné)" class="form-control" value="' . $_POST["zip"] . '"/>
						</div>
					</div>';
			}
		}

		// Check if the terms were accepted.
		if (!isset($_POST["terms"]) or empty($_POST["terms"])) {
			$error = true;
			echo '<div class="alert alert-danger alert-dismissable fade in">
					<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
					Pro úspěšnou registraci je potřeba souhlasit s obchodními podmínkami.
				</div>';
		}

		// If everything is right insert user to database
		if ($error === false) {
			$sqlQuery = 'INSERT INTO Uzivatel (email, jmeno, prijmeni, telefon, heslo, ulice, mesto, psc)
				VALUES (\'' . $_POST["email"] . '\', \'' . $_POST["name"] . '\', \'' . $_POST["surname"] . '\', \'' . $_POST["phone"] . '\', \'' . md5($_POST["password1"]) . '\', \'' . $_POST["street"] .
				'\', \'' . $_POST["city"] . '\', ' . $_POST["zip"] . ')';
			$result = mysql_query($sqlQuery, $db);

			// Insert query failed
			if (!$result) {
				echo '<div class="alert alert-danger alert-dismissable fade in">
						  <a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						  Selhalo vložení nového uživatele do databáze.
					  </div>';
			}
			// Insert query success
			else {
				$message = "Byl vám uspěšně vytvořen účet na stránce AAB půjčovna.<br />Vaše heslo je: " . $_POST["password1"] . "<br /><br />Děkuji<br />Půjčovna AAB";
				if (mail($_POST["email"], 'AAB pujcovna: Uspesna registrace', $message, 'Content-type: text/html; charset=UTF-8')) {
					echo '<div class="alert alert-success alert-dismissable fade in">
						<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						Registrace proběhla ůspěšně.
					</div>';
				}
				else {
					echo '<div class="alert alert-danger alert-dismissable fade in">
						<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						Selhalo odeslání emailu.
					</div>';
				}
			}
		}

		echo $form;
		mysql_close($db);
	}

?>

						<hr>
						<div class="form-group">
							<label>Obchodní podmínky <span style="color:red;">*</span></label>
							<div class="form-group">
								<input name="terms" type="checkbox" value="true"/>
								<span>Četl jsem <a id="showTermsReg" href="#" style="color:#3746ff;">obchodní podmínky společnosti AAB Sport</a> a s uvedenými podmínkami souhlasím.</span>
							</div>
						</div>
					</div>
				</div>
				<div class="form-row">
					<button type="submit">Vytvořit účet</button>
				</div>
			</div>
		</form>

<?php 
	include("terms.php");
	include("footer.php");
?>
