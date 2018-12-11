-----------------------------------------------
-----   IDS: SQL Database
-----   Autor: Tomas Aubrecht (xaubre02)
-----   Datum: 25.3.2017
-----------------------------------------------

-----------------------------------------------
------------- Odstraneni tabulek --------------
DROP TABLE Kouzlo CASCADE CONSTRAINT;
DROP TABLE Svitek CASCADE CONSTRAINT;
DROP TABLE Kouzelnik CASCADE CONSTRAINT;
DROP TABLE Grimoar CASCADE CONSTRAINT;
DROP TABLE Nabiti CASCADE CONSTRAINT;
DROP TABLE Ded_misto CASCADE CONSTRAINT;
DROP TABLE Element CASCADE CONSTRAINT;
DROP TABLE Kouz_mel CASCADE CONSTRAINT;
DROP TABLE Kouz_umi CASCADE CONSTRAINT;
DROP TABLE Je_v_syn CASCADE CONSTRAINT;
DROP TABLE Grim_obsah CASCADE CONSTRAINT;
DROP TABLE Ma_ved_elem CASCADE CONSTRAINT;
DROP TABLE Nab_ded_mista CASCADE CONSTRAINT;

-----------------------------------------------
------------- Odstraneni sekvenci -------------
DROP SEQUENCE sudaID;
DROP SEQUENCE lichaID;

-----------------------------------------------
-------------- Vytvoreni tabulek --------------
CREATE TABLE Kouzelnik(
jmeno_kouz VARCHAR(100) NOT NULL,
mana INTEGER NOT NULL,
uroven VARCHAR(2) NOT NULL,

CONSTRAINT pk_kouzelnik PRIMARY KEY (jmeno_kouz)
);

CREATE TABLE Element(
druh VARCHAR(32) NOT NULL,
specializace VARCHAR(32) NOT NULL,
barva VARCHAR(32) NOT NULL,

CONSTRAINT pk_element PRIMARY KEY (druh)
);

CREATE TABLE Kouzlo(
nazev_kouz VARCHAR(100) NOT NULL,
typ VARCHAR(100) NOT NULL,
obtiznost VARCHAR(10) NOT NULL,
sila VARCHAR(100),
druh VARCHAR(32),

CONSTRAINT pk_kouzlo PRIMARY KEY (nazev_kouz),
CONSTRAINT fk_element FOREIGN KEY (druh) REFERENCES Element(druh)
);

CREATE TABLE Svitek(
id_svitku INTEGER NOT NULL,
nazev VARCHAR(100) NOT NULL,
nazev_kouz VARCHAR(100) NOT NULL,
jmeno_kouz VARCHAR(100),

CONSTRAINT pk_svitek PRIMARY KEY (id_svitku),
CONSTRAINT fk_sv_k FOREIGN KEY (nazev_kouz) REFERENCES Kouzlo(nazev_kouz),
CONSTRAINT fk_sv_klnik FOREIGN KEY (jmeno_kouz) REFERENCES Kouzelnik(jmeno_kouz)
);

CREATE TABLE Grimoar(
nazev_grim VARCHAR(100) NOT NULL,
mana INTEGER NOT NULL,
druh VARCHAR(32),
jmeno_kouz VARCHAR(100),

CONSTRAINT pk_Grimoar PRIMARY KEY (nazev_grim),
CONSTRAINT fk_grim_el FOREIGN KEY (druh) REFERENCES Element(druh),
CONSTRAINT fk_grim_ma FOREIGN KEY (jmeno_kouz) REFERENCES Kouzelnik(jmeno_kouz),
CONSTRAINT ch_mana CHECK(mana >= 0)
);

CREATE TABLE Nabiti(
id_nabiti INTEGER NOT NULL,
datum DATE NOT NULL,
nazev_grim VARCHAR(100),
jmeno_kouz VARCHAR(100),

CONSTRAINT pk_Nabiti PRIMARY KEY (id_nabiti),
CONSTRAINT fk_nab_kouz FOREIGN KEY (jmeno_kouz) REFERENCES Kouzelnik(jmeno_kouz),
CONSTRAINT fk_nab_grim FOREIGN KEY (nazev_grim) REFERENCES Grimoar(nazev_grim)
);

CREATE TABLE Ded_misto(
jmeno VARCHAR(100) NOT NULL,
druh VARCHAR(32) NOT NULL,
procento INTEGER,

CONSTRAINT pk_Ded_misto PRIMARY KEY (jmeno),
CONSTRAINT fk_ded_m_el FOREIGN KEY (druh) REFERENCES Element(druh),
CONSTRAINT ch_perc1 CHECK(procento >= 0),
CONSTRAINT ch_perc2 CHECK(procento <= 100)
);

CREATE TABLE Kouz_mel(
jmeno_kouz VARCHAR(100) NOT NULL,
nazev_grim VARCHAR(100) NOT NULL,
mel_od DATE NOT NULL,
mel_do DATE NOT NULL,

CONSTRAINT pk_Kouz_mel PRIMARY KEY (jmeno_kouz, nazev_grim),
CONSTRAINT fk_kz_mel_k FOREIGN KEY (jmeno_kouz) REFERENCES Kouzelnik(jmeno_kouz),
CONSTRAINT fk_kz_mel_g FOREIGN KEY (nazev_grim) REFERENCES Grimoar(nazev_grim),
CONSTRAINT ch_date CHECK(mel_od <= mel_do)
);

CREATE TABLE Kouz_umi(
jmeno_kouz VARCHAR(100) NOT NULL,
nazev_kouz VARCHAR(100) NOT NULL,

CONSTRAINT pk_Kouz_umi PRIMARY KEY (jmeno_kouz, nazev_kouz),
CONSTRAINT fk_k_umi_k FOREIGN KEY (jmeno_kouz) REFERENCES Kouzelnik(jmeno_kouz),
CONSTRAINT fk_k_umi_klnik FOREIGN KEY (nazev_kouz) REFERENCES Kouzlo(nazev_kouz)
);

CREATE TABLE Je_v_syn(
jmeno_kouz VARCHAR(100) NOT NULL,
druh VARCHAR(32) NOT NULL,

CONSTRAINT pk_Je_v_syn PRIMARY KEY (jmeno_kouz, druh),
CONSTRAINT fk_je_v_s_k FOREIGN KEY (jmeno_kouz) REFERENCES Kouzelnik(jmeno_kouz),
CONSTRAINT fk_je_v_s_el FOREIGN KEY (druh) REFERENCES Element(druh)
);

CREATE TABLE Grim_obsah(
nazev_kouz VARCHAR(100) NOT NULL,
nazev_grim VARCHAR(100) NOT NULL,

CONSTRAINT pk_Grim_obsah PRIMARY KEY (nazev_kouz, nazev_grim),
CONSTRAINT fk_grim_ob_k FOREIGN KEY (nazev_kouz) REFERENCES Kouzlo(nazev_kouz),
CONSTRAINT fk_grim_ob_g FOREIGN KEY (nazev_grim) REFERENCES Grimoar(nazev_grim)
);

CREATE TABLE Ma_ved_elem(
nazev_kouz VARCHAR(100) NOT NULL,
druh VARCHAR(32) NOT NULL,

CONSTRAINT pk_Ma_ved_elem PRIMARY KEY (nazev_kouz, druh),
CONSTRAINT fk_ma_ve_el_k FOREIGN KEY (nazev_kouz) REFERENCES Kouzlo(nazev_kouz),
CONSTRAINT fk_ma_ve_el_e FOREIGN KEY (druh) REFERENCES Element(druh)
);

CREATE TABLE Nab_ded_mista(
Nabiti INTEGER NOT NULL,
misto VARCHAR(100) NOT NULL,

CONSTRAINT pk_n_d_misto PRIMARY KEY (Nabiti, misto),
CONSTRAINT fk_nab_d_m_n FOREIGN KEY (Nabiti) REFERENCES Nabiti(id_nabiti),
CONSTRAINT fk_nab_d_m_m FOREIGN KEY (misto) REFERENCES Ded_misto(jmeno)
);

------------------------------------------------------------
---------- Vytvoreni sekvenci pro pridelovani ID -----------
--- pouze suda cisla ID
CREATE SEQUENCE sudaID
START WITH 1000
INCREMENT BY 2;

--- pouze licha cisla ID
CREATE SEQUENCE lichaID
START WITH 1001
INCREMENT BY 2;

--- nastavení formátu času
ALTER session SET nls_date_format = 'YYYY-MM-DD HH24:MI:SS';

------------------------------------------------------------
------------ TRIGGER pro pridelovani ID svitku -------------
CREATE OR REPLACE TRIGGER priradIDsvitku
	BEFORE INSERT ON Svitek
	FOR EACH ROW
BEGIN
	:new.id_svitku := sudaID.nextval; --- priradi nasledujici hodnotu
END priradIDsvitku;
/
------------------------------------------------------------
------------ TRIGGER pro pridelovani ID nabiti -------------
CREATE OR REPLACE TRIGGER priradIDnabiti
	BEFORE INSERT ON Nabiti
	FOR EACH ROW
BEGIN
	:new.id_nabiti := lichaID.nextval; --- priradi nasledujici hodnotu
END priradIDnabiti;
/

------------------------------------------------------------
------- TRIGGER pro kontrolu vedlejších elementů -----------
CREATE OR REPLACE TRIGGER kontrolaVedElem
	BEFORE INSERT ON Ma_ved_elem
	FOR EACH ROW
DECLARE
	kouzlo Ma_ved_elem.nazev_kouz%TYPE;
  typ VARCHAR(100);
BEGIN
	kouzlo := :new.nazev_kouz;
	SELECT typ INTO typ FROM Kouzlo WHERE nazev_kouz = kouzlo;
	IF typ <> 'Obranné' THEN
		Raise_Application_Error (-20030, 'Pouze obranná kouzla mohou mít vedlejší elementy!');
  END IF;
END kontrolaVedElem;
/

------------------------------------------------------------
-------------- TRIGGER pro kontrolu svitků -----------------
CREATE OR REPLACE TRIGGER kontrolaSvitku
	BEFORE INSERT ON Svitek
	FOR EACH ROW
DECLARE
	kouzlo Svitek.nazev_kouz%TYPE;
  typ VARCHAR(100);
BEGIN
	kouzlo := :new.nazev_kouz;
	SELECT typ INTO typ FROM Kouzlo WHERE nazev_kouz = kouzlo;
	IF typ <> 'Útočné' THEN
		Raise_Application_Error (-20030, 'Svitek může obsahovat pouze útočná kouzla!');
  END IF;
END kontrolaSvitku;
/

------------------------------------------------------------
---------- TRIGGER pro kontrolu many kouzelnika ------------
CREATE OR REPLACE TRIGGER kontrolaMany
	BEFORE INSERT ON Kouzelnik
	FOR EACH ROW
DECLARE
	uroven Kouzelnik.uroven%TYPE;
	mana   Kouzelnik.mana%TYPE;
BEGIN
	uroven := :new.uroven;
	mana   := :new.mana;
	IF mana < 0 THEN
		Raise_Application_Error (-20012, 'Kouzelník nemůže mít zápornou manu!');
	END IF;
	IF mana > 100000 THEN
		Raise_Application_Error (-20011, 'Žádný kouzelník není schOPEN mít takové množství many!');
	END IF;
	IF(uroven = 'S' AND mana > 50000) OR 
		(uroven = 'A' AND mana > 30000) OR
		(uroven = 'B' AND mana > 20000) OR
		(uroven = 'C' AND mana > 10000) OR
		(uroven = 'D' AND mana > 5000)  OR
		(uroven = 'E' AND mana > 2000)  THEN
		Raise_Application_Error (-20010, 'Kouzelník má více many, než mu jeho úroveň dovoluje!');
  END IF;
END kontrolaMany;
/

------------------------------------------------------------
------------ TRIGGER pro kontrolu času nabití --------------
CREATE OR REPLACE TRIGGER kontrolaCasu 
	BEFORE INSERT ON Nabiti 
	FOR EACH ROW
DECLARE
	cas VARCHAR(6);
	den VARCHAR(2);
BEGIN
	cas := TO_CHAR(:new.datum, 'HH24MISS');
	den := TO_CHAR(:new.datum, 'DD');
	IF cas < '000000' OR cas > '030000' THEN
		Raise_Application_Error (-20020, 'Nabití je možné provést pouze od půlnoci do tří hodin!');
	END IF;
	IF den != '01' THEN
		Raise_Application_Error (-20022, 'Nabití je možné provést pouze první den v novém měsíci!');
	END IF;
END kontrolaCasu;
/

--------------------------------------------------------------------------------------------------
------------------------------------------ PROCEDURA ---------------------------------------------
--- Vypíše procentuální zastoupení elementu mezi kouzly a zároveň jeho průměrnou míru prosakování.
CREATE OR REPLACE PROCEDURE procElementu(elem IN VARCHAR)
is
	cursor magic is SELECT * FROM Kouzlo NATURAL LEFT JOIN Element;
	spell magic%ROWTYPE;
	kouzel INTEGER; --- celkový počet kouzel
	danych INTEGER; --- počet kouzel s daným elementem
	proc INTEGER;   --- průměrné procento prosakování
BEGIN
	--- inicializace
	kouzel := 0;
	danych := 0;
	SELECT AVG(procento) INTO proc FROM Element NATURAL LEFT JOIN Ded_misto WHERE druh = elem GROUP BY druh;
	IF proc IS NULL THEN
		proc := 0;
	END IF;
	OPEN magic;
	LOOP
		FETCH magic INTO spell;
		EXIT WHEN magic%NOTFOUND;
		--- pokud se jedná o kouzlo s hledaným elementem
		IF spell.druh = elem THEN
			danych := danych + 1;
		END IF;
		kouzel := kouzel + 1;
	END LOOP;
	CLOSE magic;
	dbms_output.put_line('Celkové zastoupení elementu "' || elem || '" mezi kouzly je ' || ROUND(danych * 100 / kouzel) || '% a jeho průměrná míra prosakování je ' || proc || '%.');
EXCEPTION
	--- pokud nebyl daný element nalezen
	WHEN NO_DATA_FOUND THEN
		dbms_output.put_line('Element "' || elem || '" nebyl nalezen!');
	--- ostatní chyby
	WHEN OTHERS THEN
		Raise_Application_Error (-20025, 'Vyskytla se chyba!');
END;
/

------------------------------------------------------------------------------------------------------
-------------------------------------------- PROCEDURA -----------------------------------------------
--- Vypíše procentuální vyjádření vzácnosti daného kouzla.
--- Minimum: 0% je nejběžnější - každý kouzelník ho umí, je v každém grimoáru a v každém svitku.
--- Maximum: 100% je nejvzácnější - žádný kouzelník ho neumí a žádný grimoár ani svitek ho neobsahuje.
CREATE OR REPLACE PROCEDURE vzacnostKouzla(kouz IN VARCHAR)
IS
	hledane Kouzlo.nazev_kouz%TYPE; --- jmeon hledaneho kouzla
	svitku INTEGER;     --- celkový počet svitků
	obsahS INTEGER;     --- počet svitků s daným kouzlem
	obsahG INTEGER;     --- počet grimoárů s daným kouzlem
	grimoaru INTEGER;   --- celkový počet grimoárů
	kouzelniku INTEGER; --- celkový počet kouzelníků
	umi INTEGER;        --- počet kouzelniku, kteří umí dané kouzlo
	--- pruměry daných výskytů
	prumSvit FLOAT;
	prumGrim FLOAT;
	prumKouz FLOAT;
BEGIN
	--- inicializace
	svitku := 0;
	obsahS := 0;
	obsahG := 0;
	grimoaru := 0;
	kouzelniku := 0;
	umi := 0;
  
	SELECT nazev_kouz INTO hledane FROM Kouzlo WHERE nazev_kouz = kouz; --- kontrola, zdali kouzlo existuje
	SELECT COUNT(*) INTO svitku FROM Svitek;
	SELECT COUNT(*) INTO obsahS FROM Svitek WHERE nazev_kouz = kouz;
	SELECT COUNT(*) INTO grimoaru FROM Grimoar;
	SELECT COUNT(*) INTO obsahG FROM Grimoar NATURAL JOIN Grim_obsah NATURAL JOIN Kouzlo WHERE nazev_kouz = kouz;
	SELECT COUNT(*) INTO kouzelniku FROM Kouzelnik;
	SELECT COUNT(*) INTO umi FROM Kouzelnik NATURAL JOIN Kouz_umi NATURAL JOIN Kouzlo WHERE nazev_kouz = kouz;
	
	--- ošetření dělení nulou pro výskyt daných svitků
	IF svitku = 0 THEN
		prumSvit := 0;
	ELSE
		prumSvit := 100 - (obsahS * 100 / svitku);
	END IF;
	--- ošetření dělení nulou pro výskyt daných grimoárů
	IF grimoaru = 0 THEN
		prumGrim := 0;
	ELSE
		prumGrim := 100 - (obsahG * 100 / grimoaru);
	END IF;
	--- ošetření dělení nulou pro výskyt daných kouzelníků
	IF kouzelniku = 0 THEN
		prumKouz := 0;
	ELSE
		prumKouz := 100 - (umi * 100 / kouzelniku);
	END IF;
	dbms_output.put_line('Vzácnost kouzla "' || hledane || '" je ' || ROUND((prumSvit + prumGrim + prumKouz) / 3) || '%.');
EXCEPTION
	--- pokud nebylo dané kouzlo nalezeno
	WHEN NO_DATA_FOUND THEN
		dbms_output.put_line('Kouzlo "' || kouz || '" nebylo nalezeno!');
	--- ostatní chyby
	WHEN OTHERS THEN
		Raise_Application_Error (-20025, 'Vyskytla se chyba!');
END;
/

----------------------------------------------------------------------
----------------------- Naplneni tabulek daty ------------------------
INSERT INTO Kouzelnik VALUES('Baba Yaga', 1500, 'E');
INSERT INTO Kouzelnik VALUES('Merlin', 8900, 'C');
INSERT INTO Kouzelnik VALUES('Gandalf', 20000, 'B');
INSERT INTO Kouzelnik VALUES('Saruman', 95000, 'SS');

INSERT INTO Element VALUES('Oheň', 'Útok', 'Červená');
INSERT INTO Element VALUES('Vzduch', 'Útok', 'Bílá');
INSERT INTO Element VALUES('Voda', 'Podpora', 'Modrá');
INSERT INTO Element VALUES('Země', 'Obrana', 'Zelená');

INSERT INTO Kouzlo VALUES('Fireball', 'Útočné', 'obtížné', 'silné', 'Oheň');
INSERT INTO Kouzlo VALUES('Fire lance', 'Útočné', 'Snadné', 'silné', 'Oheň');
INSERT INTO Kouzlo VALUES('Fire-block', 'Obranné', 'snadné', 'Silné', 'Oheň');
INSERT INTO Kouzlo VALUES('Droplets of purity', 'Obranné', 'obtížné', 'Velmi silné', 'Voda');
INSERT INTO Kouzlo VALUES('Death from Shadows', 'Útočné', 'obtížné', 'Velmi silné', 'Vzduch');

INSERT INTO Svitek (nazev, nazev_kouz, jmeno_kouz) VALUES('Svitek pánů z Mordoru', 'Fireball', 'Saruman');
INSERT INTO Svitek (nazev, nazev_kouz, jmeno_kouz) VALUES('Fireball', 'Fireball', 'Saruman');
INSERT INTO Svitek (nazev, nazev_kouz, jmeno_kouz) VALUES('Fire games', 'Fireball', 'Gandalf');

INSERT INTO Grimoar VALUES('Velká kniha čar a kouzel', '10000', 'Oheň', 'Baba Yaga');

INSERT INTO Nabiti (datum, nazev_grim, jmeno_kouz) VALUES(TO_DATE('1325-05-01 0:00:00', 'YYYY-MM-DD HH24:MI:SS'), 'Velká kniha čar a kouzel', 'Baba Yaga');
INSERT INTO Nabiti (datum, nazev_grim, jmeno_kouz) VALUES(TO_DATE('1121-12-01 02:59:59', 'YYYY-MM-DD HH24:MI:SS'), 'Velká kniha čar a kouzel', 'Merlin');

INSERT INTO Ded_misto VALUES('Mount Doom', 'Oheň', '80');
INSERT INTO Ded_misto VALUES('Ereldin', 'Oheň', '15');
INSERT INTO Ded_misto VALUES('Mariánský příkop', 'Voda', '100');

INSERT INTO Kouz_mel VALUES('Gandalf', 'Velká kniha čar a kouzel', TO_DATE('1200-08-09 07:45:00', 'YYYY-MM-DD HH24:MI:SS'), TO_DATE('1256-06-05 10:20:00', 'YYYY-MM-DD HH24:MI:SS'));

INSERT INTO Kouz_umi VALUES('Saruman','Fireball');
INSERT INTO Kouz_umi VALUES('Saruman','Fire-block');
INSERT INTO Kouz_umi VALUES('Merlin','Fireball');
INSERT INTO Kouz_umi VALUES('Baba Yaga','Fireball');
INSERT INTO Kouz_umi VALUES('Saruman','Fire lance');
INSERT INTO Kouz_umi VALUES('Gandalf','Fireball');
INSERT INTO Kouz_umi VALUES('Gandalf','Fire-block');
INSERT INTO Kouz_umi VALUES('Merlin','Droplets of purity');

INSERT INTO Je_v_syn VALUES('Gandalf','Voda');
INSERT INTO Je_v_syn VALUES('Gandalf','Oheň');
INSERT INTO Je_v_syn VALUES('Gandalf','Vzduch');
INSERT INTO Je_v_syn VALUES('Saruman','Oheň');
INSERT INTO Je_v_syn VALUES('Saruman','Vzduch');

INSERT INTO Grim_obsah VALUES('Fireball', 'Velká kniha čar a kouzel');
INSERT INTO Grim_obsah VALUES('Fire lance', 'Velká kniha čar a kouzel');
INSERT INTO Grim_obsah VALUES('Droplets of purity', 'Velká kniha čar a kouzel');

INSERT INTO Ma_ved_elem VALUES('Fire-block', 'Voda');

INSERT INTO Nab_ded_mista VALUES('1001', 'Ereldin');
INSERT INTO Nab_ded_mista VALUES('1003', 'Mount Doom');

------------------------------------------------------------------------
------------------------- Vyber dat z tabulek --------------------------

--- Zobrazí veškeré svitky, které jednotlivý kouzelníci aktuálně vlastní
SELECT jmeno_kouz as Kouzelnik, nazev as Svitek
FROM Kouzelnik JOIN Svitek USING (jmeno_kouz);

--- Zobrazí jméno kouzelníka a data, kdy provedl nějaké nabití
SELECT jmeno_kouz as Kouzelnik, datum
FROM Kouzelnik JOIN Nabiti USING (jmeno_kouz);

--- Zobrazi hlavni element kouzel
SELECT nazev_kouz as Kouzlo, druh as Element
FROM Kouzlo JOIN Element USING (druh);

--- Zobrazi kouzelniky, kteri vlastni nejaky grimoar a zaroven zobrazi hlavni element grimoaru
SELECT jmeno_kouz as Kouzelnik, nazev_grim as Grimoar, druh as Element
FROM Kouzelnik JOIN Grimoar USING (jmeno_kouz) JOIN Element USING (druh);

--- Zobrazi grimoar, jeho kouzla a jejich hlavni element
SELECT nazev_grim as Grimoar, nazev_kouz as Kouzlo, druh as Element 
FROM Kouzlo NATURAL JOIN Element NATURAL JOIN GRIM_OBSAH NATURAL LEFT JOIN Grimoar;

--- Zobrazí počet grimoarů, které kouzelník vlastnil
SELECT jmeno_kouz as Kouzelnik, COUNT(nazev_grim) as Mel_grimoaru
FROM Kouzelnik NATURAL LEFT JOIN kouz_mel
GROUP BY jmeno_kouz
ORDER BY Mel_grimoaru DESC;

--- Zobrazí počet kouzel, které kouzelník umi
SELECT jmeno_kouz as Kouzelnik, COUNT(nazev_kouz) as Umi_kouzel
FROM Kouzelnik NATURAL LEFT JOIN kouz_umi
GROUP BY jmeno_kouz
ORDER BY Umi_kouzel DESC;

--- Zobrazi kouzelniky, kteri jsou v synergii s vodou
SELECT jmeno_kouz as Kouzelnik
FROM Kouzelnik K
WHERE EXISTS (SELECT * FROM je_v_syn S WHERE K.jmeno_kouz=S.jmeno_kouz AND S.druh='Voda');

--- Zobrazi kouzelniky, kteri nekdy provedli nejake nabiti
SELECT jmeno_kouz as Kouzelnik
FROM Kouzelnik K
WHERE EXISTS (SELECT * FROM Nabiti N WHERE n.jmeno_kouz = k.jmeno_kouz);

--- Zobrazí všechna kouzla, která jsou obsažena ve Velké knize čar a kouzel
SELECT *
FROM Kouzlo
WHERE nazev_kouz IN (SELECT nazev_kouz FROM Grimoar NATURAL JOIN grim_obsah WHERE nazev_grim = 'Velká kniha čar a kouzel')
ORDER BY nazev_kouz;

---------------------------------------------------
---------------- Vyvolání procedur ----------------
SET serveroutput ON;
--- procElementu
exec procElementu('Voda');
exec procElementu('Oheň');
exec procElementu('Vzduch');
exec procElementu('CHYBA');
--- vzacnostKouzla
exec vzacnostKouzla('Fireball');
exec vzacnostKouzla('Droplets of purity');
exec vzacnostKouzla('Death from Shadows');
exec vzacnostKouzla('CHYBA');

---------------------------------------------------
-------------- EXPLAIN PLAN a INDEX ---------------
EXPLAIN PLAN FOR
SELECT specializace, uroven, COUNT(jmeno_kouz) as Synergie
FROM Element NATURAL LEFT JOIN Je_v_syn NATURAL LEFT JOIN Kouzelnik
GROUP BY specializace, uroven
ORDER BY Synergie DESC;
SELECT * FROM TABLE(DBMS_XPLAN.display);

CREATE INDEX mujIndex ON Element (specializace);

EXPLAIN PLAN FOR
SELECT /*+ INDEX(Element mujIndex)*/ specializace, uroven, COUNT(jmeno_kouz) as Synergie
FROM Element NATURAL LEFT JOIN Je_v_syn NATURAL LEFT JOIN Kouzelnik
GROUP BY specializace, uroven
ORDER BY Synergie DESC;
SELECT * FROM TABLE(dbms_xplan.display);

--------------------------------------------------
--------------- MATERIALIZED VIEW -----------------
CREATE MATERIALIZED VIEW LOG ON Kouzelnik WITH PRIMARY KEY, ROWID(uroven)INCLUDING NEW VALUES;
DROP  MATERIALIZED VIEW KouzelnikMV;
CREATE MATERIALIZED VIEW KouzelnikMV
NOLOGGING --- databáze nezaznamenává operace s pohledem
CACHE --- databáze postupne optimalizuje čtení z pohledu 
BUILD IMMEDIATE --- databáze naplní pohled ihned po jeho vytvoření
REFRESH FAST ON COMMIT --- databáze aktualizuje pohled dle logů master tabulek po commitu
ENABLE QUERY REWRITE --- bude používán optimalizátorem
AS SELECT jmeno_kouz as Kouzelník, uroven as Úroveň
FROM Kouzelnik
ORDER BY jmeno_kouz;

SELECT * FROM KouzelnikMV;
INSERT INTO Kouzelnik VALUES('Wizard of OZ', 45000, 'S');
COMMIT;
SELECT * FROM KouzelnikMV;

---------------------------------------------------
----------- Definice pristupovych prav ------------
--- Tabulky
GRANT ALL ON Kouzlo TO rychly;
GRANT ALL ON Svitek TO rychly;
GRANT ALL ON Grimoar TO rychly;
GRANT ALL ON Nabiti TO rychly;
GRANT ALL ON Ded_misto TO rychly;
GRANT ALL ON Element TO rychly;
GRANT ALL ON Grim_obsah TO rychly;
GRANT ALL ON Ma_ved_elem TO rychly;
GRANT ALL ON Nab_ded_mista TO rychly;

--- Procedury
GRANT EXECUTE ON procElementu TO rychly;
GRANT EXECUTE ON vzacnostKouzla TO rychly;

--- Materializované pohledy
GRANT ALL ON KouzelnikMV TO rychly;