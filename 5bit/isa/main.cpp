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

int main(int argc, char *argv[])
{
	Params params(argc, argv);
	if (!params.valid())
	{
		params.printError();
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
