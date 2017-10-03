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

bool live;

void forceExit(int sig)
{
	(void)sig;
	live = false;
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	live = true;
	signal(SIGINT, &forceExit);
	
    Params params(argc, argv);
    if (!params.valid())
    {
        params.printError();
        return EXIT_FAILURE;
    }
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
