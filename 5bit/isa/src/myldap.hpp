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

#include "encoding.hpp"
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <thread>


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
    std::string file;                // nazev CSV souboru
    
    
    // Zkontroluje, zdali je zde nejake cekajici prichozi pripojeni
    bool pending(void);
    
    // kazde vlakno vykona tuto funkci pro zpracovani pozadavku klienta
    void process(SOCKET socket);
    
    // Odesle data.
    bool sendMsg(SOCKET &socket, std::string data);
    
    // Najde v csv souboru vsechny zaznamy odpovidajici filtru
    std::vector<csv_record> findEntries(LDAPFilter *filter);
    
    // aplikace filtru na dany zaznam a jeho pripadne ulozeni do vysledneho seznamu
    void applyFilter(csv_record *rec, LDAPFilter *filter, std::vector<csv_record> *list);
    
    // aplikace substringu na dany retezec, pokud splnuje pozadavky, prida se do seznamu
    void applySubstrings(csv_record *rec, std::string *str, LDAPFilter *filter, std::vector<csv_record> *list);
    
    // zpracovani vsech filtru u operace AND
    void applyANDFilters(std::vector<std::vector<csv_record>> *and_list, std::vector<csv_record> *list);
    
    // odstrani redudantni polozky
    void removeRedudancy(std::vector<csv_record> &list);
    
public:
    
    // Inicializace promennych
    MyLDAP(bool *flag, std::string path);
    
    // Uvolni veskerou pamet
    ~MyLDAP(void);
    
    // Inicializuje socket pro prichozi pripojeni
    bool listenTo(std::string port);
    
    // Zpracuje prichozi pripojeni a obslouzi jej
    void serve(void);
};
