<?php include("header.php");?>

<body>

<?php include("side_menu.php");?>

<?php include("header_bar.php");?>

<?php
	if (!isset($_SESSION["email"]))
		header('Location: index.php');

	if (isset($_GET["goods_number"]) and isset($_GET["dateFrom"])) { // Delete goods from cart
		$sqlQuery = 'DELETE FROM KosikObsahuje WHERE email=\'' . $_SESSION["email"] . '\' AND cislo_zbozi=' . $_GET["goods_number"] . ' AND datumOd=\'' . $_GET["dateFrom"] . '\'';
		$result = mysql_query($sqlQuery, $db);
	}
	else if (isset($_GET["reserve"])) { // Reservation
		$sqlQuery = 'SELECT email, cislo_zbozi, datumOd, datumDo, cenaDen FROM KosikObsahuje WHERE email=\'' . $_SESSION["email"] . '\'';
		$result = mysql_query($sqlQuery, $db);

		while ($row = mysql_fetch_array($result)) {
			$sqlInsert = 'INSERT INTO Rezervuje (email, cislo_zbozi, datumOd, datumDo, cenaDen)
				VALUES (\'' . $row["email"] . '\', ' . $row["cislo_zbozi"] . ', \'' . $row["datumOd"] . '\', \'' . $row["datumDo"] . '\', ' . $row["cenaDen"] . ')';
			mysql_query($sqlInsert, $db);
		}

		$sqlDelete = 'DELETE FROM KosikObsahuje WHERE email=\'' . $_SESSION["email"] . '\'';
		mysql_query($sqlDelete, $db);
	}
?>


		<!-- STEP 1 -->
		<div id="cartStep1" style="display:block; width: 98%;">
			<div class="row">
				<div class="col-sm-12 well">
<?php
	include("db_connect.php");

	$sqlQuery = 'SELECT cislo_zbozi, nazev, obrazek, zaloha, datumOd, datumDo, cenaDen FROM Zbozi NATURAL JOIN KosikObsahuje WHERE email=\'' . $_SESSION["email"] . '\'';
	$result = mysql_query($sqlQuery);
	if (mysql_num_rows($result) != 0) {
			echo	'<table class="table table-hover">
						<thead>
							<tr>
								<th class="text-left">Zboží</th>
								<th class="text-left">Datum od</th>
								<th class="text-left">Datum do</th>
								<th class="text-center">Cena(Kč/den)</th>
								<th class="text-center">Akce</th>
							</tr>
						</thead>
						<tbody>';
	}
?>

<?php
	$cartTable1 = '';
	$cartTable2 = '';
	$tmp = '';

	$cenaCelkem = 0;
	$zalohaCelkem = 0;

	while ($row = mysql_fetch_array($result)) {
		$cenaCelkem += (((strtotime($row["datumDo"]) - strtotime($row["datumOd"])) / 86400) + 1) * intval($row["cenaDen"]);
		$zalohaCelkem += intval($row["zaloha"]);

		$tmp = '<tr>
					<td class="col-sm-3">
						<div class="media">
							<a class="pull-left" href="#"> <img class="media-object cart-img" src="pic/' . $row["obrazek"] . '"> </a>
							<div class="media-body">
								<h4 class="media-heading"><a href="#">' . $row["nazev"] . '</a></h4>
							</div>
						</div>
					</td>
					<td class="col-sm-2 text-center">
						<div class="input-group date">
							<div class="input-group date">
								<span class="glyphicon glyphicon-calendar"></span>
								' . $row["datumOd"] . '
							</div>
						</div>
					</td>
					<td class="col-sm-2 text-center">
						<div class="input-group date">
							<div class="input-group date">
								<span class="glyphicon glyphicon-calendar"></span>
								' . $row["datumDo"] . '
							</div>
						</div>
					</td>
					<td class="col-sm-1 text-center">
						<strong>' . $row["cenaDen"] . '</strong>
					</td>';

		$cartTable1 .= $tmp .
						'<td class="col-sm-1 text-center">
							<a href="cart.php?goods_number=' . $row["cislo_zbozi"] . '&dateFrom=' . $row["datumOd"] . '" class="btn btn-danger" role="button">
								<span class="glyphicon glyphicon-remove"></span>
								<span>Odebrat</span>
							</a>
						</td>
					</tr>';

		$cartTable2 .= $tmp . '</tr>';
	}

	echo $cartTable1;
?>

<?php
	if (mysql_num_rows($result) != 0) {
				echo '</tbody>
						<tfoot>
						<tr>
								<td></td>
								<td></td>
								<td></td>
								<td><h5>Vratná záloha</h5><h3>Cena celkem</h3></td>
								<td class="text-right"><h5>';
								echo $zalohaCelkem;
								echo ',-</strong></h5><h3>';
								echo $cenaCelkem;
								echo ',-</h3></td>
							</tr>
							<tr>
								<td></td>
								<td></td>
								<td></td>
								<td>
									<a href="index.php" type="button" class="btn btn-default">
										<span class="glyphicon glyphicon-shopping-cart"></span>
										Zpět do půjčovny
									</a>
								</td>
								<td>
									<button id="next1" type="button" class="btn item-blue">
										<span>Pokračovat</span>
										<span class="glyphicon glyphicon-step-forward"></span>
									</button>
								</td>
							</tr>
						</tfoot>
					</table>';
	}
	else {
		echo '<div class="col-md-12 text-center">
				<div class="form-title-row">
					<h1>Váš košík je aktuálně prázdný.</h1>
				</div>
			</div>
			<div class="col-md-12 text-center" style="margin-top:20px;">
				<img class="empty-cart" src="pic/cart.png" />
			</div>
			<div class="col-md-12 text-center" style="margin-top:40px;">
				<a href="index.php" type="button" class="btn btn-default">
					<span class="glyphicon glyphicon-shopping-cart"></span>
					Zpět do půjčovny
				</a>
			</div>';
	}
?>

				</div>
			</div>
		</div>



		<!-- STEP 2 -->
		<div id="cartStep2" class="col-sm-12 well" style="display:none;">
			<div class="row">
				<form>
					<div class="col-sm-12">
						<div class="form-group">
							<label>Poznámky k rezervaci</label>
							<textarea id="notesID" placeholder="Pokud potřebujete uvést dodatečné informace, napište je sem. (nepovinné)" rows="3" class="form-control"></textarea>
						</div>

						<hr>
						<div class="form-group">
							<label>Možnosti platby *</label>
							<div class="form-group">
								<span>Zvolte možnost platby.</span>
								<div class="radio">
									<label>
										<input id="cashID" name="radiopay" type="radio" value="Platba na pobočce (hotově nebo kartou)">
										Platba na pobočce (hotově nebo kartou)
									</label>
								</div>
								<div class="radio">
									<label>
										<input id="transferID" name="radiopay" type="radio" value="Platba převodem z účtu">
										Platba převodem z účtu
									</label>
								</div>
							</div>
						</div>
						<div class="form-group">
							<label>Obchodní podmínky *</label>
							<div class="form-group">
								<input id="termsID" type="checkbox" value=""/>
								<span id="termsText">Četl jsem <a id="showTermsCart" href="#" style="color:#3746ff;">obchodní podmínky společnosti AAB Sport</a> a s uvedenými podmínkami souhlasím.</span>
							</div>
						</div>

						<!-- Show when payment was not selected -->
						<div id="badRadioPay" style="display:none;" class="alert alert-danger alert-dismissable fade in">
							<a id="closeBadRadioPay" href="#" class="close" aria-label="close">&times;</a>
							Pro pokračování je nutné zvolit způsob platby.
						</div>
						<!-- Show when terms was not checked -->
						<div id="badTerms" style="display:none;" class="alert alert-danger alert-dismissable fade in">
							<a id="closeBadTerms" href="#" class="close" aria-label="close">&times;</a>
							Pro pokračování je nutné souhlasit s obchodními podmínkami.
						</div>

						<hr>

						<div class="row">
							<div class="col-sm-2"> </div>
							<div class="col-sm-2 form-group">
								<button id="back1" type="button" class="btn btn-default btn-lg">
									<span class="glyphicon glyphicon-step-backward"></span>
									<span>Zpět</span>
								</button>
							</div>
							<div class="col-sm-4"> </div>
							<div class="col-sm-2 form-group">
								<button id="next2" type="button" class="btn btn-lg item-blue">
									<span>Pokračovat</span>
									<span class="glyphicon glyphicon-step-forward"></span>
								</button>
							</div>
						</div>
					</div>
				</form>
			</div>
		</div>



		<!-- FINISH STEP -->
		<div id="cartStepFinish" class="col-sm-12 well" style="display:none;">
			<div class="row">
				<form>
					<div class="col-sm-12">
						<div class="row">
							<table class="table table-hover">
								<thead>
									<tr>
										<th class="text-left">Zboží</th>
										<th class="text-left">Datum od</th>
										<th class="text-left">Datum do</th>
										<th class="text-center">Cena(Kč/den)</th>
									</tr>
								</thead>
								<tbody>

<?php
	echo $cartTable2;
?>

								</tbody>
								<tfoot> <!--TODO předělat tfoot do div -->
									<tr>
										<td></td>
										<td></td>
										<td><h5>Vratná záloha</h5><h3>Cena celkem</h3></td>
										<td class="text-right"><h5><?echo $zalohaCelkem;?>,-</strong></h5><h3><?echo $cenaCelkem;?>,-</h3></td>
									</tr>
								</tfoot>
							</table>

							<hr>

							<div class="col-sm-2 form-group">
								<p><strong>Jméno:</strong></p>
								<p><strong>Email:</strong></p>
								<p><strong>Telefon:</strong></p>
								<p><strong>Ulice:</strong></p>
								<p><strong>Město:</strong></p>
								<p><strong>PSČ:</strong></p>
							</div>
							<div class="col-sm-4 form-group">

<?php
	$sqlQuery = 'SELECT jmeno, prijmeni, email, telefon, ulice, mesto, psc FROM Uzivatel WHERE email=\'' . $_SESSION["email"] . '\'';
	$result = mysql_query($sqlQuery);
	$row = mysql_fetch_row($result);

	echo '<p>' . $row[0] . ' ' . $row[1] . '</p>
		<p>' . $row[2] . '</p>
		<p>' . $row[3] . '</p>
		<p>' . $row[4] . '</p>
		<p>' . $row[5] . '</p>
		<p>' . $row[6] . '</p>';
?>

							</div>
							<div class="col-sm-2 form-group">
								<p><strong>Způsob platby:</strong></p>
								<p><strong>Poznámky:</strong></p>
							</div>
							<div class="col-sm-4 form-group">
								<p id="payFinal"></p>
								<p id="notes"></p>
							</div>
						</div>

						<hr>

						<div class="row">
							<div class="col-sm-2"> </div>
							<div class="col-sm-2 form-group">
								<button id="back2" type="button" class="btn btn-default btn-lg">
									<span class="glyphicon glyphicon-step-backward"></span>
									<span>Zpět</span>
								</button>
							</div>
							<div class="col-sm-4"> </div>
							<div class="col-sm-2 form-group">
								<form>
									<input type="hidden" name="reserve" value="reserve" />
									<button id="finish" type="submit" class="btn btn-lg btn-success">
										<span>Potvrdit a rezervovat</span>
										<span class="glyphicon glyphicon-step-forward"></span>
									</button>
								</form>
							</div>
						</div>
					</div>
				</form>
			</div>
		</div>


<?php
	include("terms.php");
	include("footer.php");
?>
