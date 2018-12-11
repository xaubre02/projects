<?php
	// User want to log in.
	$unknownUser = false;
	if (isset($_POST["logEmail"]) and isset($_POST["logPassword"])) {
		include("db_connect.php");

		$email = $_POST["logEmail"];
		$password = MD5($_POST["logPassword"]);

		$sqlQuery = 'SELECT jmeno, prijmeni FROM Uzivatel WHERE email=\'' . $email . '\' AND heslo=\'' . $password . '\'';
		$result = mysql_query($sqlQuery, $db);

		if (!$result) {
			die('Invalid query: ' . mysql_error());
		}

		if (mysql_num_rows($result) == 0) {
			$unknownUser = true;
		}
		else {
			$row = mysql_fetch_row($result);

			$_SESSION["user_name"] = $row[0] . ' ' . $row[1];
			$_SESSION["email"] = $email;
		}

		mysql_close($db);
	}

	// User want to log out.
	// if (isset($_POST["logOut"])) {
		// session_unset();
		// session_destroy();
	// }
?>

<div class="main">
		<!-- Hlavička -->
		<header class="">
			<div class="search">
				<input type="text" placeholder="Hledaný výraz" name="Hledaný výraz" required>
				<button class="button">Hledat</button>
			</div>
			<nav class="header">
<?php
	if (isset($_SESSION["email"])) {
			echo '<a id="cartRef" href="cart.php" class="header_item">
					<img src="pic/cart.png">
					<span>Košík</span>
				</a>';
	}
?>

<?php
	if (isset($_SESSION["user_name"])) {
		echo '<a id="userRef" href="#" class="header_item">
				<img src="pic/user.png">' .
				'<span>' . $_SESSION["user_name"] . '</span>';
	}
	else {
		echo '<a id="logRef" href="#" class="header_item">
				<img src="pic/user.png">' .
				'<span>Přihlásit se</span>';
	}
?>

				</a>
			</nav>
			<div class="parentPanel">
				<!-- Panel pro přihlášení -->
				<div id="logPanel" class="form-panel" style="display:none;">
					<div class="form">
						<h2>Přihlašte se ke svému účtu</h2>
						<form method="post">
							<input name="logEmail" type="email" placeholder="E-mail"/>
							<input name="logPassword" type="password" placeholder="Heslo"/>
							<button type="submit">Přihlásit se</button>
						</form>
					</div>
					<div class="cta"><a href="registration.php">Vytvořte si nový účet</a></div>
					<div class="cta b-radius"><a href="forg_password.php">Zapomněli jste heslo?</a></div>
				</div>

<?php
	if ($unknownUser === true) {
		echo '<script>
				var logPanel = document.getElementById("logPanel");
				logPanel.style.display = "block";

				// logPanel.innerHTML = \'<div class="badLog"><label>Tento účet neexistuje.</label></div>\' + logPanel.innerHTML;
				logPanel.innerHTML = \'<div id="badLog" class="alert alert-danger alert-dismissable fade in"><a id="closeBadLog" href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>Tento účet neexistuje nebo bylo zadáno špatné heslo.</div>\' + logPanel.innerHTML;
				
				// badLog div could be closed when clicked on X
				$("#closeBadLog").click(function(event){
						event.stopPropagation();  // stop propagation to enable display userPanel
						
						var bl = document.getElementById("badLog");
						bl.style.display = "none";
				});
				
			</script>';
	}
?>
				
				<div id="userPanel" class="form-panel" style="display:none;">
					<div class="dropdown">
						<div class="dropdown-user">
<?php
	if (isset($_SESSION["user_name"]) && isset($_SESSION["email"])) {
		echo '<h3>' . $_SESSION["user_name"] . '</h3>
			<h4>' . $_SESSION["email"] . '</h4>';
	}
?>
						</div>
						<div class="dropdown-content">
<?php
	if (isset($_SESSION["email"])) {
		if ($_SESSION["email"] == "admin@gmail.com") {  // admin possibilities
			echo '<a href="change_user_info.php">Moje údaje</a>
				<a href="my_reservation.php">Moje rezervace</a>
				<a href="category.php">Zobrazit kategorie</a>
				<a href="category_add.php">Přidat kategorie</a>
				<a href="subcategory.php">Zobrazit podkategorie</a>
				<a href="subcategory_add.php">Přidat podkategorie</a>
				<a href="discount.php">Zobrazit akce</a>
				<a href="discount_add.php">Přidat akce</a>
				<a href="goods_add.php">Přidat zboží</a>
				<a href="change_password.php">Změnit heslo</a>
				<a href="logout.php">Odhlásit</a>';
		}
		else {	// normal user 
			echo '<a href="change_user_info.php">Moje údaje</a>
				<a href="my_reservation.php">Moje rezervace</a>
				<a href="change_password.php">Změnit heslo</a>
				<a href="logout.php">Odhlásit</a>';
		}
	}
?>
						</div>
					</div>
				</div>

				<!-- Panel pro košík -->
				<div id="cartPanel" class="form-panel" style="display:none;">
					<div class="form">

<?php
	if (isset($_SESSION["email"])) {
		include("db_connect.php");

		$i = 1;

		$sqlQuery = 'SELECT nazev, obrazek, cenaDen FROM KosikObsahuje NATURAL JOIN Zbozi WHERE email=\'' . $_SESSION["email"] . '\'';
		$result = mysql_query($sqlQuery);

		if (mysql_num_rows($result) <= 0) {  // v kosiku nejsou zadne polozky
			echo '<div>
					<h2 class="text-center" >Váš košík je prázdný.</h2>
					<div class="text-center">
						<img src="pic/cart.png" class="empty-cart" alt="cart" />
					</div>
				</div>';
		}
		else {	// v kosiku jsou nejaky polozky
			echo '<div>
					<ul class="shopping-cart-items">';
			$i = 1;

			while ($row = mysql_fetch_array($result)) {
				echo '<li class="clearfix">
					<img src="pic/' . $row["obrazek"] . '" alt="item' . $i . '" />
					<span class="item-name">' . $row["nazev"] . '</span>
					<span class="item-price">' . $row["cenaDen"] . ' Kč/den</span>
				</li>';

				$i += 1;
			}

			echo '</ul>
				</div>
				<a href="cart.php" class="abutton">Do košíku</a>';
		}
	}

?>
					</div>
				</div>
			</div>
		</header>
