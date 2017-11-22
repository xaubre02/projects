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
		<a href="home_doc.php">Dokumentace</a>
	</nav>
</header>

<div class="content">
	<h1>Vítejte!</h1>
	<p>Magie v kouzelnickém světě je členěna podle elementů (voda, oheň, vzduch, ...), které mají různé specializace (obrana, útok) a různé,
	ale pevně dané barvy magie (např. ohnivá magie je pomerančově oranžová).
	<br>Každé kouzlo má jeden hlavní element, přičemž každý kouzelník má positivní synergii s určitými elementy.
	<br>U kouzelníků rovněž evidujeme velikost many, jeho dosaženou úroveň v kouzlení (předpokládáme klasickou stupnici. E, D, C, B, A, S, SS,).
	<br>U jednotlivých kouzel pak jejich úroveň složitosti seslání a sílu. Kouzla mohou být útočná či obranná, přičemž obranná mohou mít navíc 
	i vedlejší elementy. Kouzla mohou být samovolně sesílána, pokud je daný kouzelník umí, nebo lze využit kouzelnických knih, tzv. grimoárů.
	<br>Grimoáry v sobě seskupují různá kouzla. Uchováváme veškerou historii jejich vlastnictví. Grimoáry mohou obsahovat kouzla různých elementů, 
	nicméně jeden z elementů je pro ně primární. Využívání grimoárů způsobuje ztrátu jejich nabité magie a je nutno je znovu dobít, ale pouze na 
	dedikovaných místech, kde prosakuje magie (míra prosakování magie daného místa je evidována) určitých typů element (předpokládejte, že na daném 
	místě prosakuje právě jeden typ). Toto nabití však nemusí být provedeno vlastníkem, ale i jiným kouzelníkem a zároveň musí být provedeno první 
	den v novém měsíci od půlnoci do tří hodin do rána.
	<br>Alternativním způsobem sesílání magie je pak s využitím svitků, který obsahuje právě jedno útočné kouzlo a po jeho použití se rozpadne.</p>
</div>

<? include("footer.php"); ?>