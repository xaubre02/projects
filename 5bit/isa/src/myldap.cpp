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

bool MyLDAP::sendData(SOCKET &socket, std::string data)
{
    // odeslani dat
    if (send(socket, data.c_str(), int(data.size()), 0) == SOCKET_ERROR)
        return false;

    return true;
}

bool MyLDAP::recvData(SOCKET &socket, char *buffer, int length)
{
    // vynulovani buffer
    memset(buffer, 0, length);
    // prijmuti dat od klienta
    if (recv(socket, buffer, length, 0) == SOCKET_ERROR)
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
    fd_set rfds;
    struct timeval tv { 0, 100 };
    
    FD_ZERO(&rfds);
    FD_SET(listen_socket, &rfds);
    
    return (select(listen_socket + 1, &rfds, NULL, NULL, &tv) > 0) ? true : false;
}

void MyLDAP::serve(void)
{
    std::thread *new_t;
    struct sockaddr_in addr_in;
    socklen_t addrlen = sizeof(addr_in);
    
    while(*live)
    {
        if (pending())
        {
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
    unsigned char buff[BUFF_SIZE];
    while(*live)
    {
        memset(buff, 0, BUFF_SIZE);
        int ret = recv(socket, buff, BUFF_SIZE, 0);
        if(ret != SOCKET_ERROR)
        {
            for (int c = 0; c < ret; c++)
            {
                printf("0x%02x ", buff[c]);
            }
            std::cout << std::endl;
            // BindResponse
            // LDAPMessage  Len   INT   Len   Val   [APPLICATION 1] SEQUENCE  Len   Enum  Len   Val   matchedDN   diagnosticMessage
            // 0x30         0x0c  0x02  0x01  0x01  0x61                      0x07  0x0a  0x01  0x00  0x04  0x00  0x04  0x00
            unsigned char answer[14];
            answer[0]  = 0x30;
            answer[1]  = 0x0c;
            answer[2]  = 0x02;
            answer[3]  = 0x01;
            answer[4]  = 0x01;
            answer[5]  = 0x61;
            answer[6]  = 0x07;
            answer[7]  = 0x0a;
            answer[8]  = 0x01;
            answer[9]  = 0x00;
            answer[10] = 0x04;
            answer[11] = 0x00;
            answer[12] = 0x04;
            answer[13] = 0x00;
            
            if (send(socket, answer, 14, 0) == SOCKET_ERROR)
            {
                std::cerr << "Failed to send a message...\n";
                *live = false;
                close(socket);
                return;
            }
            ret = recv(socket, buff, BUFF_SIZE, 0);
            if(ret != SOCKET_ERROR)
            {
                for (int c = 0; c < ret; c++)
                {
                    printf("0x%02x ", buff[c]);
                }
                std::cout << std::endl;
            }
        }
        
        *live = false;
    }
    
    close(socket);
}

int MyLDAP::decode(const char *msg)
{
    std::cout << msg << std::endl;
    return -1;
}

bool MyLDAP::answer(int type)
{
    switch(type)
    {
        default:
            return false;
    }
}
