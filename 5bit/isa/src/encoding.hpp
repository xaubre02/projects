#pragma once

/**************************************
* -+-+-+-+-+-+-+-+-+-+-+-+-
* - Predmet: ISA
* - Projekt: LDAP Server
* - Modul: BER kodovani
* - Datum: 2017-09-26
* - Autor: Tomas Aubrecht
* - Login: xaubre02
* -+-+-+-+-+-+-+-+-+-+-+-+-
***************************************/

#include <cstdlib>
#include <string>

// Vycet podporovanych zprav od klienta
enum MessageType : int
{
    ldap_bind   = 0x60,
    ldap_search = 0x63,
    ldap_unbind = 0x42
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
