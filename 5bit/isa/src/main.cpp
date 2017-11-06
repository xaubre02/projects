/**************************************
* -+-+-+-+-+-+-+-+-+-+-+-+-
* - Predmet: ISA
* - Projekt: LDAP Server
* - Modul: Funkce main()
* - Datum: 2017-09-26
* - Autor: Tomas Aubrecht
* - Login: xaubre02
* -+-+-+-+-+-+-+-+-+-+-+-+-
***************************************/

#include "params.hpp"
#include "myldap.hpp"
#include <signal.h>

// priznak behu programu
bool live;

// funkce pro ukonceni behu serveru po stisknuti CTRL+C
void forceExit(int sig)
{
	(void)sig;
	live = false;
}

int main(int argc, char *argv[])
{
	live = true;
	signal(SIGINT, &forceExit);
	
    // zpracovani parametru programu
    Params params(argc, argv);
    if (!params.valid())
    {
        params.printError();
        return EXIT_FAILURE;
    }
    // inicializace a spusteni serveru, pokud vse probehlo v poradku
    else
    {
        MyLDAP myldap(&live, params.file());
        if(!myldap.listenTo(params.port()))
        {
            return EXIT_FAILURE;
        }
        
        myldap.serve();
    }
    
    return EXIT_SUCCESS;
}
