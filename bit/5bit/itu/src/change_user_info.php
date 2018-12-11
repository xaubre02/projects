<?php include("header.php");?>

<body>

<?php include("side_menu.php");?>

<?php include("header_bar.php");?>

		<form class="form-basic" method="post">
			<div class="col-sm-12 well">
				<div class="form-title-row">
					<h1>Změna údajů</h1>
				</div>
				<div class="row">
					<div class="col-sm-12">

<?php
	if (!isset($_SESSION["email"]))
		header('Location: index.php');

	include 'db_connect.php';

	// First attempt to change user information form.
	if (!isset($_POST["name"]) or !isset($_POST["surname"]) or !isset($_POST["email"]) or !isset($_POST["phone"]) or
		!isset($_POST["street"]) or !isset($_POST["city"]) or !isset($_POST["zip"])) {

		$sqlQuery = 'SELECT jmeno, prijmeni, email, telefon, ulice, mesto, psc FROM Uzivatel WHERE email=\'' . $_SESSION["email"] . '\'';
		$result = mysql_query($sqlQuery);

		$row = mysql_fetch_row($result);

		echo '<div class="row">
				<div class="col-sm-6 form-group">
					<label>Jméno <span style="color:red;">*</span></label>
					<input name="name" type="text" placeholder="Vaše jméno" class="form-control" value="' . $row[0] . '"/>
				</div>
				<div class="col-sm-6 form-group">
					<label>Příjmení <span style="color:red;">*</span></label>
					<input name="surname" type="text" placeholder="Vaše příjmení" class="form-control" value="' . $row[1] . '"/>
				</div>
			</div>
			<div class="row">
				<div class="col-sm-6 form-group">
					<label>Email <span style="color:red;">*</span></label>
					<input name="email" type="email" placeholder="Váš email" class="form-control" value="' . $row[2] . '"/>
				</div>
				<div class="col-sm-6 form-group">
					<label>Telefon <span style="color:red;">*</span></label>
					<input name="phone" type="text" maxlength="9" placeholder="Váš telefon (777888999)" class="form-control" value="' . $row[3] . '"/>
				</div>
			</div>
			<div class="row">
				<div class="col-sm-4 form-group">
					<label>Ulice</label>
					<input name="street" type="text" placeholder="Ulice (nepovinné)" class="form-control" value="' . $row[4] . '"/>
				</div>
				<div class="col-sm-4 form-group">
					<label>Město</label>
					<input name="city" type="text" placeholder="Město (nepovinné)" class="form-control" value="' . $row[5] . '"/>
				</div>
				<div class="col-sm-4 form-group">
					<label>PSČ</label>
					<input name="zip" type="text" maxlength="5" placeholder="PSČ (nepovinné)" class="form-control" value="' . $row[6] . '"/>
				</div>
			</div>';
	}
	// Information from registration form is processed.
	else {
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

			if ((mysql_num_rows($result) != 0) and ($_POST["email"] != $_SESSION["email"])) {
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

		// If everything is right insert user to database
		if ($error === false) {
			$sqlQuery = 'UPDATE Uzivatel SET email=\'' . $_POST["email"] . '\', jmeno=\'' . $_POST["name"] . '\', prijmeni=\'' . $_POST["surname"] . '\', telefon=\'' . $_POST["phone"] . '\',
				ulice=\'' . $_POST["street"] .'\', mesto=\'' . $_POST["city"] . '\', psc=' . $_POST["zip"] . ' WHERE email=\'' . $_SESSION["email"] . '\'';
			$result = mysql_query($sqlQuery, $db);

			// Insert query failed
			if (!$result) {
				echo '<div class="alert alert-danger alert-dismissable fade in">
						  <a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						  Selhala úprava údajů z důvodů databáze.
					  </div>';
			}
			// Insert query success
			else {
				echo '<div class="alert alert-success alert-dismissable fade in">
						<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						Změna údajů proběhla ůspěšně.
					</div>';
			}
		}

		echo $form;
		mysql_close($db);
	}

?>

					</div>
				</div>
				<div class="form-row">
					<button type="submit">Uložit změny</button>
				</div>
			</div>
		</form>


<?php
	include("terms.php");
	include("footer.php");
?>
