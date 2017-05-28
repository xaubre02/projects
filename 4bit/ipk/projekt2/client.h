/***************************************
 * **********
 * Projekt: IPK - Matematicky klient
 * Datum: 2017-04-15
 * Autor: Tomas Aubrecht
 * Login: xaubre02
 * **********
 **************************************/

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <iostream>
#include <string.h>
#include <limits>
#include <math.h>
#include <sstream>
#include <cstdlib>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <iomanip>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <stdio.h>
#include <openssl/md5.h>

#define BUFF_SIZE 512
#define LOGIN "xaubre02"

using namespace std;


class Client
{
public:
	// inicializuje promenne a zpracuje argumenty
	Client(int argc, char* argv[]);
	// uvolni pamet
	~Client(void);
	
	// snazi se pripojit na server podle poskytnute adresy
	void seekAndConnect(void);
	// prijima a zpracovava zpravy od serveru a pripadne na ne odpovi
	void serve(void);

private:

	// bool pro hlavni smycku klienta
	bool run;
	// pomocna promenna pro indikaci platneho prikladu
	bool valid;
	// pomocna promenna pro indikaci vyskytu chyby pri pocitani prikladu
	bool error;
	// urcuje, zdali je adresa IPv4 nebo IPv6
	bool useIPv4;

	// ukladani vysledku sitovych operaci (socket(), connect(), ...)
	int result;
	// socket tohoto klienta
	int ClientSocket;
	// uchovava adresu
	string IP_addr;
	// struktura serveru s informacemi o nem
	struct sockaddr_in  sa_server;  // IPv4
	struct sockaddr_in6 sa_server6; // IPv6
	// buffer pro prijimani zprav od serveru s defaultni velikosti 512B
	char buffer[BUFF_SIZE];

	/**
	 * Zpracuje argumenty programu.
	 * @param argc Pocet argumentu
	 * @param argv Pole argumentu
	 */
	void parseArguments(int argc, char* argv[]);

	/** 
	 * Odesle serveru uvitaci zpravu se zahashovanym loginem.
	 */
	void sendHello(void);

	/**
	 * Zahashuje login pomoci md5.
	 * @return Hashovany login
	 */
	string hashLogin(void);

	/**
	 * Kontrola zpravneho formatu zpravy a nasledne rizeni zpracovani matematickeho problemu a vraceni retezce obsahujici odpoved.
	 * @param problem Retezec s prikladem k vyreseni
	 * @return Vysledek pocetni operace.
	 */
	string solveProblem(string problem);

	/**
	 * Zkontroluje platnost znamenka a vrati jeho ciselnou reprezentaci.
	 * @param sign Retezec se znamenkem
	 * @return Ciselnou reprezentaci znamenka.
	 */
	int getSign(string sign);

	/**
	 * Prevede cislo z retezce na double
	 * @param ch Retezec s cislem
	 * @return Cislo
	 */
	long convertNumber(const char *ch);
	
	/**
	 * Orizne cislo na 2 desetinna mista.
	 * @param number Cislo k oriznuti
	 * @return Oriznute cislo
	 */
	double truncateNumber(double number);

	/**
	 * Vypocita matematicky priklad.
	 * @param num1 Prvni cislo prikladu
	 * @param num2 Druhe cislo prikladu
	 * @param sign Znamenko prikladu
	 * @return Vysledek prikladu
	 */
	double calculate(double num1, double num2, int sign);
	
	/**
	 * Dekoduje zpravu od serveru a zpracuje pozadavek.
	 */
	void decodeMsg(void);

	/**
	 * Odesle zpravu serveru.
	 * @param msg Zprava k odeslani
	 */
	void sendMsg(const char *msg);

	/**
	 * Prijme zpravu od serveru a ulozi ji do bufferu.
	 */
	void recvMsg(void);

	/**
	 * Zpracuje chybu a ukonci program.
	 * @param spec Podrobnejsi popis chyby.
	 */
	void exitError(const char *spec = NULL) const;
};

#endif