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
#include <iostream>
#include <sstream>
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
        close(socket);
        return;
    }
    else
    {
        // TODO - send NoticeOfDisconnection
        close(socket);
        return;
    }
}

MyLDAPMsgDecoder::MyLDAPMsgDecoder(unsigned const char *buff, int len)
{
    clear();
    decode(buff, len);
}

void MyLDAPMsgDecoder::clear(void)
{
    // inicializace promennych
    err = true;
    id = -1;
    type = -1;
}
    
void MyLDAPMsgDecoder::decode(unsigned const char *buff, int len)
{
    // -----------------------------
    // ------- DEBUG - tisk --------
    for (int c = 0; c < len; c++)
    {
        printf("0x%02x ", buff[c]);
    }
    // -----------------------------
    
    
    // LDAPMessage musi mit alespon 7 bytu (LDAPMsg Len MsgID Len Val protocolOp Len)
    if (len < 7)
        return;
    
    // Musi se jednat o LDAPMessage a musi mit spravnou delku
    if (buff[0] != 0x30 || buff[1] != (len - 2))
        return;
    
    // MessageID musi mit typ INTEGER a nesmi mit nulovou delku, zaroven kontrola delky
    if (buff[2] != 0x02 || buff[3] == 0x00 || buff[3] != (len - 4))
        return;
    
    // kontrola delky daneho typu
    if (buff[6] != (len - 7))
        return;
        
    // ulozeni id
    id = buff[4];
    
    // ulozeni typu
    type = buff[5];
    
    switch (type)
    {
        case ldap_bind:
            // version musi byt typu INTEGER o delce 1 v rozsahu 0...127, ale podpora je pouze po verzi 3
            if (buff[7] != 0x02 || buff[8] != 0x01 || buff[9] > 0x03 || buff[9] < 0x01)
                return;
            
            // name musi byt typu OCTET STRING s nulovou délkou
            if (buff[10] != 0x04 || buff[11] != 0x00)
                return;
                
            // authentication simple - [0] OCTET STRING o nulové delce
            if (buff[12] != 0x80 || buff[13] != 0x00)
                return;
                
            break;
        
        case ldap_search:
            break;
        
        case ldap_unbind:
            break;
        
        default:
            return;
    }
    
    // zprava byla uspesne dekodovana
    err = false;
}

bool MyLDAPMsgDecoder::valid(void)
{
    return !err;
}

int MyLDAPMsgDecoder::getType(void)
{
    return type;
}

int MyLDAPMsgDecoder::getID(void)
{
    return id;
}

std::string MyLDAPMsgConstructor::createBindResponse(int msg_id)
{
    std::stringstream msg;
    
    msg << 0x30;   // ---------- LDAPMessage
    msg << 0x0c;   // ---------- LENGTH
    msg << 0x02;   // ----- MessageID
    msg << 0x01;   // ----- LENGTH
    msg << msg_id; // ----- VALUE
    msg << 0x61;   // ----- bindResponse - [APPLICATION 1] SEQUENCE
    msg << 0x07;   // ----- LENGTH
    msg << 0x0a;   // ENUMERATE
    msg << 0x01;   // LENGTH
    msg << 0x00;   // VALUE
    msg << 0x04;   // matchedDN
    msg << 0x00;   // LENGTH
    msg << 0x04;   // diagnosticMessage
    msg << 0x00;   // LENGTH
    
    return msg.str();
}

std::string MyLDAPMsgConstructor::createSearchResultDone(int msg_id, int resultCode)
{
    std::stringstream msg;
    
    msg << 0x30;   // ---------- LDAPMessage
    msg << 0x00;   // ---------- LENGTH
    msg << 0x02;   // ----- MessageID
    msg << 0x01;   // ----- LENGTH
    msg << msg_id; // ----- VALUE
    msg << 0x65;   // ----- searchResDone - [APPLICATION 5] SEQUENCE
    msg << 0x07;   // ----- LENGTH
    msg << 0x0a;   // ENUMERATE
    msg << 0x01;   // LENGTH
    msg << resultCode;   // VALUE
    msg << 0x04;   // matchedDN
    msg << 0x00;   // LENGTH
    msg << 0x04;   // diagnosticMessage
    msg << 0x00;   // LENGTH
    
    return msg.str();
}

std::string MyLDAPMsgConstructor::createNoticeOfDisconnection(int resultCode)
{/*
    unsigned char msg[31];
    msg[0] = 0x30; // LDAPMessage
    msg[1] = 0x1d; // LENGTH
    msg[2] = 0x02; // INTEGER
    msg[3] = 0x01; // LENGTH
    msg[4] = 0x00; // VALUE
    msg[5] = 0x78; // extendedResp
    msg[6] = 0x18; // LENGTH
    msg[7] = 0x0a; // LDAPResult
    msg[8] = 0x01; // LENGTH
    msg[9] = resultCode; // VALUE 
    msg[10] = 0x04; // matchedDN
    msg[11] = 0x00; // LENGTH
    msg[12] = 0x04; // diagnosticMessage
    msg[13] = 0x00; // LENGTH
    msg[14] = 0x8a; // [10] LDAPOID
    msg[15] = 0x0f; // LENGTH
    msg[16] = 0x01; // 1
    msg[17] = 0x2e; // .
    msg[18] = 0x03; // 3
    msg[19] = 0x2e; // .
    msg[20] = 0x06; // 6
    msg[21] = 0x2e; // .
    msg[22] = 0x01; // 1
    msg[23] = 0x2e; // .
    msg[24] = 0x04; // 4
    msg[25] = 0x2e; // .
    msg[26] = 0x01; // 1
    msg[27] = 0x2e; // .
    msg[28] = 0x0; // 1466
    msg[29] = 0x2e; // .
    msg[30] = 0x0; // 20036*/
    resultCode++;
    return std::string("faaail");
}
