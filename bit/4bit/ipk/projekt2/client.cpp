/***************************************
 * **********
 * Projekt: IPK - Matematicky klient
 * Datum: 2017-04-15
 * Autor: Tomas Aubrecht
 * Login: xaubre02
 * **********
 **************************************/

 #include "client.h"

Client::Client(int argc, char* argv[])
{
	// inicializace promennych
	run = true;
	valid = true;
	error = false;
	useIPv4 = false;
	// zpracovani argumentu
	parseArguments(argc, argv);
	// vynulovani bufferu a struktury serveru
	bzero(buffer, BUFF_SIZE);
	bzero((char *)&sa_server, sizeof(sa_server));
	bzero((char *)&sa_server6, sizeof(sa_server6));
}

Client::~Client(void)
{
	// uzavre socket klienta
	close(ClientSocket);
}

void Client::seekAndConnect(void)
{
	// deklarace sitovych struktur
	struct hostent *server;
	struct in_addr IPv4;
	struct in6_addr IPv6;

	// inicializace struktur na zaklade typu IP adresy
	if (useIPv4)
	{
		inet_pton(AF_INET, IP_addr.c_str(), &IPv4);
		server = gethostbyaddr(&IPv4, sizeof(IPv4), AF_INET);
		// pokud nebyla nalezena zadana adresa, ukonci se s chybou
		if(server == NULL)
		{
			close(ClientSocket);
			cerr << "Error: No such host with this IP!" << endl;
			exit(EXIT_FAILURE);
		}
		// napleni struktury serveru daty
		sa_server.sin_family = AF_INET;
		bcopy((char *)server->h_addr, (char *)&sa_server.sin_addr.s_addr, server->h_length);
		// server bude vzdy naslouchat na portu 55555
		sa_server.sin_port = htons(55555);

		// vytvori socket klienta a zkontroluje uspesnost akce
		ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (ClientSocket <= 0) 
			exitError();

		// pripoji se a zkontroluje uspesnost akce
		result = connect(ClientSocket, (struct sockaddr *)&sa_server, sizeof(sa_server));
		if (result != 0)
		{	
			close(ClientSocket);
			exitError();
		}
	}
	else
	{
		inet_pton(AF_INET6, IP_addr.c_str(), &IPv6);
		server = gethostbyaddr(&IPv6, sizeof(IPv6), AF_INET6);
		// pokud nebyla nalezena zadana adresa, ukonci se s chybou
		if(server == NULL)
		{
			close(ClientSocket);
			cerr << "Error: No such host with this IP!" << endl;
			exit(EXIT_FAILURE);
		}

		// napleni struktury serveru daty
		sa_server6.sin6_family = AF_INET6;
		bcopy((char *)server->h_addr, (char *)&sa_server6.sin6_addr.s6_addr, server->h_length);
		// server bude vzdy naslouchat na portu 55555
		sa_server6.sin6_port = htons(55555);

		// vytvori socket klienta a zkontroluje uspesnost akce
		ClientSocket = socket(AF_INET6, SOCK_STREAM, 0);
		if (ClientSocket <= 0) 
			exitError();

		// pripoji se a zkontroluje uspesnost akce
		result = connect(ClientSocket, (struct sockaddr *)&sa_server6, sizeof(sa_server6));
		if (result != 0)
		{	
			close(ClientSocket);
			exitError();
		}
	}

	// zasle uvitaci zpravu
	sendHello();
}

void Client::serve(void)
{
	// funguje, dokud neprijme zpravu BYE[mezera]SECRET
	while (run)
	{
		// prijme zpravu
		recvMsg();
		// dekoduje a zpracuje
		decodeMsg();
		// pripravi flagy pro dalsi zpravu
		valid = true;
		error = false;
		// vynuluje buffer
		bzero(buffer, BUFF_SIZE);
	}
}

void Client::parseArguments(int argc, char* argv[])
{
	// musi byt zadany pouze 2 argumenty
	if (argc != 2)
	{
		cerr << "Error: Invalid parameters!" << endl;
		exit(EXIT_FAILURE);
	}
	// ulozeni IP adresy
	IP_addr = argv[1];
	// hledani tecky v adrese
	size_t pos = IP_addr.find(".", 0);
	int dots = 0;
	// hleda dalsi tecky v adrese, kdy pocet tecek musi byt 3
	while (pos != string::npos)
	{
		pos = IP_addr.find(".", pos + 1);
		dots++;
	}
	// pokud byly nalezeny 3 tecky jedna se o IPv4 adresu
	// nebo by se alespon melo jednat, s tim uz si ale poradi funkce gethostbyaddr()
	if(dots == 3) 
		useIPv4 = true;
}

void Client::sendHello(void)
{
	// vytvori uvitaci zpravu a posle ji serveru
	string hello("HELLO ");
	hello.append(hashLogin() + "\n");
	sendMsg(hello.c_str());
}

string Client::hashLogin(void)
{
	// zahashuje muj login
	char mdString[33];
	char str[] = LOGIN;
	// vyuziti knihovny openssl/md5
	unsigned char digest[MD5_DIGEST_LENGTH];
    MD5((unsigned char*)&str, strlen(str), (unsigned char*)&digest);

	for(int i = 0; i < 16; i++)
		sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
	// vrati zahashovany login
	return mdString;
}

// matematicky problem je zadan ve formatu: CISLO[mezera]OPERATOR[mezera]CISLO\n
string Client::solveProblem(string problem)
{
	// promenne uchovavajici vyskyt prvni mezery, druhe mezery a znaku konce radku
	size_t s1;
	size_t s2;
	size_t eol;

	// nalezeni prvni mezery oddelujici prvni cislo a znamenko
	s1 = problem.find(" ", 0);
	if (s1 == string::npos)
	{
		valid = false;
		return "ERROR";
	}

	// nalezeni druhe mezery oddelujici znamenko a druhe cislo
	s2 = problem.find(" ", s1 + 1);
	if (s2 == string::npos)
	{
		valid = false;
		return "ERROR";
	}
	// nalezeni konce radku
	eol = problem.find("\n", s2 + 1);
	if (eol == string::npos)
	{
		valid = false;
		return "ERROR";
	}

	int  sign = getSign(problem.substr(s1 + 1, s2 - s1 - 1));						// podretezec mezi mezerama
	long num1 = convertNumber(problem.substr(0, s1).c_str());						// podretezec od zacatku retezce po prvni mezeru
	long num2 = convertNumber(problem.substr(s2 + 1, eol - s2 - 1).c_str());		// podretezec od druhe mezery po konec radku
	// pokud byly predchozi operace uspesne, vypocitej priklad
	if(valid && !error)
	{
		stringstream con;
		con << fixed << setprecision(2) << truncateNumber(calculate(num1, num2, sign));
		// pokud pocetni operace probehla uspesne a nepretekla, vrat retezec obsahujici vysledek
		if(valid && !error) return con.str();
	}
	
	// v pripade chyby vrat ERROR
	return "ERROR";
}

int Client::getSign(string sign)
{
	     if(!sign.compare("+")) return 0;
	else if(!sign.compare("-")) return 1;
	else if(!sign.compare("*")) return 2;
	else if(!sign.compare("/")) return 3;

	// v pripade chyby nastavi priznak a vrati -1
	valid = false;
	return -1;
}

long Client::convertNumber(const char *ch)
{
	// prevede string na long
	char * end = NULL;
	long number = strtol(ch, &end, 10);
	// pokud se jedna o neplatne cislo
	if(*end != '\0') valid = false;
	// pokud pretece datovy typ long
	if(errno == ERANGE)
	{
		error = true;
		errno = 0;
	}

	return number;
}

double Client::truncateNumber(double number)
{
	// oreze na 2 desetinna mista bez zaokrouhleni
	return number >= 0. ? (floor(number * 100.) / 100.) : (ceil(number * 100.) / 100.);
}

double Client::calculate(double num1, double num2, int sign)
{
	// pokud se jedna o deleni nulou
	if(sign == 3 && num2 == 0)
	{
		error = true;
		return -1;
	}

	// na zaklade znamenka provede danou operaci
	switch(sign)
	{
		case 0: return num1 + num2;
		case 1: return num1 - num2;
		case 2: return num1 * num2;
		case 3: return num1 / num2;
	}
	// jinak chyba (nemelo by nastat)
	valid = false;
	return -1;
}

void Client::decodeMsg(void)
{
	string msg(buffer);
	// kontrola, zdali je v pozadavku mezera, ktera tam musi byt, jinak se ignoruje
	size_t pos = msg.find(" ", 0);
	if (pos == string::npos) return;
	// ulozeni pozdadavku
	string cmd(msg.substr(0, pos));
	// porovnavani pozadavku s podporovanymi operacemi
	if (!cmd.compare("SOLVE"))
	{
		// vytvoreni odpovedi
		string result("RESULT ");
		// zkontoluje formát zbytku zpravy a pokud je v poradku, vrati vysledek
		result.append(solveProblem(msg.substr(pos + 1, msg.length() - pos)) + "\n");
		// odeslani odpovedi pouze v pripade, kdy se jedna o platny format zpravy
		if(valid) sendMsg(result.c_str());
	}
	else if (!cmd.compare("BYE"))
	{
		// formát zprávy musí být BYE[mezera]SECRET\n
		size_t eol = msg.find("\n", pos);
		if (eol == string::npos) return;

		// vypise na stdout SECRET a ukonci program
		cout << msg.substr(pos + 1, eol - pos);
		run = false;
	}
}

void Client::sendMsg(const char *msg)
{
	// odesle zpravu a zkontroluje uspesnost odeslani
	result = send(ClientSocket, msg, strlen(msg), 0);
	if (result < 0)
	{	
		exitError("Send");
	}
}

void Client::recvMsg(void)
{
	// vynuluje buffer
	bzero(buffer, BUFF_SIZE);
	// prijme zpravu a zkontroluje uspesnost prijeti
	result = recv(ClientSocket, buffer, sizeof(buffer), 0);
	if (result < 0)
	{	
		exitError("Receive");
	}
}

void Client::exitError(const char *spec) const
{
	// pridani specifikace ke zprave
	if(spec != NULL) cerr << spec << " ";
	// vypsani na stderr a ukonceni programu s chybou
	cerr << "Error: " << strerror(errno) << endl;
	exit(EXIT_FAILURE); 
}
