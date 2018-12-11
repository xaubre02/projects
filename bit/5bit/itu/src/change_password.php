<?php include("header.php");?>

<body>

<?php include("side_menu.php");?>

<?php include("header_bar.php");?>

		<form class="form-basic" method="post">
			<div class="col-sm-12 well">
				<div class="form-title-row">
					<h1>Změna hesla</h1>
				</div>
				<div class="row">

<?php
	if (!isset($_SESSION["email"]))
		header('Location: index.php');

	// First attempt to change user password form.
	if (!isset($_POST["old_pwd"]) or !isset($_POST["new_pwd1"]) or !isset($_POST["new_pwd1"])) {
		;
	}
	// Information from registration form is processed.
	else {
		include("db_connect.php");

		$form = '';
		$error = false;


		$sqlQuery = 'SELECT email FROM Uzivatel WHERE email=\'' . $_SESSION["email"] . '\' AND heslo=\'' . md5($_POST["old_pwd"]) . '\'';
		$result = mysql_query($sqlQuery);

		if (mysql_num_rows($result) == 0) {
			$error = true;
			echo '<div class="alert alert-danger alert-dismissable fade in">
					<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
					Špatné staré heslo.
				</div>';
		}

		if ($_POST["new_pwd1"] != $_POST["new_pwd2"]) {
			$error = true;
			echo '<div class="alert alert-danger alert-dismissable fade in">
					<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
					Nové hesla se neshodují.
				</div>';
		}
		else if (strlen($_POST["new_pwd1"]) < 6){
			$error = true;
			echo '<div class="alert alert-danger alert-dismissable fade in">
					<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
					Nové hesla je příliš krátké (min 6 znaků).
				</div>';
		}


		// If everything is right insert user to database
		if ($error === false) {
			$sqlQuery = 'UPDATE Uzivatel SET heslo=\'' . md5($_POST["new_pwd1"]) . '\' WHERE email=\'' . $_SESSION["email"] . '\'';
			$result = mysql_query($sqlQuery, $db);

			// Insert query failed
			if (!$result) {
				echo '<div class="alert alert-danger alert-dismissable fade in">
						<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						Selhala změna hesla z důvodů databáze.
					</div>';
			}
			// Insert query success
			else {
				echo '<div class="alert alert-success alert-dismissable fade in">
						<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						Změna hesla proběhlo ůspěšně.
					</div>';
			}
		}

		echo $form;
		mysql_close($db);
	}
?>

					<div class="col-sm-6 col-sm-offset-3 form-group">
						<label>Staré heslo <span style="color:red;">*</span></label>
						<input name="old_pwd" type="password" placeholder="Váše staré heslo" class="form-control"/>
					</div>
					<div class="col-sm-6 col-sm-offset-3 form-group">
						<label>Nové heslo <span style="color:red;">*</span></label>
						<input name="new_pwd1" type="password" placeholder="Váše nové heslo" class="form-control"/>
					</div>
					<div class="col-sm-6 col-sm-offset-3 form-group">
						<label>Nové heslo znovu <span style="color:red;">*</span></label>
						<input name="new_pwd2" type="password" placeholder="Váše nové heslo ještě jednou" class="form-control"/>
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
