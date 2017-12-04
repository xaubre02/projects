<?php include("header.php");?>

<body>

<?php include("side_menu.php");?>

<?php include("header_bar.php");?>

		<form class="form-basic" method="post">
			<div class="col-sm-12 well">
				<div class="form-title-row">
					<h1>Zadejte údaje pro vytvoření nové kategorie</h1>
				</div>
				<div class="row">

<?php
	if (!isset($_SESSION["email"]))
		header('Location: index.php');

	if (isset($_POST["categoryName"]) and !empty($_POST["categoryName"])) {
		include 'db_connect.php';

		$error = false;

		$sqlQuery = 'SELECT nazevKategorie FROM Kategorie WHERE nazevKategorie=\'' . $_POST["categoryName"] . '\' AND hlavniKategorie IS NULL';
		$result = mysql_query($sqlQuery, $db);

		if (mysql_num_rows($result) != 0) {
			$error = true;

			echo '<div class="alert alert-danger alert-dismissable fade in">
					<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
					Kategorie s tímto názvem již existuje.
				</div>';
		}

		if ($error === false) {
			$sqlQuery = 'INSERT INTO Kategorie (nazevKategorie, hlavniKategorie) VALUES (\'' . $_POST["categoryName"] . '\', NULL)';
			$result = mysql_query($sqlQuery, $db);

			// Insert query failed
			if (!$result) {
				echo '<div class="alert alert-danger alert-dismissable fade in">
						  <a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						  Selhalo vložení nové kategorie do databáze.
					  </div>';
			}
			// Insert query success
			else {
				echo '<div class="alert alert-success alert-dismissable fade in">
						<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						Kategorie ůspěšně vložena.
					</div>';
			}
		}

		echo '<div class="col-sm-6 col-sm-offset-3 form-group">
				<label>Název kategorie <span style="color:red;">*</span></label>
				<input name="categoryName" type="text" placeholder="Název kategorie" class="form-control" value="' . $_POST["categoryName"] . '"/>
			</div>';

		mysql_close($db);
	}
	else {
		echo '<div class="col-sm-6 col-sm-offset-3 form-group">
				<label>Název kategorie <span style="color:red;">*</span></label>
				<input name="categoryName" type="text" placeholder="Název kategorie" class="form-control"/>
			</div>';
	}
?>

				</div>
				<div class="form-row">
					<button type="submit">Uložit</button>
				</div>
			</div>
		</form>


<?php
	include("terms.php");
	include("footer.php");
?>
