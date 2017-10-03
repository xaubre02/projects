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

#include "params.hpp"
#include <sys/stat.h>
#include <string.h>
#include <fstream>

Params::Params(int argc, char *argv[])
{
	// inicializace pomocnych promennych
	em.clear();
	fn.clear();
	pn.clear();
	
	if (parseParams(argc, argv))
		validateParams();
}

bool Params::valid(void)
{
	return em.empty();
}

void Params::printError(void)
{
	std::cerr << em << std::endl;
}

std::string Params::file(void)
{
	return fn;
}

std::string Params::port(void)
{
	return pn;
}

bool Params::parseParams(int argc, char *argv[])
{
	// server ma dva povinne argumenty (-f <soubor>) a dva volitelne (-p <port>)  + nazev programu
	if (argc != 3 && argc != 5)
	{
		em = "Neplatny pocet argumentu programu!";
		return false;
	}
	
	// pokud je program spusten se 3 argumenty, druhy musi byt prepinac -f
	if (argc == 3)
	{
		if (!strcmp(argv[1], "-f"))
		{
			fn = argv[2];
			pn = "389";
		}
		else
		{
			em = "Zadejte nazev souboru!";
			return false;
		}
	}
	// pokud je program spusten s 5 argumenty, druhy a ctvrty musi byt prepinace -f a -p nebo naopak
	else if (argc == 5)
	{
		if (!strcmp(argv[1], "-f") && !strcmp(argv[3], "-p"))
		{
			fn = argv[2];
			pn = argv[4];
		}
		else if (!strcmp(argv[3], "-f") && !strcmp(argv[1], "-p"))
		{
			fn = argv[4];
			pn = argv[2];
		}
		else
		{
			em = "Neplatne argumenty programu!";
			return false;
		}
	}
	return true;
}

void Params::validateParams(void)
{
	// zkontroluje, zdali se nejedna o adresar
	struct stat sb;
	if (!stat(fn.c_str(), &sb) && S_ISDIR(sb.st_mode))
	{	
		em = "Zadany soubor je adresar!";
		return;
	}
	// zkontroluje, zdali se jedna o platny soubor
	std::ifstream file;
	file.open(fn.c_str(), std::ios::binary);
	if (!file.is_open())
    {
		em = "Neplatny soubor!";
        return;
	}
	
	// port musi byt cislo v rozsahu 0-65535 (unsigned 16-bit int)
	char *error = NULL;
    long num = strtol(pn.c_str(), &error, 10);
    if (*error != '\0' || num < 0 || num > 65535)
    {
        em = "Neplatne cislo portu!";
        return;
	}
}
