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
#include <algorithm>
#include <fcntl.h>

MyLDAP::MyLDAP(bool *flag, std::string path)
{
	live = flag;
    listen_socket = SOCKET_ERROR;
    file = path;
}

MyLDAP::~MyLDAP(void)
{
    close(listen_socket);
    // odstraneni vlaken
    for (auto t : list)
    {
        t->detach();
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
    
    // nastaveni na neblokujici socket
    if (fcntl(listen_socket, F_SETFL, fcntl(listen_socket, F_GETFL, 0) | O_NONBLOCK) == SOCKET_ERROR )
    {
        std::cerr << "Chyba pri nastavovani neblokujiciho socketu!\n";
        close(listen_socket);
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

void MyLDAP::process(SOCKET socket)
{
    // vytvoreni a vynulovani bufferu pro prijem zprav
    unsigned char buff[BUFF_SIZE];
    memset(buff, 0, BUFF_SIZE);
    
    // -------------------------------------------------------------------------
    // -------------------------- BIND REQUEST ---------------------------------
    // prijeti zpravy od klienta
    int ret = recv(socket, buff, BUFF_SIZE, 0);
    if (ret <= 0)
    {
        std::cerr << "Nastala chyba pri prijimani zpravy!\n";
        close(socket);
        return;
    }
    // dekodovani zpravy
    MyLDAPMsgDecoder msg(buff, ret);

    // pokud se jedna o zpravu bindRequest
    if (msg.getType() == ldap_bind)
    {
        // odeslat bindResponse s danym ID zpravy a navratovym kodem
        if (!sendMsg(socket, MyLDAPMsgConstructor::createBindResponse(msg.getID(), msg.resultCode())))
        {
            std::cerr << "Nastala chyba pri odesilani BindResponse!\n";
            close(socket);
            return;
        }
    }
    // jinak ukonci spojeni
    else
    {
        std::cerr << "Neplatna zprava! Ocekavan BindRequest. Ukoncuji spojeni...\n";
        close(socket);
        return;
    }

    // -------------------------------------------------------------------------
    // ------------------------- SEARCH REQUEST --------------------------------
    // prijeti zpravy od klienta
    ret = recv(socket, buff, BUFF_SIZE, 0);
    if(ret <= 0)
    {
        std::cerr << "Nastala chyba pri prijimani zpravy!\n";
        close(socket);
        return;
    }
    // dekodovani zpravy
    msg.decode(buff, ret);
    
    // pokud se jedna o zpravu searchRequest
    if(msg.getType() == ldap_search)
    {
        int result_code = msg.resultCode();
        
        // pozdaovane atributy
        req_atts ra(msg.requiredAttributes());
        if (!ra.valid)
            result_code = rc_noSuchAttribute;
            
        // pokud se jedna o platny pozadavek
        if (result_code == rc_success)
        {
            // najdi vsechny zaznamy splnujici zadany filtr
            std::vector<csv_record> entries = findEntries(msg.getFilter());
            // odstraneni duplicitnich zaznamu
            removeRedudancy(entries);
            // sizelimit
            ulong size_limit = msg.sizeLimit();
            
            // dokud je c mensi nez sizeLimit a zaroven dokud je mensi nez pocet nalezenych zaznamu
            for (ulong c = 0; c < size_limit && c < entries.size(); c++)
            {
                if (!sendMsg(socket, MyLDAPMsgConstructor::createSearchResultEntry(msg.getID(), &entries[c], &ra)))
                {
                    std::cerr << "Nastala chyba pri odesilani SearchResultEntry!\n";
                    close(socket);
                    return;
                }
            }
            if (size_limit < entries.size())
                result_code = rc_sizeLimitExceeded;

        }

        // odeslat searchResDone s danym ID zpravy a navratovym kodem
        if (!sendMsg(socket, MyLDAPMsgConstructor::createSearchResultDone(msg.getID(), result_code)))
        {
            std::cerr << "Nastala chyba pri odesilani SearchResultDone!\n";
            close(socket);
            return;
        }
    }
    // pokud se jedna o platnou zpravu unbindRequest
    else if(msg.getType() == ldap_unbind && msg.resultCode() == rc_success)
    {
        close(socket);
        return;
    }
    else
    {
        std::cerr << "Neplatna zprava! Ocekavan SearchRequest. Ukoncuji spojeni...\n";
        close(socket);
        return;
    }
    
    // -------------------------------------------------------------------------
    // ------------------------- UNBIND REQUEST --------------------------------
    // prijeti zpravy od klienta
    ret = recv(socket, buff, BUFF_SIZE, 0);
    if(ret <= 0)
    {
        std::cerr << "Nastala chyba pri prijimani zpravy!\n";
        close(socket);
        return;
    }
    
    // Ukonceni spojeni za jakychkoliv podminek - server v tuto chvili splnil pozadavky klienta a zadne dalsi zpravy nejsou podporovane
    close(socket);
    return;
}

std::vector<csv_record> MyLDAP::findEntries(LDAPFilter *filter)
{
    // vysledny seznam
    std::vector<csv_record> list;
    
    // otevreni souboru pro cteni
    std::ifstream csv;
	csv.open(file.c_str(), std::ios::binary);
	if (!csv.is_open())
    {
		std::cerr << "Nelze otevrit soubor!\n";
        return std::vector<csv_record>();
	}
    
    // prochazi vsechny radky daneho csv souboru
    std::string line;
    while (getline(csv, line))
    {
        // pokud se jedna o platny zaznam, aplikuj na nej filtr
        csv_record rec(line);
        if (rec.valid)
        {
            applyFilter(&rec, filter, &list);
        }
    }

    return list;
}

void MyLDAP::applyFilter(csv_record *rec, LDAPFilter *filter, std::vector<csv_record> *list)
{
    // pomocny seznam seznamu s danymi polozkami
    std::vector<std::vector<csv_record>> and_list;
    
    // pomocny seznam pro zpracovani not polozky
    std::vector<csv_record> not_list;
    bool included = false;
    
    switch (filter->type)
    {
        case filter_and:
            for (auto fil : filter->f_and)
            {
                // zpracovani jednotlivych filtru v operaci AND
                std::vector<csv_record> tmp_list;
                applyFilter(rec, &fil, &tmp_list);
                // ulozeni seznamu, co se maji ANDovat
                and_list.push_back(tmp_list);
            }
            applyANDFilters(&and_list, list);
            break;

        case filter_or:
            // aplikace kazdeho filtru v or
            for (auto fil : filter->f_or)
                applyFilter(rec, &fil, list);
            
            break;
        
        case filter_not:
            // aplikace kazdeho filtru v not - je jen jeden
            for (auto fil : filter->f_not)
                applyFilter(rec, &fil, &not_list);
            
            // pokud neni zaznam v not_listu, tak ho pridej do vysledneho seznamu
            for (auto entry : not_list)
            {
                if (entry == *rec)
                {
                    included = true;
                    break;
                }
            }    
            if (!included)
                list->push_back(*rec);

            break;
        
        // equalityMatch
        case filter_eq_match:
            switch (filter->at_des)
            {
                case ad_uid: // hledani podle uid
                    if (!filter->as_val.compare(rec->uid))
                        list->push_back(*rec);
                    break;
                    
                case ad_cn: // hledani podle cn
                    if (!filter->as_val.compare(rec->cn))
                        list->push_back(*rec);
                    break;
                    
                case ad_mail: // hledani podle mailu
                    if (!filter->as_val.compare(rec->mail))
                        list->push_back(*rec);
                    break;
                    
                default: // nemelo by nastat
                    return;
            }
            break;
        // substring
        case filter_substr:
            switch (filter->at_des)
            {
                case ad_uid: // hledani podle uid
                    applySubstrings(rec, &rec->uid, filter, list);
                    break;
                    
                case ad_cn: // hledani podle cn
                    applySubstrings(rec, &rec->cn, filter, list);
                    break;
                    
                case ad_mail: // hledani podle mailu
                    applySubstrings(rec, &rec->mail, filter, list);
                    break;
                    
                default: // nemelo by nastat
                    return;
            }
            break;
    }
}

void MyLDAP::applySubstrings(csv_record *rec, std::string *str, LDAPFilter *filter, std::vector<csv_record> *list)
{
    // pokud je zadan nejaky podretezec, kterym ma dany retezec zacinat
    if (!filter->sub_init.empty())
    {
        // pokud se takovy podretezec v danem retezci nenachazi
        if (str->find(filter->sub_init) == std::string::npos)
            return;
            
        // pokud timto podretezcem ale nezacina
        if (str->substr(0, filter->sub_init.length()).compare(filter->sub_init)) // porovnani pocatku retezce o delce podretezce filtru s podretezcem filtru
            return;
    }
    
    // pokud jsou zadany nejake podretezce, ktere mohou byt kdekoliv
    size_t pos = 0;
    for (auto a : filter->sub_any)
    {
        pos = str->substr(pos).find(a);
        // pokud se takovy podretezec v danem retezci nenachazi
        if (pos == std::string::npos)
            return;
        
        // dalsi podretezec se bude vyhledavat od konce predchouiho
        pos += a.length();
    }
    
    // pokud je zadan nejaky podretezec, kterym ma dany retezec koncit
    if (!filter->sub_fin.empty())
    {
        // pokud se takovy podretezec v danem retezci nenachazi
        if (str->find(filter->sub_fin) == std::string::npos)
            return;
            
        // pokud timto podretezcem ale nekonci
        if (str->substr(str->length() - filter->sub_fin.length()).compare(filter->sub_fin)) // porovnani konce retezce o delce podretezce filtru s podretezcem filtru
            return;
    }
    
    // ulozeni daneho zaznamu
    list->push_back(*rec);
}

void MyLDAP::applyANDFilters(std::vector<std::vector<csv_record>> *and_list, std::vector<csv_record> *list)
{
    // ulozeni posledniho seznamu
    std::vector<csv_record> tmp = and_list->back();
    // odstrani posledni seznam (kvuli porovnavani sama se sebou)
    and_list->pop_back();
    
    for (csv_record rec : tmp)
    {
        // prochazi vsechny AND seznamy a kontroluje, zdali je dany zaznam obsazen ve vsech
        bool is_in_all = true;
        for (auto a : *and_list)
        {
            // kontrola, zdali je obsazen v tomto seznamu
            if (std::find(a.begin(), a.end(), rec) == a.end())
            {
                is_in_all = false;
                break;
            }
        }
        // pokud dany zaznam splnuje vsechny podminky
        if (is_in_all)
            list->push_back(rec);
    }
}

void MyLDAP::removeRedudancy(std::vector<csv_record> &list)
{
    // pro kazdy zaznam v seznamu
    for (unsigned int x = 0; x < list.size(); x++)
    {
        // znovu projdi seznam a hledej duplicitni zaznamy
        for (unsigned int y = 0; y < list.size(); )
        {
            // odebrani zaznamu, pokud se zaznamy rovnaji a zaroven to neni jeden sam se sebou porovnavajici se zaznam
            if(x != y && list[x] == list[y])
                list.erase(list.begin() + y);
            
            else
                y++;
        }
    }
}
