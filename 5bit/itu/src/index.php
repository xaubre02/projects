<?php include("header.php");?>

<body>

<?php include("side_menu.php");?>

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

		<!-- Úvodní obrázek -->
		<div class="intro_img">
			<img src="pic/snowboard.jpg">
			<div class="intro_overlay">
				<h1>Akční nabídka</h1>
				<h3>Loňská kolekce</h3>
				<p><a href="#cat" class="button">Prohlédnout</a></p>
			</div>
		</div>

<?php
	echo $message;
?>

		<!-- Produkty -->
		<div class="catalog" id="cat">
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

	$sqlQuery = 'SELECT cislo_zbozi, nazev, akcni_cena, zaloha, popis, obrazek FROM Akce NATURAL JOIN Zbozi NATURAL JOIN Kategorie';
	$result = mysql_query($sqlQuery, $db);

	while ($row = mysql_fetch_array($result)) {
		$form .= '<div class="product">
					<img src="pic/' . $row["obrazek"] . '"/>
					<span class="action_layer">Akce</span>
					<div class="product_name">
						<span>' . $row["nazev"] . '</span>
						<p>
							' . $row["popis"] . '
						</p>
					</div>
					<span>' . $row["zaloha"] . ' Kč</span>
					<span>' . $row["akcni_cena"] . ' Kč</span>
					<form method="post">
						Od<input id="' . $i . 'datepickerFrom" type="text" name="datumOd"><br />
						<button type="submit" class="button">Rezervovat</button>
						Do<input id="' . $i . 'datepickerTo" type="text" name="datumDo">
						<input type="hidden" name="cislo_zbozi" value="' . $row["cislo_zbozi"] . '">
						<input type="hidden" name="cena" value="' . $row["akcni_cena"] . '">
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
