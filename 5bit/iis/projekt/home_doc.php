<?
	include("check_login.php");
	include("header.php"); 
?>

<header>
	<nav class="main-menu">
		<a href="#" class="selected">Home</a>
		<a href="overview_wizards.php">Přehled</a>
		<a href="profile_overview.php"><? echo $_SESSION['login']?></a>
		<form method="get">
			<input type="submit" name="logout" value="Odhlásit se">
		</form>
	</nav>
	<nav class="sub-menu">
		<a href="home.php">Informace</a>
		<a href="#" class="selected">Dokumentace</a>
	</nav>
</header>

<div class="content">
	<h1>Dokumentace</h1>
	<p>TODO...</p>
</div>

<? include("footer.php"); ?>