<?php include("header.php");?>

<body>

<?php include("side_menu.php");?>

<?php include("header_bar.php");?>

		<div class="div-upper-footer">
			<div class="col-sm-12 well">
				<div class="form-title-row">
					<h1>Akce</h1>
				</div>
				<div class="table-responsive">
					<table class="table table-striped">
						<thead>
							<tr>
								<th>Zboží</th><th>Cena</th><th>Akční cena</th><th>Akce</th>
							</tr>
						</thead>
						<tbody>

<?php
	if (!isset($_SESSION["email"]))
		header('Location: index.php');

	include("db_connect.php");

	if (isset($_GET["goods_number"])) {
		$sqlQuery = 'DELETE FROM Akce WHERE cislo_zbozi=\'' . $_GET["goods_number"] . '\'';
		$result = mysql_query($sqlQuery, $db);
	}

	$sqlQuery = 'SELECT cislo_zbozi, nazev, cena, akcni_cena FROM Akce NATURAL JOIN Zbozi';
	$result = mysql_query($sqlQuery, $db);

	while ($row = mysql_fetch_array($result)) {
		echo '<tr>
				<td>' . $row["cislo_zbozi"] . ', ' . $row["nazev"] . '</td>
				<td>' . $row["cena"] . '</td>
				<td>' . $row["akcni_cena"] . '</td>
				<td><a href="discount.php?goods_number=' . $row["cislo_zbozi"] . '" class="btn btn-danger btn-sm" role="button">Odebrat akci</a></td>
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
