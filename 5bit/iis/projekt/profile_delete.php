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
		<a href="#" class="selected">Odstranit účet</a>
	</nav>
</header>

<div class="content">
	<h1>Odstranit účet</h1>
	<p>TODO...</p>
</div>

<? include("footer.php"); ?>