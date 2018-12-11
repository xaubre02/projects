/**
* @file    tester.h
* @author  Tomáš Aubrecht (xaubre02)
* @date    květen 2018
* @brief   Deklarace testovací třídy.
*/

#pragma once

#include "detection.h"

// Merlin compatibility
#ifdef __linux__
	#include <boost/filesystem.hpp>
	namespace fs = boost::filesystem;
#elif _WIN32
	#include <filesystem>
	namespace fs = experimental::filesystem;
#endif

#include <fstream> 


/**
* @brief	Třída pro účely testování detektoru příznaků VPMD.
*/
class Tester {
public:

	/**
	* @brief	Konstruktor třídy, který vynuluje počítadlo snímků.
	*/
	Tester() : image_cnt{ 0 } {}

	/**
	* @brief	Destruktor třídy, který uzavře používané soubory.
	*/
	~Tester() { datafile.close(); resfile.close(); }

	/**
	* @brief	Postupně prochází snímky v databázi a zobrazuje je spolu s detekovanými nálezy. Pro přechod k dalšímu snímku musí uživatel stisknout klávesu mezerník nebo klávesu ESC pro ukončení.
	*
	* @param	database	cesta k databázi
	* @return	úspěšnost provedení inspekce
	*/
	bool inspectDetectionResults(fs::path database);

	/**
	* @brief	Postupně prochází snímky v databázi, zobrazuje je a čeká, až uživatel prvním kliknutím levého tlačítka myši do snímku určí polohu optického disku a druhým kliknutím určí polohu fovey. Sběr dat končí po projítí celé databáze nebo v případě stisku klávesy ESC. Výstupem je soubor groundtruth se získanými daty, který je uložen v adresáři databáze.
	*
	* @param	database	cesta k databázi
	* @return	úspěšnost provedení sběru dat
	*/
	bool manualDataCollection(fs::path database);

	/**
	* @brief	Spustí automatické testování detekce optického disku a fovey na snímcích, jejichž parametry jsou uloženy v souboru groundtruth. Výstupem je soubor test_results s výsledky, který je uložen v adresáři s groundtruth souborem.
	*
	* @param	groundtruth	cesta ke groundtruth souboru
	* @return	úspěšnost provedení automatických testů
	*/
	bool runAutomatedTests(fs::path groundtruth);

private:

	/**
	* @brief	Provede sběr dat z aktuálního snímku v adresáři databáze.
	*
	* @param	image	aktuální snímek
	* @param	database	adresář databáze
	* @return	false, pokud uživatel přerušil sběr dat, jinak true
	*/
	bool collectData(fs::path image, fs::path database);

	/**
	* @brief	Inicializuje a vytvoří soubor groundtruth v daném adresáři.
	*
	* @param	database_dir	adresář souboru
	* @return	úspěšnost vytvoření
	*/
	bool createDataFile(fs::path database_dir);

	/**
	* @brief	Inicializuje a vytvoří soubor test_results v daném adresáři.
	*
	* @param	path	adresář souboru
	* @return	úspěšnost vytvoření
	*/
	bool createResultsFile(fs::path groundtruth_file);

	/**
	* @brief	Parsuje řádek (záznam) ze souboru groundtruth.
	*
	* @param	line	záznam
	* @return	úspěšnost parsování
	*/
	bool parseDataFileLine(std::string line);

	/**
	* @brief	Extrahuje z položky jméno a hodnotu proměnné, která je uložena v groundtruth souboru.
	*
	* @param	item	položka oddělená středníkem
	*/
	void parseDataFileItem(std::string item);

	/**
	* @brief	Převede textový zápis bodu na datový typ Point.
	*
	* @param	str	řetězec pro převod
	* @return	převedený bod
	*/
	Point string2point(std::string str);

	/**
	* @brief	Vypočítá vzdálenost mezi dvěma body a zkontroluje, zdali je tato vzdálenost menší než daný limit.
	*
	* @param	p1	první bod
	* @param	p2	druhý bod
	* @param	limit	limit vzdálenosti
	* @return	true v případě nedosažení limitu, jinak false
	*/
	bool isWithinLimit(Point p1, Point p2, double limit);

	/**
	* @brief	počítadlo zpracovaných snímků
	*/
	unsigned long image_cnt;

	/**
	* @brief	soubor groundtruth pro uložení/čtení podkladů pro testování
	*/
	std::fstream datafile;

	/**
	* @brief	soubor test_results pro uložení výsledků testování
	*/
	std::fstream resfile;

	/**
	* @brief	cesta k aktuálně zpracovávanému snímku při testování
	*/
	fs::path cur_img;

	/**
	* @brief	struktura pro uchování dat o optickém disku a fovey při testování
	*/
	Retina retina;
};
