	<!-- Zatemnění -->
	<div class="cover hide"></div>
	<!-- LOGO -->
	<nav class="sidebar">
		<a href="index.php" class="logo">
			<img src="pic/logo.png">
		</a>

		<!-- Boční menu -->
		<nav class="side_menu">

<?php
	include("db_connect.php");

	$categoryCounter = 0;
	$subcategoryCounter = 0;
	$form = '';

	$sqlQuery = 'SELECT nazevKategorie FROM Kategorie WHERE hlavniKategorie IS NULL';
	$result = mysql_query($sqlQuery, $db);

	echo '<a href="index.php" class="button menu_action ';
	if (basename($_SERVER['PHP_SELF']) == "index.php")
		echo 'selected';
	
	echo '"><span>Akční nabídka</span></a>';
	
	while ($row = mysql_fetch_array($result)) {
		$form .= '<a href="catalog.php?hlavniKategorie=' . $row["nazevKategorie"] . '" class="menu_item button" onclick="select_category_catalog(' . $categoryCounter . ')">
					<span>' . $row["nazevKategorie"] . '</span>
					<img src="pic/arrow_down.png">
				</a>';
		$categoryCounter += 1;

		$sqlQuerySub = 'SELECT nazevKategorie FROM Kategorie WHERE hlavniKategorie=\'' . $row["nazevKategorie"] . '\'';
		$resultSub = mysql_query($sqlQuerySub);

		$form .= '<nav class="submenu hide">';

		while ($rowSub = mysql_fetch_array($resultSub)) {
			$form .= '<a href="catalog.php?podKategorie=' . $rowSub["nazevKategorie"] . '" class="submenu_item button" onclick="select_subcategory(' . $subcategoryCounter . ')">
						<img src="pic/arrow_right.png">
						<span>' . $rowSub["nazevKategorie"] . '</span>
					</a>';
			$subcategoryCounter += 1;
		}

		$form .= '</nav>';
	}

	echo $form;
?>

		</nav>
	</nav>
