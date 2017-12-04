<?php include("header.php");?>

<body>

<?php include("side_menu.php");?>

<?php include("header_bar.php");?>

		<form class="form-basic" method="post">
			<div class="col-sm-12 well">
				<div class="form-title-row">
					<h1>Zadejte údaje pro vytvoření nové podkategorie</h1>
				</div>
				<div class="row">

<?php
	if (!isset($_SESSION["email"]))
		header('Location: index.php');

	include('db_connect.php');

	if (isset($_POST["subcategoryName"]) and !empty($_POST["subcategoryName"])) {
		$error = false;
		$form = '';

		if ($_POST["categoryName"] == "0") {
			$error = true;

			echo '<div class="alert alert-danger alert-dismissable fade in">
					<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
					Není vybrána kategorie.
				</div>';
		}
		$form .= '<div class="col-sm-6 col-sm-offset-3 form-group">
					<label>Kategorie <span style="color:red;">*</span></label>
					<select class="form-control" name="categoryName">
						<option value="0">Nezadáno</option>';

		$sqlQuery = 'SELECT nazevKategorie FROM Kategorie WHERE hlavniKategorie IS NULL';
		$result = mysql_query($sqlQuery, $db);
		while ($row = mysql_fetch_array($result)) {
			if ($_POST["categoryName"] == $row["nazevKategorie"]) {
				$form .= '<option value="' . $row["nazevKategorie"] . '" selected>' . $row["nazevKategorie"] . '</option>';
			}
			else {
				$form .= '<option value="' . $row["nazevKategorie"] . '">' . $row["nazevKategorie"] . '</option>';
			}
		}

		$form .= '</select>
			</div>';

		$sqlQuery = 'SELECT nazevKategorie FROM Kategorie WHERE nazevKategorie=\'' . $_POST["subcategoryName"] . '\' AND hlavniKategorie IS NOT NULL';
		$result = mysql_query($sqlQuery, $db);

		if (mysql_num_rows($result) != 0) {
			$error = true;

			echo '<div class="alert alert-danger alert-dismissable fade in">
					<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
					Podkategorie s tímto názvem již existuje.
				</div>';
		}

		if ($error === false) {
			$sqlQuery = 'INSERT INTO Kategorie (nazevKategorie, hlavniKategorie) VALUES (\'' . $_POST["subcategoryName"] . '\', \'' . $_POST["categoryName"] . '\')';
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

		$form .= '<div class="col-sm-6 col-sm-offset-3 form-group">
					<label>Název podkategorie <span style="color:red;">*</span></label>
					<input name="subcategoryName" type="text" placeholder="Název podkategorie" class="form-control" value="' . $_POST["subcategoryName"] . '"/>
				</div>';

		echo $form;

		mysql_close($db);
	}
	else {
		echo '<div class="col-sm-6 col-sm-offset-3 form-group">
				<label>Kategorie <span style="color:red;">*</span></label>
				<select class="form-control" name="categoryName">
					<option value="0" selected>Nezadáno</option>';

		$sqlQuery = 'SELECT nazevKategorie FROM Kategorie WHERE hlavniKategorie IS NULL';
		$result = mysql_query($sqlQuery, $db);
		while ($row = mysql_fetch_array($result)) {
			echo '<option value="' . $row["nazevKategorie"] . '">' . $row["nazevKategorie"] . '</option>';
		}

		echo '</select>
			</div>';

		echo '<div class="col-sm-6 col-sm-offset-3 form-group">
				<label>Název podkategorie <span style="color:red;">*</span></label>
				<input name="subcategoryName" type="text" placeholder="Název podkategorie" class="form-control"/>
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
