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
		<a href="#" class="selected">Informace</a>
	</nav>
</header>

<div class="content">
	<h1>Magický svět</h1>
	<p>&nbsp&nbsp&nbsp&nbsp&nbsp Magie v kouzelnickém světě je členěna podle <a href="overview_elements.php">elementů</a> (voda, oheň, vzduch, ...), které mají různé specializace (obrana, útok, podpora) a různé barvy magie. Například ohnivá magie je oranžová, vody je modrá apod.
	<br>&nbsp&nbsp&nbsp&nbsp&nbsp Každé <a href="overview_spells.php">kouzlo</a> má jeden hlavní element, přičemž kouzelník může mít positivní synergii s některým z elementů.
	<br>&nbsp&nbsp&nbsp&nbsp&nbsp U <a href="overview_wizards.php">kouzelníků</a> rovněž evidujeme velikost many, jeho dosaženou úroveň v kouzlení. Předpokládáme klasickou stupnici E, D, C, B, A, S, SS.
	<br>&nbsp&nbsp&nbsp&nbsp&nbsp U jednotlivých kouzel pak jejich úroveň složitosti seslání a sílu. Kouzla mohou být útočná či obranná, přičemž obranná mohou mít navíc 
	i vedlejší elementy. Kouzla mohou být samovolně sesílána, pokud je daný kouzelník umí, nebo lze využit kouzelnických knih, tzv. grimoárů.
	<br>&nbsp&nbsp&nbsp&nbsp&nbsp <a href="overview_spellbooks.php">Grimoáry</a> v sobě seskupují různá kouzla. Uchováváme veškerou historii jejich vlastnictví. Grimoáry mohou obsahovat kouzla různých elementů, 
	nicméně jeden z elementů je pro ně primární. Využívání grimoárů způsobuje ztrátu jejich nabité magie a je nutno je znovu dobít, ale pouze na 
	<a href="overview_ded_places.php">dedikovaných místech</a>, kde prosakuje magie určitých typů element. Míra prosakování magie daného místa je evidována a na daném místě prosakuje právě jeden typ. 
	Toto nabití však nemusí být provedeno vlastníkem, ale i jiným kouzelníkem.
	<br>&nbsp&nbsp&nbsp&nbsp&nbspAlternativním způsobem sesílání magie je pak s využitím <a href="overview_scrolls.php">svitků</a>, který obsahuje právě jedno útočné kouzlo a po jeho použití se rozpadne.</p>
</div>

<? include("footer.php"); ?>