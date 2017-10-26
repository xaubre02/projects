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
enum ProtocolOp : int
{
    ldap_bind   = 0x60,
    ldap_search = 0x63,
    ldap_unbind = 0x42
};

// Vycet navratovych kodu
enum ResultCode : int
{
    rc_success = 0x00,
    rc_protocolError = 0x02,
    rc_sizeLimitExceeded = 0x04,
    rc_authMethodNotSupported = 0x07
    
};

/**
 *  Trida pro dekodovani LDAP zprav.
 */
class MyLDAPMsgDecoder
{
    // Navratova hodnota zpravy - result code
    int rc;
    
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
    
    // Vrati resultCode (hodnota promenne rc)
	int resultCode(void);
    
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

    static std::string createBindResponse(int msg_id, int res_code);
    static std::string createSearchResultDone(int msg_id, int res_code);
    static std::string createNoticeOfDisconnection(int res_code);
};
