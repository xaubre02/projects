<?php include("header.php");?>

<body onload="load_from_cookies()">

<?php include("side_menu_catalog.php");?>

<?php
	// Process reservation if some exists
	$error = false;
	$message = '';

	if (isset($_POST["datumOd"]) or isset($_POST["datumDo"])) {
		if (!isset($_SESSION["email"])) { // unlogged user could not reserve goods.
			$message = '<div class="alert alert-danger alert-dismissable fade in">
							<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
							Nepřihlášený uživatel si nemůže rezervovat zboží.
						</div>';
		}
		else {
			include("db_connect.php");

			if (empty($_POST["datumOd"])) {
				$error = true;
				$message = '<div class="alert alert-danger alert-dismissable fade in">
								<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
								Nebylo zvoleno počáteční datum rezervace.
							</div>';
			}

			if (empty($_POST["datumDo"])) {
				$error = true;
				$message = '<div class="alert alert-danger alert-dismissable fade in">
								<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
								Nebylo zvoleno koncové datum rezervace.
							</div>';
			}

			if (strtotime($_POST["datumOd"]) > strtotime($_POST["datumDo"])) {
				$error = true;
				$message = '<div class="alert alert-danger alert-dismissable fade in">
								<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
								Datum počátku rezervace nesmí být větší než datum konce rezervace.
							</div>';
			}

			$sqlQuery = 'SELECT cislo_zbozi FROM Rezervuje WHERE cislo_zbozi=' . $_POST["cislo_zbozi"] . ' AND ((datumOd>=\'' . $_POST["datumOd"] . '\' AND datumOd<=\'' . $_POST["datumDo"] . '\') OR
				(datumDo>=\'' . $_POST["datumOd"] . '\' AND datumDo<=\'' . $_POST["datumDo"] . '\'))';

			$result = mysql_query($sqlQuery, $db);

			if (mysql_num_rows($result) != 0) {
				$error = true;
				$message = '<div class="alert alert-danger alert-dismissable fade in">
								<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
								Zboží je již rezervováno.
							</div>';
			}

			$sqlQuery = 'SELECT cislo_zbozi FROM KosikObsahuje WHERE cislo_zbozi=' . $_POST["cislo_zbozi"] . ' AND ((datumOd>=\'' . $_POST["datumOd"] . '\' AND datumOd<=\'' . $_POST["datumDo"] . '\') OR
				(datumDo>=\'' . $_POST["datumOd"] . '\' AND datumDo<=\'' . $_POST["datumDo"] . '\'))';
			$result = mysql_query($sqlQuery, $db);

			if (mysql_num_rows($result) != 0) {
				$error = true;
				$message = '<div class="alert alert-danger alert-dismissable fade in">
								<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
								Zboží je již rezervováno.
							</div>';
			}

			if ($error === false) {
				$sqlQuery = 'INSERT INTO KosikObsahuje (email, cislo_zbozi, datumOd, datumDo, cenaDen)
					VALUES (\'' . $_SESSION["email"] . '\', ' . $_POST["cislo_zbozi"] . ', \'' . $_POST["datumOd"] . '\', \'' . $_POST["datumDo"] . '\', ' . $_POST["cena"] . ')';
				$result = mysql_query($sqlQuery, $db);

				// Insert query failed
				if (!$result) {
					$message = '<div class="alert alert-danger alert-dismissable fade in">
									<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
									Selhalo vložení rezervace do databáze.
								</div>';
				}
				// Insert query success
				else {
					$message = '<div class="alert alert-success alert-dismissable fade in">
									<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
									Vaše rezervace byla úspešně vložena do košíku.
								</div>';
				}
			}
		}
	}
?>

<?php include("header_bar.php");?>

		<div class="catalog">

<?php
	echo $message;
?>


			<div class="cat_cap">
				<h1 id="caption"></h1>
				<h2 id="subcaption"></h2>
			</div>

			<div class="filter">
				<span>Název</span>
				<span>Záloha</span>
				<span>Cena za den</span>
				<span>Datum</span>
			</div>

<?php
	// show the goods from database
	include("db_connect.php");

	$form = '';
	$i = 0;

	if (isset($_GET["hlavniKategorie"])) {
		$sqlQuery = 'SELECT cislo_zbozi, nazev, cena, zaloha, popis, obrazek FROM Zbozi NATURAL JOIN Kategorie WHERE hlavniKategorie=\'' . $_GET["hlavniKategorie"] . '\'';
	}
	else if (isset($_GET["podKategorie"])) {
		$sqlQuery = 'SELECT cislo_zbozi, nazev, cena, zaloha, popis, obrazek FROM Zbozi WHERE nazevKategorie=\'' . $_GET["podKategorie"] . '\'';
	}
	$result = mysql_query($sqlQuery, $db);

	while ($row = mysql_fetch_array($result)) {
		$cena = $row["cena"];
		$akceSpan = '';

		$sqlQueryAkce = 'SELECT akcni_cena FROM Akce WHERE cislo_zbozi=\'' . $row["cislo_zbozi"] . '\'';
		$resultAkce = mysql_query($sqlQueryAkce);

		if (mysql_num_rows($resultAkce) != 0) {
			$rowAkce = mysql_fetch_row($resultAkce);

			$cena = $rowAkce[0];
			$akceSpan = '<span class="action_layer">Akce</span>';
		}

		$form .= '<div class="product">
					<img src="pic/' . $row["obrazek"] . '"/>' . $akceSpan . '
					<div class="product_name">
						<span>' . $row["nazev"] . '</span>
						<p>
							' . $row["popis"] . '
						</p>
					</div>
					<span>' . $row["zaloha"] . ' Kč</span>
					<span>' . $cena . ' Kč</span>
					<form method="post">
						Od<input id="' . $i . 'datepickerFrom" type="text" name="datumOd"><br />
						<button type="submit" class="button">Rezervovat</button>
						Do<input id="' . $i . 'datepickerTo" type="text" name="datumDo">
						<input type="hidden" name="cislo_zbozi" value="' . $row["cislo_zbozi"] . '">
						<input type="hidden" name="cena" value="' . $cena . '">
					</form>
				</div>
				<script>
					var startDate' . $i . ' = "";
					var endDate' . $i . '  = "";
					var dateRange' . $i . ' = [];';

		// Find out when is the good reserved.
		$sqlQueryRes = 'SELECT datumOd, datumDo FROM Rezervuje WHERE cislo_zbozi=\'' . $row["cislo_zbozi"] . '\'';
		$resultRes = mysql_query($sqlQueryRes, $db);

		while($rowRes = mysql_fetch_array($resultRes)) {
			$form .= 'startDate' . $i . ' = "' . $rowRes["datumOd"] . '";
					endDate' . $i . ' = "' . $rowRes["datumDo"] . '";

					for (var d = new Date(startDate' . $i . '); d <= new Date(endDate' . $i . '); d.setDate(d.getDate() + 1)) {
						dateRange' . $i . '.push($.datepicker.formatDate(\'yy-mm-dd\', d));
					}';
		}

		// Find out which good is already in some user's cart
		$sqlQueryRes = 'SELECT datumOd, datumDo FROM KosikObsahuje WHERE cislo_zbozi=\'' . $row["cislo_zbozi"] . '\'';
		$resultRes = mysql_query($sqlQueryRes, $db);

		while($rowRes = mysql_fetch_array($resultRes)) {
			$form .= 'startDate' . $i . ' = "' . $rowRes["datumOd"] . '";
					endDate' . $i . ' = "' . $rowRes["datumDo"] . '";

					for (var d = new Date(startDate' . $i . '); d <= new Date(endDate' . $i . '); d.setDate(d.getDate() + 1)) {
						dateRange' . $i . '.push($.datepicker.formatDate(\'yy-mm-dd\', d));
					}';
		}

		$form .=	'$(\'#' . $i . 'datepickerFrom\').datepicker({
						changeMonth: true,
						changeYear: true,
						showAnim: "slideDown",
						minDate: new Date(),
						dateFormat: "yy-mm-dd",
						beforeShowDay: function(date){
							var string = jQuery.datepicker.formatDate(\'yy-mm-dd\', date);
							return [ dateRange' . $i . '.indexOf(string) == -1 ]
						}
					});

					$(\'#' . $i . 'datepickerTo\').datepicker({
						changeMonth: true,
						changeYear: true,
						showAnim: "slideDown",
						minDate: new Date(),
						dateFormat: "yy-mm-dd",
						beforeShowDay: function(date){
							var string = jQuery.datepicker.formatDate(\'yy-mm-dd\', date);
							return [ dateRange' . $i . '.indexOf(string) == -1 ]
						}
					});
				</script>';

		$i += 1;
	}

	echo $form;
?>

		</div>

		
<?php 
	include("terms.php");
	include("footer.php");
?>
