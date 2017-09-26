#pragma once

/**************************************
* -+-+-+-+-+-+-+-+-+-+-+-+-
* - Predmet: ISA
* - Projekt: LDAP Server
* - Modul: Zpracovani parametru
* - Datum: 2017-09-26
* - Autor: Tomas Aubrecht
* - Login: xaubre02
* -+-+-+-+-+-+-+-+-+-+-+-+-
***************************************/

#include <cstdlib>
#include <string>

class Params
{	
public:
	
	// Zpracuje zadane vstupni parametry a nastavi priznaky
	Params(int argc, char *argv[]);
	
	// Vrati "true", pokud jsou zadany platne parametry programu, jinak "false"
	bool valid(void);
	
	// Vytiskne prvni nalezenou chybu v parametrech na standartni chybovy vystup
	void printError(void);
	
	// Vrati nazev souboru, se kterym ma server pracovat
	std::string file(void);
	
	// Vrati cislo portu, na kterem ma server naslouchat
	std::string port(void);
	
private:

	std::string em;		// Error message
	std::string fn;		// File name
	std::string pn;		// Port number
	
	// Zpracuje zadane vstupni parametry
	bool parseParams(int argc, char *argv[]);
	
	// Zkontroluje hodnoty prepinacu
	void validateParams(void);
};
