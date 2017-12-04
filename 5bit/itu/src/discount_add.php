<?php include("header.php");?>

<body>

<?php include("side_menu.php");?>

<?php include("header_bar.php");?>

		<form class="form-basic" method="post">
			<div class="col-sm-12 well">
				<div class="form-title-row">
					<h1>Formulář pro vložení akce na zboží</h1>
				</div>

<?php
	if (!isset($_SESSION["email"]))
		header('Location: index.php');

	if (isset($_POST["goods_number"]) and isset($_POST["discount_prise"])) {
		$error = false;
		if (empty($_POST["goods_number"])) {
			$error = true;
			echo '<div class="alert alert-danger alert-dismissable fade in">
					<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
					Žádné zboží není vybráno.
				</div>';
		}
		else {
			$sqlQuery = 'SELECT * FROM Akce WHERE cislo_zbozi=' . $_POST["goods_number"] . '';
			$result = mysql_query($sqlQuery, $db);

			if (mysql_num_rows($result) != 0) {
				$error = true;
				echo '<div class="alert alert-danger alert-dismissable fade in">
						<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						Na zvolené zboží již existuje akce.
					</div>';
			}
		}

		if (empty($_POST["discount_prise"])) {
			$error = true;
			echo '<div class="alert alert-danger alert-dismissable fade in">
					<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
					Není specifikována akční cena.
				</div>';
		}

		if ($error === false) {
			$sqlQuery = 'INSERT INTO Akce (cislo_zbozi, akcni_cena) VALUES (' . $_POST["goods_number"] . ', ' . $_POST["discount_prise"] . ')';
			$result = mysql_query($sqlQuery, $db);

			// Insert query failed
			if (!$result) {
				echo '<div class="alert alert-danger alert-dismissable fade in">
						  <a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						  Selhalo vložení akce do databáze.
					  </div>';
			}
			// Insert query success
			else {
				echo '<div class="alert alert-success alert-dismissable fade in">
						<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
						Akce ůspěšně vložena.
					</div>';
			}
		}
	}
?>

				<div class="row">
					<div class="col-sm-6 col-sm-offset-3 form-group">
						<label>Hlavní kategorie</label>
						<select class="form-control" id="categoryName" name="categoryName" onchange="CategoryChange()">
							<option value="0">Nezadáno</option>;

<?php
	include("db_connect.php");

	$sqlQuery = 'SELECT nazevKategorie FROM Kategorie WHERE hlavniKategorie IS NULL';
	$result = mysql_query($sqlQuery, $db);
	while ($row = mysql_fetch_array($result)) {
		echo '<option value="' . $row["nazevKategorie"] . '">' . $row["nazevKategorie"] . '</option>';
	}
?>

						</select>
					</div>
					<div class="col-sm-6 col-sm-offset-3 form-group">
						<label>Podkategorie</label>
						<select class="form-control" id="subcategoryName" name="subcategoryName" onchange="SubcategoryChange()">
							<option value="0">Nezadáno</option>;
						</select>
					</div>
					<div class="col-sm-6 col-sm-offset-3 form-group">
						<label>Zboží <span style="color:red;">*</span></label>
						<select class="form-control" id="goods_number" name="goods_number">
							<option value="0">Nezadáno</option>;
						</select>
					</div>
					<div class="col-sm-6 col-sm-offset-3 form-group">
						<label>Akční cena <span style="color:red;">*</span></label>
						<input name="discount_prise" type="number" min="1" placeholder="Akční cena v Kč" class="form-control"/>
					</div>
				</div>

<script>
	var categories = [];
	var goods = [];
	goods["0"] = '<option value="0">Nezadáno</option>';
	categories["0"] = '<option value="0">Nezadáno</option>';

<?php
	$sqlQuery = 'SELECT nazevKategorie FROM Kategorie WHERE hlavniKategorie IS NULL';
	$result = mysql_query($sqlQuery, $db);

	while ($row = mysql_fetch_array($result)) {
		echo 'categories["' . $row["nazevKategorie"] . '"] = \'<option value="0">Nezadáno</option>\';';
	}

	$sqlQuery = 'SELECT nazevKategorie, hlavniKategorie FROM Kategorie WHERE hlavniKategorie IS NOT NULL';
	$result = mysql_query($sqlQuery, $db);

	while ($row = mysql_fetch_array($result)) {
		echo 'categories["' . $row["hlavniKategorie"] . '"] += \'<option value="' . $row["nazevKategorie"] . '">' . $row["nazevKategorie"] . '</option>\';';

		echo 'goods["' . $row["hlavniKategorie"] . '"] = \'<option value="0">Nezadáno</option>\';';
		echo 'goods["' . $row["nazevKategorie"] . '"] = \'<option value="0">Nezadáno</option>\';';
	}

	$sqlQuery = 'SELECT cislo_zbozi, nazev, cena, nazevKategorie, hlavniKategorie FROM Zbozi NATURAL JOIN Kategorie';
	$result = mysql_query($sqlQuery, $db);

	while ($row = mysql_fetch_array($result)) {
		echo 'goods["' . $row["hlavniKategorie"] . '"] += \'<option value="' . $row["cislo_zbozi"] . '">' . $row["cislo_zbozi"] . ', ' . $row["nazev"] . ' (' . $row["cena"] . ' Kč)</option>\';';
		echo 'goods["' . $row["nazevKategorie"] . '"] += \'<option value="' . $row["cislo_zbozi"] . '">' . $row["cislo_zbozi"] . ', ' . $row["nazev"] . ' (' . $row["cena"] . ' Kč)</option>\';';
	}
?>

	function CategoryChange() {
		var select = document.getElementById("categoryName");
		var option = select.options[select.selectedIndex].value;

		document.getElementById("subcategoryName").innerHTML = categories[option];
		document.getElementById("goods_number").innerHTML = goods[option];
	}

	function SubcategoryChange() {
		var select = document.getElementById("subcategoryName");
		var option = select.options[select.selectedIndex].value;

		if (option != "0") {
			document.getElementById("goods_number").innerHTML = goods[option];
		}
		else {
			select = document.getElementById("categoryName");
			option = select.options[select.selectedIndex].value;
			document.getElementById("goods_number").innerHTML = goods[option];
		}
	}
</script>

				<div class="form-row">
					<button type="submit">Vložit</button>
				</div>
			</div>
		</form>


<?php
	include("terms.php");
	include("footer.php");
?>

