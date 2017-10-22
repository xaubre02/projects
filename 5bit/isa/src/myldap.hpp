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
#define BUFF_SIZE       (2048)

typedef int SOCKET;

/**
 *  Trida reprezentujici vlastni LDAP server.
 */
class MyLDAP
{
	bool *live;
    SOCKET listen_socket;            // socket pro prichozi pripojeni
    std::vector<std::thread*> list;  // seznam vlaken zpracovavajici jednotlive klienty
    std::mutex mtx;                  // zamek pro pristup k souboru
    std::fstream file;               // CSV soubor - kriticka sekce
    
    
    // Zkontroluje, zdali je zde cekajici prichozi pripojeni
    bool pending(void);
    
    // kazde vlakno vykona tuto funkci pro zpracovani pozadavku klienta
    void process(SOCKET socket);
    
    // Odesle data.
    bool sendMsg(SOCKET &socket, std::string data);
    
    
public:
    
    // Inicializuje server a otevře soubor
    MyLDAP(bool *flag, std::string path);
    
    // Uvolni veskerou pamet (socket, soubor a vlakna)
    ~MyLDAP(void);
    
    // Inicializuje socket pro richozi pripojeni
    bool listenTo(std::string port);
    
    // Zpracuje prichozi pripojeni a obslouzi je
    void serve(void);
};

// vycet podporovanych zprav od klienta
enum MessageType : int
{
    ldap_bind = 0x60,
    ldap_search = 0x61,
    ldap_unbind = 0x82
};

/**
 *  Trida pro dekodovani LDAP zprav.
 */
class MyLDAPMsgDecoder
{
    // Priznak chyby
    bool err;
    
    // id zpravy
    int id;
    
    // typ zpravy
    int type;
    
public:

    // Dekoduje obsah bufferu
    MyLDAPMsgDecoder(unsigned const char *buff, int len);
    
    // Reinicializuje objekt
    void clear(void);
    
    // Dekoduje obsah bufferu
    void decode(unsigned const char *buff, int len);
    
    // Vrati "true", pokud se jedna o platnou podporovanou zpravu, jinak "false"
	bool valid(void);
    
    // Vrati typ dane zpravy
    int getType(void);
    
    // Vrati ID zpravy
    int getID(void);
};

/**
 *  Trida pro vytvareni LDAP zprav.
 */
class MyLDAPMsgConstructor
{
public:

    static std::string createBindResponse(int msg_id);
    static std::string createSearchResultDone(int msg_id, int resultCode);
    static std::string createNoticeOfDisconnection(int resultCode);
};
