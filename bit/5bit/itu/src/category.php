<?php include("header.php");?>

<body>

<?php include("side_menu.php");?>

<?php include("header_bar.php");?>
		<div class="div-upper-footer">
			<div class="col-sm-12 well">
				<div class="form-title-row">
					<h1>Hlavní kategorie zboží</h1>
				</div>
				<div class="table-responsive">
					<table class="table table-striped">
						<thead>
							<tr>
								<th>Hlavní kategorie</th><th>Akce</th>
							</tr>
						</thead>
						<tbody>

<?php
	if (!isset($_SESSION["email"]))
		header('Location: index.php');

	include("db_connect.php");

	if (isset($_GET["categoryName"])) {
		$sqlQuery = 'DELETE FROM Kategorie WHERE nazevKategorie=\'' . $_GET["categoryName"] . '\'';
		$result = mysql_query($sqlQuery, $db);
	}

	$sqlQuery = 'SELECT nazevKategorie FROM Kategorie WHERE hlavniKategorie IS NULL';
	$result = mysql_query($sqlQuery, $db);

	while ($row = mysql_fetch_array($result)) {
		echo '<tr>
				<td>' . $row["nazevKategorie"] . '</td>
				<td><a href="category.php?categoryName=' . $row["nazevKategorie"] . '" class="btn btn-danger btn-sm" role="button">Odebrat hlavní kategorii</a></td>
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
