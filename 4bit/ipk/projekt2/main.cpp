/***************************************
 * **********
 * Projekt: IPK - Matematicky klient
 * Datum: 2017-04-15
 * Autor: Tomas Aubrecht
 * Login: xaubre02
 * **********
 **************************************/

#include "client.h"

int main(int argc, char* argv[])
{
	// vytvori objekt klienta a zpracuje argumenty
	Client client(argc, argv);
	// najde a pripoji se podle poskytnute adresy na dany server (nebo taky ne)
	client.seekAndConnect();
	// obsluhuje server
	client.serve();

	return EXIT_SUCCESS;
}