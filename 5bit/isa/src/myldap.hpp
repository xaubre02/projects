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
#include <string>

class MyLDAP
{
public:
	
	MyLDAP(std::string port, std::string file);
	~MyLDAP(void);
};
