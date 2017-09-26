/**************************************
* -+-+-+-+-+-+-+-+-+-+-+-+-
* - Predmet: ISA
* - Projekt: LDAP Server
* - Modul: main
* - Datum: 2017-09-26
* - Autor: Tomas Aubrecht
* - Login: xaubre02
* -+-+-+-+-+-+-+-+-+-+-+-+-
***************************************/

#include "params.hpp"
#include <iostream>

Params::Params(int argc, char *argv[])
{
	err.clear();
	std::cout << argc << std::endl;
	std::cout << argv[0] << std::endl;
}

bool Params::valid(void)
{
	return err.empty();
}

void Params::printError(void)
{
	std::cerr << err << std::endl;
}

std::string Params::file(void)
{
	return fn;
}

std::string Params::port(void)
{
	return pn;
}
