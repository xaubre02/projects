<?php include("header.php");?>

<body>

<?php include("side_menu.php");?>

<?php include("header_bar.php");?>

		<form class="form-basic" method="post">
			<div class="col-sm-12 well">
				<div class="form-title-row">
					<h1>Zadejte údaje pro získání nového hesla</h1>
				</div>
				<div class="row">

<?php
	if (isset($_POST["email"])) {
		include 'db_connect.php';

		$sqlQuery = 'SELECT email FROM Uzivatel WHERE email=\'' . $_POST["email"] . '\'';
		$result = mysql_query($sqlQuery, $db);

		if (mysql_num_rows($result) == 0) {
			echo '<div class="alert alert-danger alert-dismissable fade in">
					<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
					Uživatel se zadaným emailem neexistuje.
				</div>';
		}
		else { // Change the password
			// generate random password (10 chars)
			$chars = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.:_';
			$password = array();
			$alphaLength = strlen($chars) - 1;
			for ($i = 0; $i < 10; $i++) {
				$n = rand(0, $alphaLength);
				$password[] = $chars[$n];
			}
			$password = implode($password);

			$sqlQuery = 'UPDATE Uzivatel SET heslo=\'' . md5($password) . '\'';
			$result = mysql_query($sqlQuery, $db);
			if ($result) {
				$message = "Bylo Vám vygenerováno nové heslo.<br />Vaše nové heslo je: " . $password . "<br /><br />Děkuji<br />Půjčovna AAB";
				if (mail($_POST["email"], 'AAB pujcovna: Zapomenute heslo', $message, 'Content-type: text/html; charset=UTF-8')) {
					echo '<div class="alert alert-success alert-dismissable fade in">
						<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						Bylo Vám odesláne nové heslo na zadaný email.
					</div>';
				}
				else {
					echo '<div class="alert alert-danger alert-dismissable fade in">
						<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						Selhalo odeslání emailu.
					</div>';
				}
			}
			else {
				echo '<div class="alert alert-danger alert-dismissable fade in">
						<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						Operace s databází selhala.
					</div>';
			}
		}

		echo '<div class="col-sm-6 col-sm-offset-3 form-group">
				<label>Email <span style="color:red;">*</span></label>
				<input name="email" type="email" placeholder="Váš email" class="form-control" value="' . $_POST["email"] . '"/>
			</div>';

		mysql_close($db);
	}
	else {
		echo '<div class="col-sm-6 col-sm-offset-3 form-group">
				<label>Email <span style="color:red;">*</span></label>
				<input name="email" type="email" placeholder="Váš email" class="form-control"/>
			</div>';
	}
?>

				</div>
				<div class="form-row">
					<button type="submit">Zaslat nové heslo na E-mail</button>
				</div>
			</div>
		</form>


<?php 
	include("terms.php");
	include("footer.php");
?>
