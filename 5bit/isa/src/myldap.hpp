#pragma once

/**************************************
* -+-+-+-+-+-+-+-+-+-+-+-+-
* - Predmet: ISA
* - Projekt: LDAP Server
* - Modul: Vlastni LDAP server
* - Datum: 2017-09-26
* - Autor: Tomas Aubrecht
* - Login: xaubre02
* -+-+-+-+-+-+-+-+-+-+-+-+-
***************************************/

#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

/** 
 * Dojde k uzamceni pri vytvoreni objektu lock_guard.
 * Jakmile dojde k vyvolani destruktoru(control out of scope) tohoto objektu, zamek je automaticky odemcen.
 */
#define LOCK_SECTION    std::lock_guard<std::mutex> lock(mtx);
#define SOCKET_ERROR    (-1)
#define BUFF_SIZE         (2048)

typedef int SOCKET;

/**
 *  Trida reprezentujici vlastni LDAP server.
 */
class MyLDAP
{
	bool *live;
    SOCKET listen_socket;            // socket pro prichozi pripojeni
    std::vector<std::thread*> list;   // seznam vlaken zpracovavajici jednotlive klienty
    std::mutex mtx;                  // zamek pro pristup k souboru
    std::fstream file;               // CSV soubor - kriticka sekce
    
    // kazde vlakno vykona tuto funkci pro zpracovani pozadavku klienta
    void process(SOCKET socket);
    
    // Odesle data.
    bool sendData(SOCKET &socket, std::string data);
    
    // Vynuluje buffer a prijme data o delce length.
    bool recvData(SOCKET &socket, char* buffer, int length);
    
public:
    
    // Inicializuje server a otevře soubor
    MyLDAP(bool *flag, std::string path);
    
    // Uvolni veskerou pamet (socket, soubor a vlakna)
    ~MyLDAP(void);
    
    // Inicializuje socket pro richozi pripojeni
    bool listenTo(std::string port);
    
    // Zpracuje prichozi pripojeni a obslouzi je
    void serve(void);
    
    // debug
    bool pending(void);
};
