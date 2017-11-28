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
    <style type="text/css">
    	table { border-collapse: collapse; }
    	td, th { border: 1px solid black; padding: 0.3em 0.5em; text-align: left; }
    	dt { font-weight: bold; margin-top: 0.5em; }
    </style>
	
	<h1>Magický svět</h1>

	<dl>
		<dt>Autoři</dt>
		<dd>Tomáš Aubrecht
			<a href="mailto:xaubre02@stud.fit.vutbr.cz">xaubre02@stud.fit.vutbr.cz</a> - Všechno
		</dd>
		<dt>URL aplikace</dt>
		<dd><a href="https://www.stud.fit.vutbr.cz/~xaubre02">https://www.stud.fit.vutbr.cz/~xaubre02/</a></dd>
	</dl>

	<h2>Uživatelé systému pro testování</h2>
	<table>
	<tr><th>Login</th><th>Heslo</th><th>Role</th></tr>
	<tr><td>********</td><td>********</td><td>Administrátor</td></tr>
	<tr><td>Gandalf</td><td>123456</td><td>Kouzelník</td></tr>
	<tr><td>Baba Yaga</td><td>123456</td><td>Kouzelník</td></tr>
	</table>

	<h2>Implementace</h2>
	<p>...</p>

	<h2>Instalace</h2>
	<ul>
		<li>...</li>
		<li>...</li>
		<li>...</li>
	</ul>

	<h2>Známé problémy</h2>
	<p>...</p>

</div>

<? include("footer.php"); ?>