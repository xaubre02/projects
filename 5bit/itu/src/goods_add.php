<?php include("header.php");?>

<body>

<?php include("side_menu.php");?>

<?php include("header_bar.php");?>

		<form class="form-basic" method="post" enctype="multipart/form-data">
			<div class="col-sm-12 well">
				<div class="form-title-row">
					<h1>Zadejte údaje pro vytvoření nového zboží</h1>
				</div>

<?php
	if (!isset($_SESSION["email"]))
		header('Location: index.php');

	include("db_connect.php");

	if (!isset($_POST["name"]) or !isset($_POST["cost"]) or !isset($_POST["deposit"]) or !isset($_POST["date"]) or !isset($_POST["categoryName"]) or !isset($_POST["subcategoryName"])) {
		echo '<div class="row">
				<div class="col-sm-6 form-group">
					<label>Název <span style="color:red;">*</span></label>
					<input name="name" type="text" placeholder="Název zboží" class="form-control" />
				</div>
				<div class="col-sm-6 form-group">
					<label>Cena za den <span style="color:red;">*</span></label>
					<input name="cost" type="number" min="1" placeholder="Cena za půjčení zboží na den" class="form-control" />
				</div>
			</div>

			<div class="row">
				<div class="col-sm-6 form-group">
					<label>Záloha <span style="color:red;">*</span></label>
					<input name="deposit" type="number" min="1" placeholder="Záloha" class="form-control" />
				</div>
				<div class="col-sm-6 form-group">
					<label>Datum výroby <span style="color:red;">*</span></label>
					<input name="date" type="date" placeholder="Datum výroby" class="form-control" />
				</div>
			</div>

			<div class="row">
				<div class="col-sm-6 form-group">
					<label>Obrázek</label>
					<input name="picture" type="file" accept="image/*" class="form-control" />
				</div>
				<div class="col-sm-6 form-group">
					<label>Popis</label>
					<input name="description" type="text" placeholder="Popis" class="form-control" />
				</div>
			</div>

			<div class="row">
				<div class="col-sm-6 form-group">
					<label>Kategorie <span style="color:red;">*</span></label>
					<select class="form-control" id="categoryName" name="categoryName" onchange="goodsCategoryChange()">
						<option value="0">Nezadáno</option>';

		$sqlQuery = 'SELECT nazevKategorie FROM Kategorie WHERE hlavniKategorie IS NULL';
		$result = mysql_query($sqlQuery, $db);
		while ($row = mysql_fetch_array($result)) {
			echo '<option value="' . $row["nazevKategorie"] . '">' . $row["nazevKategorie"] . '</option>';
		}

		echo '		</select>
				</div>
				<div class="col-sm-6 form-group">
					<label>Podkategorie <span style="color:red;">*</span></label>
					<select class="form-control" id="subcategoryName" name="subcategoryName">
						<option value="0" selected>Nezadáno</option>
					</select>
				</div>
			</div>';
	}
	else {
		$form = '<div class="row">';
		$error = false;

		if (empty($_POST["name"])) {
			$error = true;

			$form .= '<div class="col-sm-6 form-group has-error has-feedback">
						<label>Název <span style="color:red;">*</span></label>
						<input name="name" type="text" placeholder="Název zboží" class="form-control" value="' . $_POST["name"] . '"/>
					</div>';
		}
		else {
			$form .= '<div class="col-sm-6 form-group">
						<label>Název <span style="color:red;">*</span></label>
						<input name="name" type="text" placeholder="Název zboží" class="form-control" value="' . $_POST["name"] . '"/>
					</div>';
		}

		if (empty($_POST["cost"])) {
			$error = true;

			$form .= '<div class="col-sm-6 form-group has-error has-feedback">
						<label>Cena za den <span style="color:red;">*</span></label>
						<input name="cost" type="number" min="1" placeholder="Cena za půjčení zboží na den" class="form-control" value="' . $_POST["cost"] . '"/>
					</div>';
		}
		else {
			$form .= '<div class="col-sm-6 form-group">
						<label>Cena za den <span style="color:red;">*</span></label>
						<input name="cost" type="number" min="1" placeholder="Cena za půjčení zboží na den" class="form-control" value="' . $_POST["cost"] . '"/>
					</div>';
		}

		$form .= '</div>
				<div class="row">';

		if (empty($_POST["deposit"])) {
			$error = true;

			$form .= '<div class="col-sm-6 form-group has-error has-feedback">
						<label>Záloha <span style="color:red;">*</span></label>
						<input name="deposit" type="number" min="1" placeholder="Záloha" class="form-control" value="' . $_POST["deposit"] . '"/>
					</div>';
		}
		else {
			$form .= '<div class="col-sm-6 form-group">
						<label>Záloha <span style="color:red;">*</span></label>
						<input name="deposit" type="number" min="1" placeholder="Záloha" class="form-control" value="' . $_POST["deposit"] . '"/>
					</div>';
		}

		if (empty($_POST["date"])) {
			$error = true;

			$form .= '<div class="col-sm-6 form-group has-error has-feedback">
						<label>Datum výroby <span style="color:red;">*</span></label>
						<input name="date" type="date" placeholder="Datum výroby" class="form-control" value="' . $_POST["date"] . '"/>
					</div>';
		}
		else {
			$form .= '<div class="col-sm-6 form-group">
						<label>Datum výroby <span style="color:red;">*</span></label>
						<input name="date" type="date" placeholder="Datum výroby" class="form-control" value="' . $_POST["date"] . '"/>
					</div>';
		}

		$form .= '</div>
				<div class="row">
					<div class="col-sm-6 form-group">
						<label>Popis</label>
						<input name="description" type="text" placeholder="Popis" class="form-control" value="' . $_POST["description"] . '"/>
					</div>';

		if (!empty($_FILES["picture"]["name"])) {
			$target_dir = "pic/";
			$target_file = $target_dir . basename($_FILES["picture"]["name"]);
			$uploadOk = true;
			$imageFileType = pathinfo($target_file,PATHINFO_EXTENSION);

			$check = getimagesize($_FILES["picture"]["tmp_name"]);
			if($check === false) {
				echo '<div class="alert alert-danger alert-dismissable fade in">
						<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						Vložený soubor není obrázek.
					</div>';
				$uploadOk = false;
			}
			if (file_exists($target_file)) {
				echo '<div class="alert alert-danger alert-dismissable fade in">
						<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						Obrázek s tímto název již existuje.
					</div>';
				$uploadOk = false;
			}
			if($imageFileType != "jpg" && $imageFileType != "png" && $imageFileType != "jpeg") {
				echo '<div class="alert alert-danger alert-dismissable fade in">
						<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						Špatný formát obrázku. (png, jpg, jpeg jsou povoleny)
					</div>';
				$uploadOk = false;
			}
			if ($uploadOk === false) {
				$error = true;
			}
			else {
				if (!move_uploaded_file($_FILES["picture"]["tmp_name"], $target_file)) {
					$error = true;
					echo '<div class="alert alert-danger alert-dismissable fade in">
						<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						Chyba ukládání obrázku.
					</div>';
				}
			}
		}

		$form.= '<div class="col-sm-6 form-group">
					<label>Obrázek</label>
					<input name="picture" type="file" accept="image/*" class="form-control" />
				</div>
			</div>
			<div class="row">';

		if ($_POST["categoryName"] == "0") {
			$error = true;
			$form .= '<div class="col-sm-6 form-group has-error has-feedback">';
		}
		else {
			$form .= '<div class="col-sm-6 form-group">';
		}

		$form .= '<label>Kategorie <span style="color:red;">*</span></label>
				<select class="form-control" id="categoryName" name="categoryName" onchange="goodsCategoryChange()">
					<option value="0">Nezadáno</option>';

		$sqlQuery = 'SELECT nazevKategorie FROM Kategorie WHERE hlavniKategorie IS NULL';
		$result = mysql_query($sqlQuery, $db);
		while ($row = mysql_fetch_array($result)) {
			$form .= '<option value="' . $row["nazevKategorie"] . '">' . $row["nazevKategorie"] . '</option>';
		}

		$form .= '		</select>
				</div>';

		if ($_POST["subcategoryName"] == "0") {
			$error = true;
			$form .= '<div class="col-sm-6 form-group has-error has-feedback">';
		}
		else {
			$form .= '<div class="col-sm-6 form-group">';
		}

		$form .= '<label>Podkategorie <span style="color:red;">*</span></label>
				<select class="form-control" id="subcategoryName" name="subcategoryName">
					<option value="0" selected>Nezadáno</option>
				</select>
			</div>
		</div>';

		if ($error === false) {
			$sqlQuery = 'INSERT INTO Zbozi (nazev, cena, zaloha, datumVyroby, popis, obrazek, nazevKategorie)
				VALUES (\'' . $_POST["name"] . '\', ' . $_POST["cost"] . ', ' . $_POST["deposit"] . ', \'' . $_POST["date"] . '\',
				\'' . $_POST["description"] . '\' , \'' . $_FILES["picture"]["name"] . '\' , \'' . $_POST["subcategoryName"] . '\')';
			$result = mysql_query($sqlQuery, $db);

			// Insert query failed
			if (!$result) {
				echo '<div class="alert alert-danger alert-dismissable fade in">
						  <a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						  Selhalo vložení nového zboží do databáze.
					  </div>';
			}
			// Insert query success
			else {
				echo '<div class="alert alert-success alert-dismissable fade in">
						<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						Zboží ůspěšně vloženo.
					</div>';
			}
		}

		echo $form;
	}
?>

<script>
<?php
	echo "var categories = [];";
	echo 'categories["0"] = \'<option value="0" selected>Nezadáno</option>\';';

	$sqlQuery = 'SELECT nazevKategorie FROM Kategorie WHERE hlavniKategorie IS NULL';
	$result = mysql_query($sqlQuery, $db);

	while ($row = mysql_fetch_array($result)) {
		echo 'categories["' . $row["nazevKategorie"] . '"] = \'<option value="0" selected>Nezadáno</option>\';';
	}

	$sqlQuery = 'SELECT nazevKategorie, hlavniKategorie FROM Kategorie WHERE hlavniKategorie IS NOT NULL';
	$result = mysql_query($sqlQuery, $db);

	while ($row = mysql_fetch_array($result)) {
		echo 'categories["' . $row["hlavniKategorie"] . '"] += \'<option value="' . $row["nazevKategorie"] . '">' . $row["nazevKategorie"] . '</option>\';';
	}
?>

	function goodsCategoryChange() {
		var select = document.getElementById("categoryName");
		var option = select.options[select.selectedIndex].value;

		document.getElementById("subcategoryName").innerHTML = categories[option];
	}
</script>

				<div class="form-row">
					<button type="submit">Vytvořit zboží</button>
				</div>
			</div>
		</form>


<?php
	include("terms.php");
	include("footer.php");
?>
