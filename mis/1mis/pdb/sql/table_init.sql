/*************** DROPS *************/
DROP TABLE vybeh CASCADE CONSTRAINTS;
DROP TABLE mapovy_objekt CASCADE CONSTRAINTS;
DROP TABLE zamestnanec CASCADE CONSTRAINTS;
DROP TABLE zvire CASCADE CONSTRAINTS;
DROP TABLE druh_zvirete CASCADE CONSTRAINTS;

/************* CREATE TABLES ***********/
CREATE TABLE vybeh (
	cislo_vybehu NUMBER NOT NULL,
	typ VARCHAR(20) NOT NULL,
	validni_od DATE NOT NULL,
	validni_do DATE,
	geometrie SDO_GEOMETRY NOT NULL,

	CONSTRAINT PK_vybeh PRIMARY KEY (cislo_vybehu,validni_od)
);

CREATE TABLE mapovy_objekt (
	cislo_objektu NUMBER NOT NULL,
	typ VARCHAR(20) NOT NULL,
	geometrie SDO_GEOMETRY NOT NULL,
	validni_od DATE NOT NULL,
	validni_do DATE,

	CONSTRAINT CK_typ CHECK (typ IN ('park', 'lake', 'tree', 'path')),
	CONSTRAINT PK_mapovy_objekt PRIMARY KEY (cislo_objektu,validni_od)
);

CREATE TABLE zamestnanec (
	rodne_cislo VARCHAR(10) NOT NULL,
	jmeno VARCHAR(20) NOT NULL,
	prijmeni VARCHAR(20) NOT NULL,
	datum_narozeni DATE NOT NULL,
	validni_od DATE NOT NULL,
	validni_do DATE,

	CONSTRAINT PK_temp_key PRIMARY KEY (rodne_cislo,validni_od)
);

CREATE TABLE druh_zvirete (
	cislo_druhu NUMBER GENERATED ALWAYS AS IDENTITY (START WITH 1 INCREMENT BY 1) PRIMARY KEY,
	druh VARCHAR(20) NOT NULL,
	mnozstviKrmivaProKrmeni NUMERIC(12,3) NOT NULL,   --mnozstvi je v kilogramech
	typKrmiva VARCHAR(30) NOT NULL,
	oblastVyskytu VARCHAR(20),

	CONSTRAINT CK_mnozstvi_kladne CHECK(mnozstviKrmivaProKrmeni > 0)
);

CREATE TABLE zvire (
	cislo_zvirete NUMBER NOT NULL,
	jmeno VARCHAR(20) NOT NULL,
	validni_od DATE NOT NULL,
	validni_do DATE,
	cislo_druhu NUMBER NOT NULL,
	cislo_vybehu NUMBER NOT NULL,

	image ORDSYS.ORDImage,
	image_si ORDSYS.SI_StillImage,
	image_ac ORDSYS.SI_AverageColor,
	image_ch ORDSYS.SI_ColorHistogram,
	image_pc ORDSYS.SI_PositionalColor,
	image_tx ORDSYS.SI_Texture,

	CONSTRAINT FK_zvire_cdruhu FOREIGN KEY (cislo_druhu) REFERENCES druh_zvirete on delete cascade,
	CONSTRAINT PK_zvire PRIMARY KEY (cislo_zvirete,validni_od)
);


/******** META DATA ***********/
-- Vybeh
DELETE FROM USER_SDO_GEOM_METADATA WHERE
	TABLE_NAME = 'VYBEH' AND COLUMN_NAME = 'GEOMETRIE';
INSERT INTO USER_SDO_GEOM_METADATA VALUES (
	'vybeh', 'geometrie',
	SDO_DIM_ARRAY(SDO_DIM_ELEMENT('X', 0, 650, 0.5), SDO_DIM_ELEMENT('Y', 0, 650, 0.5)),
	NULL
);
CREATE INDEX SP_INDEX_vybeh ON Vybeh(geometrie) indextype is MDSYS.SPATIAL_INDEX;

-- mapovy_objekt
DELETE FROM USER_SDO_GEOM_METADATA WHERE
	TABLE_NAME = 'MAPOVY_OBJEKT' AND COLUMN_NAME = 'GEOMETRIE';
INSERT INTO USER_SDO_GEOM_METADATA VALUES (
	'mapovy_objekt', 'geometrie',
	SDO_DIM_ARRAY(SDO_DIM_ELEMENT('X', 0, 650, 0.5), SDO_DIM_ELEMENT('Y', 0, 650, 0.5)),
	NULL
);
CREATE INDEX SP_INDEX_mapovy_objekt ON mapovy_objekt(geometrie) indextype is MDSYS.SPATIAL_INDEX;

-- Trigger overujici validitu geomterie vybehu
CREATE OR REPLACE TRIGGER vybeh_geom_validita_trigger
	BEFORE INSERT ON vybeh
  FOR EACH ROW
BEGIN
	IF(:new.geometrie.ST_IsValid() != 1)THEN
		RAISE_APPLICATION_ERROR (-20000, 'Nevalidní geometrie!');
	END IF;
END spatial_validity_trigger;
/

-- Trigger overujici validitu geomterie mapoveho objektu
CREATE OR REPLACE TRIGGER mapovy_objekt_geom_validita_trigger
	BEFORE INSERT ON mapovy_objekt
  FOR EACH ROW
BEGIN
	IF(:new.geometrie.ST_IsValid() != 1)THEN
		RAISE_APPLICATION_ERROR (-20000, 'Nevalidní geometrie!');
	END IF;
END spatial_validity_trigger;
/

-- Triggery overujici validni casova data
CREATE OR REPLACE TRIGGER trigger_check_date_zam
	BEFORE INSERT OR UPDATE ON zamestnanec
	FOR EACH ROW
BEGIN
	IF(:new.datum_narozeni > SYSDATE) THEN
		RAISE_APPLICATION_ERROR (-20001, 'Datum narozeni nesmi byt v budoucnosti!');
	END IF;
	IF(:new.datum_narozeni > :new.validni_od) THEN
		RAISE_APPLICATION_ERROR (-20002, 'Datum narozeni musi být drive nez datum nastupu do zamestnani!');
	END IF;
	IF(:new.validni_do IS NOT NULL) THEN
		IF(:new.validni_od > :new.validni_do) THEN
			RAISE_APPLICATION_ERROR (-20003, 'Datum nastupu musi byt drive nez datum konce v zamestnani!');
		END IF;
	END IF;
END trigger_check_date_zam;
/

CREATE OR REPLACE TRIGGER trigger_check_date_vybeh
	BEFORE INSERT OR UPDATE ON vybeh
	FOR EACH ROW
BEGIN
	IF(:new.validni_do IS NOT NULL) THEN
		IF(:new.validni_od > :new.validni_do) THEN
			RAISE_APPLICATION_ERROR (-20003, 'Datum vytvoreni musi byt drive nez datum zruseni vybehu!');
		END IF;
	END IF;
END trigger_check_date_vybeh;
/

CREATE OR REPLACE TRIGGER trigger_check_date_mapobj
	BEFORE INSERT OR UPDATE ON mapovy_objekt
	FOR EACH ROW
BEGIN
	IF(:new.validni_do IS NOT NULL) THEN
		IF(:new.validni_od > :new.validni_do) THEN
			RAISE_APPLICATION_ERROR (-20003, 'Datum vytvoreni musi byt drive nez datum zruseni objektu!');
		END IF;
	END IF;
END trigger_check_date_mapobj;
/

CREATE OR REPLACE TRIGGER trigger_check_date_zvire
	BEFORE INSERT OR UPDATE ON zvire
	FOR EACH ROW
BEGIN
	IF(:new.validni_do IS NOT NULL) THEN
		IF(:new.validni_od > :new.validni_do) THEN
			RAISE_APPLICATION_ERROR (-20003, 'Datum umisteni zvirete do vybehu musi byt drive nez datum jeho prestehovani nebo umrti!');
		END IF;
	END IF;
END trigger_check_date_zvire;
/


-- Procedury pro temporalni operace UPDATE a DELETE
CREATE OR REPLACE PROCEDURE updateZvire(n_cislo_zvirete IN NUMBER, n_validni_od IN DATE, n_jmeno IN VARCHAR, n_cislo_druhu IN NUMBER, n_cislo_vybehu IN NUMBER) IS	
BEGIN
	-- pouze update aktualniho zaznamu - moznost vicenasobnych uprav zvirete v jeden den
	IF (n_validni_od = trunc(sysdate)) THEN
		UPDATE zvire SET jmeno = n_jmeno, cislo_druhu = n_cislo_druhu, cislo_vybehu = n_cislo_vybehu
		WHERE cislo_zvirete = n_cislo_zvirete AND validni_do IS NULL;
	ELSE
		-- update stavajicho zaznamu a ukonceni jeho temporalni platnosti
		UPDATE zvire SET validni_do = trunc(sysdate)
		WHERE cislo_zvirete = n_cislo_zvirete AND validni_do IS NULL;
		-- vlozeni noveho zaznamu se zahajenim jeho temporalni platnosti
		INSERT INTO zvire (cislo_zvirete, jmeno, validni_od, validni_do, cislo_druhu, cislo_vybehu, image)
		VALUES (n_cislo_zvirete, n_jmeno, trunc(sysdate), NULL, n_cislo_druhu, n_cislo_vybehu, ORDImage.init());
	END IF;
END;
/

CREATE OR REPLACE PROCEDURE deleteZvire(n_cislo_zvirete IN NUMBER, n_validni_do IN DATE) IS	
BEGIN
	-- nebude provedena operace delete, pouze se nastavi konec platnosti zaznamu vybraneho zvirete
	UPDATE zvire SET validni_do = n_validni_do
	WHERE cislo_zvirete = n_cislo_zvirete AND validni_do IS NULL;
END;
/

CREATE OR REPLACE PROCEDURE updateVybeh(n_cislo_vybehu IN NUMBER, n_validni_od IN DATE, n_typ IN VARCHAR, n_geometrie IN SDO_GEOMETRY) IS	
BEGIN
	-- pouze update aktualniho zaznamu - moznost vicenasobnych uprav vybehu v jeden den
	IF (n_validni_od = trunc(sysdate)) THEN
		UPDATE vybeh SET typ = n_typ, geometrie = n_geometrie
		WHERE cislo_vybehu = n_cislo_vybehu AND validni_do IS NULL;
	ELSE 
		-- update stavajicho zaznamu a ukonceni jeho temporalni platnosti
		UPDATE vybeh SET validni_do = trunc(sysdate)
		WHERE cislo_vybehu = n_cislo_vybehu AND validni_do IS NULL;
		-- vlozeni noveho zaznamu se zahajenim jeho temporalni platnosti
		INSERT INTO vybeh (cislo_vybehu, typ, validni_od, validni_do, geometrie)
		VALUES (n_cislo_vybehu, n_typ, trunc(sysdate), NULL, n_geometrie);
		
		-- update vsech zvirat podle nastavenych temporalnich podminek
		FOR rec IN (SELECT * FROM zvire z WHERE z.cislo_vybehu = n_cislo_vybehu AND z.validni_do IS NULL) 
		LOOP
			-- volani procedury pro update zvirete
			updateZvire(rec.cislo_zvirete, rec.validni_od, rec.jmeno, rec.cislo_druhu, rec.cislo_vybehu);
		END LOOP;
	END IF;
END;
/

CREATE OR REPLACE PROCEDURE deleteVybeh(n_cislo_vybehu IN NUMBER) IS
	any_rows_found NUMBER;
BEGIN
	-- pocet zvirat, ktere jsou aktualne ve vybehu
  SELECT COUNT(*) INTO any_rows_found
  FROM zvire WHERE cislo_vybehu = n_cislo_vybehu AND validni_do IS NULL;
  
	-- pokud se ve vybehu nachazeji nejaka zvirata, neni mozne vybeh smazat
  IF any_rows_found > 0 THEN 
    RAISE_APPLICATION_ERROR (-20004, 'Nelze smazat vybeh, kde se nachazeji zvirata! Nejprve prestehujte zvirata do platneho vybehu.');
  END IF;

	-- nebude provedena operace delete, pouze se nastavi konec platnosti u daneho vybehu
	UPDATE vybeh SET validni_do = trunc(sysdate)
	WHERE cislo_vybehu = n_cislo_vybehu AND validni_do IS NULL;
END;
/

CREATE OR REPLACE PROCEDURE updateMapovyObjekt(n_cislo_objektu IN NUMBER, n_validni_od IN DATE, n_typ IN VARCHAR, n_geometrie IN SDO_GEOMETRY) IS	
BEGIN
	-- pouze update aktualniho zaznamu - moznost vicenasobnych uprav objektu v jeden den
	IF (n_validni_od = trunc(sysdate)) THEN
		UPDATE mapovy_objekt SET geometrie = n_geometrie
		WHERE cislo_objektu = n_cislo_objektu AND validni_do IS NULL;
	ELSE
		-- update stavajicho zaznamu a ukonceni jeho temporalni platnosti
		UPDATE mapovy_objekt SET validni_do = trunc(sysdate)
		WHERE cislo_objektu = n_cislo_objektu AND validni_do IS NULL;
		-- vlozeni noveho zaznamu se zahajenim jeho temporalni platnosti
		INSERT INTO mapovy_objekt (cislo_objektu, typ, validni_od, validni_do,geometrie)
		VALUES (n_cislo_objektu, n_typ, trunc(sysdate), NULL, n_geometrie);
	END IF;
END;
/

CREATE OR REPLACE PROCEDURE deleteMapovyObjekt(n_cislo_objektu IN NUMBER) IS	
BEGIN
	-- nebude provedena operace delete, pouze se nastavi konec platnosti u daneho objektu
	UPDATE mapovy_objekt SET validni_do = trunc(sysdate)
	WHERE cislo_objektu = n_cislo_objektu AND validni_do IS NULL;
END;
/

/******** COMMIT ***********/
COMMIT;
