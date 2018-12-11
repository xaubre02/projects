<?php include("header.php");?>

<body>

<?php include("side_menu.php");?>

<?php include("header_bar.php");?>
		<div class="div-upper-footer">
			<div class="col-sm-12 well">
				<div class="form-title-row">
					<h1>Mé rezervace</h1>
				</div>
				<div class="table-responsive">
					<table class="table table-striped">
						<thead>
							<tr>
								<th>Zboží</th><th>Název</th><th>Cena</th><th>Datum od</th><th>Datum do</th><th>Akce</th>
							</tr>
						</thead>
						<tbody>

<?php
	if (!isset($_SESSION["email"]))
		header('Location: index.php');

	include("db_connect.php");

	if (isset($_GET["goods_number"]) and isset($_GET["dateFrom"])) {
		$sqlQuery = 'DELETE FROM Rezervuje WHERE cislo_zbozi=\'' . $_GET["goods_number"] . '\' AND email=\'' . $_SESSION["email"] . '\' AND datumOd=\'' . $_GET["dateFrom"] . '\'';
		$result = mysql_query($sqlQuery, $db);
	}

	$sqlQuery = 'SELECT cislo_zbozi, nazev, obrazek, cenaDen, datumOd, datumDo FROM Zbozi NATURAL JOIN Rezervuje WHERE email=\'' . $_SESSION["email"] . '\' AND datumOd>\'' . date("Y-m-d") . '\'';
	$result = mysql_query($sqlQuery, $db);

	while ($row = mysql_fetch_array($result)) {
		$cenaCelkem = ((strtotime($row["datumDo"]) - strtotime($row["datumOd"])) / 86400) + 1;

		echo '<tr>
				<td><img class="media-object cart-img" src="pic/' . $row["obrazek"] . '" style="width: 6vw;height: 6vw;"/></td><td>' . $row["nazev"] . '</td>
				<td>' . $cenaCelkem * intval($row["cenaDen"]) . ' Kč</td>
				<td>' . $row["datumOd"] . '</td>
				<td>' . $row["datumDo"] . '</td>
				<td><a href="my_reservation.php?goods_number=' . $row["cislo_zbozi"] . '&dateFrom=' . $row["datumOd"] . '" class="btn btn-danger btn-sm" role="button">Odebrat rezervaci</a></td>
			</tr>';
	}
?>
						</tbody>
					</table>
				</div>
			</div>
		</div>


<?php
	include("terms.php");
	include("footer.php");
?>
