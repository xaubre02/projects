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

#include "myldap.hpp"
#include "encoding.hpp"
#include <iostream>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>


MyLDAP::MyLDAP(bool *flag, std::string path)
{
	live = flag;
    listen_socket = SOCKET_ERROR;
    file.open(path.c_str(),    std::ios::in | std::ios::out | std::ios::app); // otevreni souboru pro cteni/zapis
}

MyLDAP::~MyLDAP(void)
{
    close(listen_socket);
    file.close();
    // odstraneni vlaken
    for (auto t : list)
    {
        t->join();
        delete(t);
    }
    
    list.clear();
}

bool MyLDAP::sendMsg(SOCKET &socket, std::string data)
{
    // odeslani dat
    if (send(socket, data.c_str(), int(data.size()), 0) == SOCKET_ERROR)
        return false;

    return true;
}

bool MyLDAP::listenTo(std::string port)
{
    struct addrinfo hints;
    struct addrinfo *result;

    // vynulovani struktury a naplneni daty
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags    = AI_PASSIVE;
    hints.ai_addr     = NULL;
    hints.ai_next     = NULL;

    // naplni vyslednou strukturu obsahujici potrebna data
    if (getaddrinfo(INADDR_ANY, port.c_str(), &hints, &result))
    {
        std::cerr << "Chyba pri inicializaci struktury addrinfo!\n";
        return false;
    }

    // inicializace socketu
    listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listen_socket == SOCKET_ERROR)
    {
        std::cerr << "Chyba pri inicializaci socketu!\n";
        freeaddrinfo(result);
        return false;
    }

    // nabindovani socketu
    if (bind(listen_socket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR)
    {
        std::cerr << "Nelze navazat socket na dany port!\n";
        close(listen_socket);
        freeaddrinfo(result);
        return false;
    }

    // socket zacne naslouchat pro prichozi pripojeni
    if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Chyba inicializovat socket pro prichozi spojeni!\n";
        close(listen_socket);
        freeaddrinfo(result);
        return false;
    }

    freeaddrinfo(result);
    return true;
}

bool MyLDAP::pending(void)
{
    // struktura pro cteni
    fd_set rfds;
    struct timeval tv { 0, 100 };
    
    // inicializace struktury
    FD_ZERO(&rfds);
    FD_SET(listen_socket, &rfds);
    
    // vrati hodnotu, zdali nekdo ceka
    return (select(listen_socket + 1, &rfds, NULL, NULL, &tv) > 0) ? true : false;
}

void MyLDAP::serve(void)
{
    // ukazatel na vlakno
    std::thread *new_t;
    // pomocne promenne pro prijem klientu
    struct sockaddr_in addr_in;
    socklen_t addrlen = sizeof(addr_in);
    
    // hlavni smycka serveru
    while(*live)
    {
        // pokud je zde cekajici klient
        if (pending())
        {
            // prijmuti klienta
            SOCKET in_con = accept(listen_socket, (struct sockaddr *)&addr_in, &addrlen); // prichozi propojeni
            if (in_con == SOCKET_ERROR)
            {    
                std::cerr << "Chyba pri navazovani spojeni s klientem!\n";
            }
            
            // vytvoreni noveho vlakna a jeho ulozeni do seznamu
            new_t = new std::thread([=] { process(in_con); });
            list.push_back(new_t);
        }
        else
            sleep(.1);
    }
}

// TODO ...
void MyLDAP::process(SOCKET socket)
{
    // vytvoreni a vynulovani bufferu pro prijem zprav
    unsigned char buff[BUFF_SIZE];
    memset(buff, 0, BUFF_SIZE);
    
    // -------------------------------------------------------------------------
    // -------------------------- BIND REQUEST ---------------------------------
    // prijeti zpravy od klienta
    int ret = recv(socket, buff, BUFF_SIZE, 0);
    if (ret == SOCKET_ERROR)
    {
        close(socket);
        return;
    }
    // dekodovani zpravy
    MyLDAPMsgDecoder msg(buff, ret);
    // pokud se jedna o platnou zpravu bindRequest
    if (msg.valid() && msg.getType() == ldap_bind)
    {
        // odeslat bindResponse s danym ID zpravy
        if (!sendMsg(socket, MyLDAPMsgConstructor::createBindResponse(msg.getID())))
        {
            close(socket);
            return;
        }
    }
    else
    {
        // TODO - send NoticeOfDisconnection
        close(socket);
        return;
    }
    // -------------------------------------------------------------------------
    // ------------------------- SEARCH REQUEST --------------------------------
    // prijeti zpravy od klienta
    ret = recv(socket, buff, BUFF_SIZE, 0);
    if(ret == SOCKET_ERROR)
    {
        close(socket);
        return;
    }
    // dekodovani zpravy
    msg.clear();
    msg.decode(buff, ret);
    // pokud se jedna o platnou zpravu searchRequest
    if(msg.valid() && msg.getType() == ldap_search)
    {
        // odeslat searchResDone s danym ID zpravy
        if (!sendMsg(socket, MyLDAPMsgConstructor::createSearchResultDone(msg.getID(), 0x00)))
        {
            close(socket);
            return;
        }
    }
    else
    {
        // TODO - send NoticeOfDisconnection
        close(socket);
        return;
    }
    // -------------------------------------------------------------------------
    // ------------------------- UNBIND REQUEST --------------------------------
    // prijeti zpravy od klienta
    ret = recv(socket, buff, BUFF_SIZE, 0);
    if(ret == SOCKET_ERROR)
    {
        close(socket);
        return;
    }
    // dekodovani zpravy
    msg.clear();
    msg.decode(buff, ret);
    // pokud se jedna o platnou zpravu unbindRequest
    if(msg.valid() && msg.getType() == ldap_unbind)
    {
        std::cout << "Successfully ended\n";
        close(socket);
        *live = false;
        return;
    }
    else
    {
        // TODO - send NoticeOfDisconnection
        close(socket);
        return;
    }
}
