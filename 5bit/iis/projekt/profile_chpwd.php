<?
	include("check_login.php");
	include("header.php"); 
?>

<header>
	<nav class="main-menu">
		<a href="home.php">Home</a>
		<a href="overview_wizards.php">Přehled</a>
		<a href="#" class="selected"><? echo $_SESSION['login']?></a>
		<form method="get">
			<input type="submit" name="logout" value="Odhlásit se">
		</form>
	</nav>
	<nav class="sub-menu">
		<a href="profile_overview.php">Přehled</a>
		<a href="profile_edit.php">Upravit profil</a>
		<a href="#" class="selected">Změnit heslo</a>
	</nav>
</header>

<div class="content">
	<?php
		if ($_SESSION['login'] == "admin")
			echo '<h1>Účet administrátora nelze upravovat</h1>';
		
		else
		{
			echo	'<h1>Změnit heslo</h1>
					<form action="profile_chpwd.php" method="post">';
						
			if(isset($_POST['chpwd']))
			{
				// pripojeni k databazi a zjisteni hesla aktualniho uzivatele
				include("connect_to_db.php");
				$sqlQuery = 'SELECT heslo FROM Kouzelnik WHERE jmeno_kouz = \'' . $_SESSION['login'] . '\'';
				$result = mysql_query($sqlQuery, $db);
				
				$heslo = mysql_fetch_array($result);
				$heslo = $heslo[0];
				
				// spravne heslo
				if ( $_POST['pwd_old'] == $heslo )
				{
					$error = FALSE;
					$form = '<div class="text_input">
								<label for="pwd_old">Staré heslo:</label>
								<input type="password" name="pwd_old" value="' . $_POST['pwd_old'] . '">
							</div>
							<div class="text_input">
								<label for="pwd_new0">Nové heslo:</label>';
							
					// zpracovani noveho hesla
					$pwd0 = $_POST['pwd_new0'];
					$pwd1 = $_POST['pwd_new1'];
					if (strlen($pwd0) < 6)
					{
						$error = TRUE;
						$form .= '	<input type="password" name="pwd_new0" value="' . $_POST['pwd_new0'] . '" class="bad-input"></div>
								<span class="err-msg">Heslo musí obsahovat alespoň 6 znaků.</span>';
					}
					else
						$form .= '	<input type="password" name="pwd_new0" value="' . $_POST['pwd_new0'] . '"></div>';
						
					$form .='<div class="text_input">
								<label for="pwd_new1">Potvrdit heslo:</label>';

					if ($pwd0 != $pwd1)
					{
						$error = TRUE;
						$form .= '<input type="password" name="pwd_new1" class="bad-input">
							</div><span class="err-msg">Zadaná hesla se neshodují.</span>';
					}
					else
						$form .= '<input type="password" name="pwd_new1"></div>';
					
					if ($error)
					{
						echo $form . '<input type="submit" value="Změnit" class="button" name="chpwd">';
					}
					else
					{
						// upraveni hesla v databazi
						$sqlQuery = 'UPDATE Kouzelnik SET heslo=\''. $_POST['pwd_new0'] .'\'WHERE jmeno_kouz = \'' . $_SESSION['login'] . '\'';
						$result = mysql_query($sqlQuery, $db);
				
						echo '<p>Změna hesla proběhla úspěšně!</p>
							  <a href="profile_chpwd.php" class="button">Rozumím</a>';
					}
					mysql_close($db);
				}
				else
				{
					echo'<div class="text_input">
							<label for="pwd_old">Staré heslo:</label>
							<input type="password" name="pwd_old" class="bad-input">
						</div>
						<span class="err-msg">Nesprávné heslo.</span>

						<div class="text_input">
							<label for="pwd_new0">Nové heslo:</label>
							<input type="password" name="pwd_new0">
						</div>
						<div class="text_input">
							<label for="pwd_new1">Potvrdit heslo:</label>
							<input type="password" name="pwd_new1">
						</div>
						<input type="submit" value="Změnit" class="button" name="chpwd">';
				}
			}
			
			else
			{	// vyhozi zobrazeni formulare
				echo'	<div class="text_input">
							<label for="pwd_old">Staré heslo:</label>
							<input type="password" name="pwd_old">
						</div>
						<div class="text_input">
							<label for="pwd_new0">Nové heslo:</label>
							<input type="password" name="pwd_new0">
						</div>
						<div class="text_input">
							<label for="pwd_new1">Potvrdit heslo:</label>
							<input type="password" name="pwd_new1">
						</div>
						<input type="submit" value="Změnit" class="button" name="chpwd">';
			}

			echo '</form>';
		}
	?>
</div>

<? include("footer.php"); ?>
