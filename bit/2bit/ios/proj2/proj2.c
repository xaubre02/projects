/// *****************************************************************
/* *************************************************** **
*****	
****		Datum:	28.4.2016                	
***		Autor: 	Tomas Aubrecht (xaubre02)	
***		Predmet: 	Operacni systemy (IOS)	
****		Projekt:	Synchronizace procesu    	
*****
/// ************************************************** */
/// *****************************************************************

// Zahrnuti potrebnych knihoven
#include <time.h> 
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>

// Zadefinovani unikatnich jmen sdilenych prostredku a jejich velikosti
#define shmSize 5*sizeof(long)
#define shmName "/xaubre02"
#define semName1 "/xaubre02a"
#define semName2 "/xaubre02b"
#define semName3 "/xaubre02c"
#define semName4 "/xaubre02d"
#define semName5 "/xaubre02e"
#define semName6 "/xaubre02f"

// Zadefinovani sekvence prikazu pro uvolňovani prostredku kvuli castemu opakovani
#define freeAll fclose(output);\
				free(passPID);\
				destroySemaphores();\
				shm_unlink(shmName);
				
/// ******************************************************************************************************** ///
/// ****************************************** Datove struktury ******************************************** ///

// Datova struktura obsahujici pole chybovych kodu a argumenty programu
typedef struct{
    int err[7];			// pole pro chybove kody
    long passengers;	// pocet pasazeru
    long capacity;		// kapacita voziku
    long genTime;		// maximalni doba generovani novych procesu
    long rideTime;		// maximalni doba jizdy
} param_set;

// Datova struktura obsahujici semafory pro synchronizaci procesu
typedef struct{
	sem_t *wrt;		// ridi pozadaveky pro zapis
    sem_t *exit;	// ridi ukoncovani procesu pasazeru
    sem_t *ride;	// ridi pozadaveky voziku pro jizdu
    sem_t *board;	// ridi pozadaveky pro nastup pasazeru
    sem_t *unboard;	// ridi pozadaveky pro vystup pasazeru
    sem_t *done;	// ridi pozadaveky pro dalsi iteraci procesu vozik
} sem_set;

// Globalni deklarovani semaforu
sem_set semafor;
		  
/// ******************************************************************************************************** ///
/// ******************************************* Chyby a hlaseni ******************************************** ///

// Vycet chyb, ktere mohou nastat
enum errors
{
    ERR_OK,		// bez chyby, je-li hodnota 0
    ERR_INPUT,	// vyskyt chyby v poctu parametru
    ERR_PROC,	// vyskyt chyby v poctu procesu
    ERR_CAP,	// vyskyt chyby v kapacite voziku 
    ERR_MUL,	// P neni nasobkem C
    ERR_GTIME,	// vyskyt chyby v casu generovani 
    ERR_RTIME	// vyskyt chyby v casu jizdy
};

// Vycet chybovych hlasek
const char *EMSG[] =
{
    "  -- OK",
    "  -- Neplatny pocet argumentu!",
    "  -- Neplatna hodnota poctu procesu!",
    "  -- Neplatna hodnota kapacity voziku!",
    "  -- Chyba! Pocet pasazeru neni vetsi nebo neni nasobkem kapacity voziku!",
    "  -- Neplatna hodnota maximalni doby generovani procesu!",
    "  -- Neplatna hodnota maximalni doby prujezdu trati!"
};

/// ******************************************************************************************************** ///
/// ******************************************* Deklarace Funkci ******************************************* ///

long timeGenerate(long maxTime);				// funkce na vygenerovani nahodneho casu
param_set inputCheck(int argc, char *argv[]);	// funkce na zpracovani parametru programu
int doCartStuff();			// funkce zpracovavajici cinnost voziku
int doPassengerStuff();	// funkce zpracovavajici cinnost pasazera
int initSemaphores();		// funkce na inicializaci semaforu
void destroySemaphores();	// funkce na uvolneni semaforu

/// ******************************************************************************************************** ///
/// ******************************************** Hlavni program ******************************************** ///
/// ******************************************************************************************************** ///

int main(int argc, char *argv[])
{
	// Promenne pro kontrolu uspechu systemovych volani
	int check = 0;
	
	// Vytvoreni seedu podle aktualniho casu
	srand((unsigned)time(0));	// slouzi pro nahodne generovani cisel
	
	// Kontrola a zpracovani parametru
    param_set roller = inputCheck(argc, argv);
	
	// Pokud se nevyskytla zadna chyba ve zpracovani parametru, program pokracuje.
	if( roller.err[0] == ERR_OK )
	{	
		int shmID;			// promenna pro praci se sdilenou pameti
		int *sharedMemory;	// promenna pro praci se sdilenou pameti
		pid_t procPid;		// promenna pro identifikaci procesu
		pid_t cartPID;		// promenna pro ulozeni PID voziku
		pid_t genProcPID;	// promenna pro ulozeni PID plodice deti
		FILE *output;		// promenna pro presmerovani vystupu do souboru
		
		// vytvoreni a otevreni souboru pro zapis vystupu procesu a nasledna kontrola uspesnosti
		if( (output = fopen("proj2.out", "w")) == NULL )
		{
			fprintf(stderr, "  -- Nepodarilo se vytvorit soubor 'proj2.out'!\n");
			return 2;
		}
		
		// Nastaveni bufferu
		setbuf(stdout, NULL);
		setbuf(stderr, NULL);
		setbuf(output, NULL);
		
		// Inicializace semaforu a nasledna kontrola uspesnosti
		if( (check = initSemaphores()) != 0)
		{
			fclose(output);
			fprintf(stderr, "  -- Nepodarilo se nainicializovat semafory!\n");
			return 2;
		}
		
		// Alokace pomocneho pole pro uchovani PID vsech pasazeru a nasledna kontrola uspesnosti
		pid_t *passPID = malloc(roller.passengers * sizeof(pid_t));
		if (passPID == NULL)
		{	
			// Uvolneni doposud naalokovanych zdroju a nasledne ukonceni s navratovou hodnotou 2
			fclose(output);
			destroySemaphores();
			fprintf(stderr, "  -- Chyba alokace!\n");
			return 2;
		}
		
		// Vytvoreni objektu ve sdilene pameti a nasledna kontrola uspesnosti
		shmID = shm_open(shmName, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
		if (shmID < 0)
		{
			// Uvolneni doposud naalokovanych zdroju a nasledne ukonceni s navratovou hodnotou 2
			fclose(output);
			free(passPID);
			destroySemaphores();
			fprintf(stderr, "  -- Nepodarilo se vytvorit objekt ve sdilene pameti!\n");
			return 2;
		}

		// Oriznuti objektu ve sdilene pameti na pozadovanou velikost a nasledna kontrola uspesnosti
		if( ftruncate(shmID, shmSize) < 0 )
		{
			// Uvolneni doposud naalokovanych zdroju a nasledne ukonceni s navratovou hodnotou 2
			freeAll
			close(shmID);
			fprintf(stderr, "  -- Nepodarilo se upravit velikost sdilene pameti!\n");
			return 2;
		}
		
		// Mapovani pole o ctyrech polozkach do sdilene pameti a nasledna kontrola uspesnosti
		sharedMemory = mmap(NULL, shmSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);
		if( sharedMemory == MAP_FAILED )
		{
			// Uvolneni doposud naalokovanych zdroju a nasledne ukonceni s navratovou hodnotou 2
			freeAll
			close(shmID);
			fprintf(stderr, "  -- Nepodarilo se namapovat pole do sdilene pameti!\n");
			return 2;
		}
		close(shmID); // muzeme uzavrit, dalsi prace az ve funkci, kde se znovu otevre
		
		// Inicializace pole ve sdilene pameti
		sharedMemory[0] = 1;				// pocitadlo akci
		sharedMemory[1] = 1;				// pocitadlo procesu
		sharedMemory[2] = roller.capacity;	// volne misto ve voziku
		sharedMemory[3] = 1;				// cislo listku ve voziku
		sharedMemory[4] = 0;				// cislo procesu pro ukonceni
		munmap(sharedMemory, shmSize);		// muzeme odmapovat, dalsi prace az ve funkci, kde se znovu namapuje
		
		// Prvni fork procesu, vytvoreni procesu voziku a nasledna kontrola uspesnosti
		procPid = fork();
		if( procPid < 0 )		// chyba pri pokusu o fork
		{
			// Uvolneni doposud naalokovanych zdroju a nasledne ukonceni s navratovou hodnotou 2
			freeAll
			fprintf(stderr, "  -- Nastala chyba pri pokusu o fork!\n");
			return 2;
		}
		else if( procPid == 0 )	// child proces = proces voziku
		{
			check = doCartStuff(roller, output);	// volani funkce pro spravu cinnosti voziku
			if( check != 0 )						// kontrola uspesnosti funkce
			{
				for(long i = 0; i < roller.passengers; i++) kill(passPID[i], SIGKILL);	// ukonceni vsech procesu pasazeru
				fprintf(stderr, "  -- Nastala chyba pri zpracovavani cinnosti voziku!\n");
				exit(2);
			}
			exit(0);	// ukonceni procesu voziku
		}
		else // parent process = hlavni proces
		{	
			cartPID = procPid;	// ulozeni PID voziku
			// Druhy fork procesu, vytvoreni procesu plodice deti a nasledna kontrola uspesnosti
			procPid = fork();
			if( procPid < 0 )		// chyba pri pokusu o fork
			{	
				kill(cartPID, SIGKILL);	// ukonceni procesu voziku
				freeAll					// uvolneni doposud naalokovanych zdroju a nasledne ukonceni s navratovou hodnotou 2
				fprintf(stderr, "  -- Nastala chyba pri pokusu o fork!\n");
				return 2;
			}
			else if( procPid == 0 )	// child proces = pomocny proces pro generovani procesu pasazeru ( plodic deti )
			{
				check = 0; // nastaveni hodnoty pro naslednou kontrolu uspesnosti
				for(long i = 0; i < roller.passengers; i++) // plodime deti
				{
					if( roller.genTime > 0 )
					{
						usleep( timeGenerate(roller.genTime) * 1000 );	// vygenerovani nahodneho casu z intervalu <0,PT>, pokud PT neni 0 ( vynasobeno 1000 kvuli prevodu na ms )
					}
					procPid = fork();		// vytvoreni procesu pasazera
					if( procPid < 0 ) 		// chyba pri pokusu o fork
					{
						check++; // inkrementovani kontrolni hodnoty
						kill(cartPID, SIGKILL);	// ukonceni procesu voziku
						for(long x = 0; x < i; x++) kill(passPID[x], SIGKILL);	// ukonceni vsech doposud vytvorenych procesu pasazeru
						fprintf(stderr, "  -- Nastala chyba pri pokusu o fork!\n");
						exit(2);
					}
					else if( procPid == 0 )	// child proces = pasazer
					{
						check = doPassengerStuff(roller, output);	// volani funkce pro spravu cinnosti pasazera
						if( check != 0 )							// kontrola uspesnosti funkce
						{
							check++;	// inkrementovani kontrolni hodnoty
							fprintf(stderr, "  -- Nastala chyba pri zpracovavani cinnosti pasazera!\n");
							exit(2);
						}
						exit(0);				// ukonceni procesu pasazera
					}
					else						// parent process = plodic deti
					{
						passPID[i] = procPid; 	// ulozeni PID procesu do pomocneho pole
					}
					
					// kontrola, zdali se nevyskytla chyba, jestli se vyskytla v promenne check nebude 0
					if( check != 0) 
					{
						kill(cartPID, SIGKILL);	// ukonceni procesu voziku
						for(long x = 0; x < i; x++) kill(passPID[x], SIGKILL);	// ukonceni vsech doposud vytvorenych procesu pasazeru
						exit(2);
					}
				}
				// Čekani na ukonceni posledniho procesu pasazera
				waitpid(passPID[roller.passengers - 1], NULL, 0); // -1 kvuli indexovani od 0
				exit(0);	// ukonceni procesu plodice deti			
			}
			else // parent process = hlavni proces
			{
				genProcPID = procPid;	// ulozeni PID plodice deti do pomocne promenne pro naslednou synchronizaci
				// Čekani na ukonceni plodice deti
				waitpid(genProcPID, NULL, 0);
				// Uvolneni vsech zdroju vcetne pripadu, kdy nastala nejaka chyba v plodici deti
				freeAll
			}
		}
	}
    else	// Pokud se vyskytla jedna ci vice chyb, program je vytiskne a ukonci se s navratovou hodnotou 1. 
	{
		if( roller.err[1] == ERR_INPUT )
		{
			fprintf(stderr, "%s\n", EMSG[roller.err[ERR_INPUT]]);
			return 1;
		}
		for(int i = 2; i < 7; i++)
		{
			if( roller.err[i] != 0 ) fprintf(stderr, "%s\n", EMSG[roller.err[i]]);
		}
        return 1;
	}
	
	if( check != 0) return 2; // ukonceni hlavniho procesu, pokud doslo pri vytvareni pasazeru k chybe
	return 0;	// ukonceni hlavniho procesu, pokud nedoslo k chybe
}

/// ******************************************************************************************************** ///
/// ******************************** Funkce na vygenerovani nahodneho casu ********************************* /// 

long timeGenerate(long maxTime)
{	
    return ( rand() % maxTime) + 1 ;	// vraci vygenerovanou hodnotu z intervalu <0,maxTime>
}

/// ******************************************************************************************************** ///
/// ******************************* Funkce na zpracovani parametru programu ******************************** ///

/* 	Spravne spusteni programu je: ./proj2 P C PT RT, kde 
		P je pocet procesu reprezentujicich pasazera; P > 0.
		C je kapacita voziku; C > 0, P > C, P musi byt vzdy nasobkem C.
		PT je maximalni hodnota doby (v milisekundach), po ktere je generovan novy proces pro pasazera; PT >= 0 && PT < 5001. 
		RT je maximalni hodnota doby (v milisekundach) prujezdu trati; RT >= 0 && RT < 5001.
		Vsechny parametry jsou cela cisla. */
param_set inputCheck(int argc, char *argv[])
{
	param_set checked = {.passengers = 0, .capacity = 0, .genTime = 0, .rideTime = 0}; // pomocna struktura pro pocatecni inicializaci
	for(int i = 0; i < 8; i++) checked.err[i] = 0;	// inicializace vsech polozek pole chybovych kodu na 0
	if(argc!=5)	// je-li pocet argumentu ruzny od 5, nastavi se chyby a funkce vrati strukturu
	{
		checked.err[ERR_OK] = ERR_INPUT;
		checked.err[ERR_INPUT] = ERR_INPUT;
        return checked;
	}
	// kontrola vsech zadanych parametru, zdali to jsou cisla, nejsou zaporne a odpovidaji zadani
	char *chyba;
	checked.passengers = strtol(argv[1], &chyba, 0);
	if(*chyba != '\0' || checked.passengers <= 0 )
	{
		checked.err[ERR_OK] = ERR_PROC;
		checked.err[ERR_PROC] = ERR_PROC;
	}
	
	checked.capacity = strtol(argv[2], &chyba, 0);
	if(*chyba != '\0' || checked.capacity <= 0 )
	{
		checked.err[ERR_OK] = ERR_CAP;
		checked.err[ERR_CAP] = ERR_CAP;
	}
	if( (checked.err[ERR_PROC]) == 0 && (checked.err[ERR_CAP] == 0) )
	{
		if( (checked.passengers % checked.capacity) != 0 )
		{
			checked.err[ERR_OK] = ERR_MUL;
			checked.err[ERR_MUL] = ERR_MUL;
		}
		if( checked.passengers == checked.capacity )
		{
			checked.err[ERR_OK] = ERR_MUL;
			checked.err[ERR_MUL] = ERR_MUL;
		}
	}
	
	checked.genTime = strtol(argv[3], &chyba, 0);
	if(*chyba != '\0' || checked.genTime < 0 || checked.genTime > 5000)
	{
		checked.err[ERR_OK] = ERR_GTIME;
		checked.err[ERR_GTIME] = ERR_GTIME;
	}

		
	checked.rideTime = strtol(argv[4], &chyba, 0);
	if(*chyba != '\0' ||  checked.rideTime < 0 || checked.rideTime > 5000 )
	{
		checked.err[ERR_OK] = ERR_RTIME;
		checked.err[ERR_RTIME] = ERR_RTIME;
	}
	
	return checked;
}

/// ******************************************************************************************************** ///
/// *********************************** Funkce na inicializaci semaforu ************************************ ///

int initSemaphores()
{
	// Postupne vytvareni semaforu a nasledna kontrola, zdali se akce povedla
	// Pri vyskytu chyby se uvolni vsechny predchozi uspesne vytvorene semafory a funkce vrati -1
	semafor.wrt = sem_open(semName1, O_CREAT | O_EXCL, 0666, 1); 		// semafor poskytujici vylucne opravneni pro zapis
	if( semafor.wrt == SEM_FAILED )
	{ 
		return -1;
	}
	semafor.exit = sem_open(semName2, O_CREAT | O_EXCL, 0666, 0); 		// semafor ridici ukoncovani procesu pasazeru, inicializovan na 0, aby se hned neukoncili
	if( semafor.exit == SEM_FAILED )
	{ 
		sem_close(semafor.wrt);
		sem_unlink(semName1);
		return -1;
	}
	semafor.ride = sem_open(semName3, O_CREAT | O_EXCL, 0666, 0); 		// semafor ridici povoleni k jizde, inicializovan na 0, aby vozik hned neodjel 
	if( semafor.ride == SEM_FAILED )
	{ 
		sem_close(semafor.wrt);
		sem_close(semafor.exit);
		sem_unlink(semName1);
		sem_unlink(semName2);
		return -1;
	}
	semafor.board = sem_open(semName4, O_CREAT | O_EXCL, 0666, 0);		// semafor ridici nastupovani pasazeru, inicializovan na 0, aby pasazeri nenastoupili do nepripraveneho voziku
	if( semafor.board == SEM_FAILED )
	{
		sem_close(semafor.wrt);
		sem_close(semafor.exit);
		sem_close(semafor.ride);
		sem_unlink(semName1);
		sem_unlink(semName2);
		sem_unlink(semName3);
		return -1;
	}
	semafor.unboard = sem_open(semName5, O_CREAT | O_EXCL, 0666, 0);	// semafor ridici vystupovani pasazeru, inicializovan na 0, aby pasazer hned nevystoupil
	if( semafor.unboard == SEM_FAILED )
	{ 	
		sem_close(semafor.wrt);
		sem_close(semafor.exit);
		sem_close(semafor.ride);
		sem_close(semafor.board);
		sem_unlink(semName1);
		sem_unlink(semName2);
		sem_unlink(semName3);
		sem_unlink(semName4);
		return -1;
	}
	semafor.done = sem_open(semName6, O_CREAT | O_EXCL, 0666, 0);		// semafor ridici povoleni pro dalsi iteraci procesu vozik
	if( semafor.done == SEM_FAILED )
	{ 	
		sem_close(semafor.wrt);
		sem_close(semafor.exit);
		sem_close(semafor.ride);
		sem_close(semafor.board);
		sem_close(semafor.unboard);
		sem_unlink(semName1);
		sem_unlink(semName2);
		sem_unlink(semName3);
		sem_unlink(semName4);
		sem_unlink(semName5);
		return -1;
	}
	// Povede-li se vse bez chyby, vrati 0
	return 0;
}

/// ******************************************************************************************************** ///
/// ************************************* Funkce na uvolneni semaforu ************************************** ///

void destroySemaphores()
{
	sem_close(semafor.wrt);
	sem_close(semafor.exit);
	sem_close(semafor.ride);
	sem_close(semafor.board);
	sem_close(semafor.unboard);
	sem_close(semafor.done);
	sem_unlink(semName1);
	sem_unlink(semName2);
	sem_unlink(semName3);
	sem_unlink(semName4);
	sem_unlink(semName5);
	sem_unlink(semName6);
}	

/// ******************************************************************************************************** ///
/// ******************************** Funkce zpracovavajici cinnost voziku ********************************** ///	
	
int doCartStuff(param_set roller, FILE *output)
{
	int shmID;				// promenna pro praci se sdilenou pameti
	int *sharedMemory;		// promenna pro praci se sdilenou pameti
	shmID = shm_open(shmName, O_RDWR, S_IRUSR | S_IWUSR);	// otevreni objektu ve sdilene pameti a nasledna kontrola uspesnosti
	if (shmID < 0)
	{
		return -1;
	}
	sharedMemory = mmap(NULL, shmSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);	// namapovani objektu ve sdilene pameti a nasledna kontrola uspesnosti
	if( sharedMemory == MAP_FAILED )
	{
		close(shmID);
		return -1;
	}
	close(shmID);	// nyni uz muzeme uzavrit
	
	// Vyhrazeni si vylucneho prava pro zapis + samotny zapis
		sem_wait(semafor.wrt);
		fprintf(output, "%d\t: C 1\t: started\n", sharedMemory[0]++);
		sem_post(semafor.wrt);
	
	// Urceni poctu iteraci potrebnych pro prevezeni vsech pasazeru
	long iterNum = roller.passengers / roller.capacity;

	for(long i = 0; i < iterNum; i++)	// hlavni cyklus prevazeni pasazeru
	{
		// Vyhrazeni si vylucneho prava pro zapis + samotny zapis
			sem_wait(semafor.wrt);
			fprintf(output, "%d\t: C 1\t: load\n", sharedMemory[0]++);
			sem_post(semafor.wrt);
			
		sem_post(semafor.board);	// udeleni povoleni k nastupu
		sem_wait(semafor.ride);		// zadost o povoleni k jizde
		
		// Zakazani nastupovani kvuli plne kapacite ridi posledni pasazer
		// Vyhrazeni si vylucneho prava pro zapis + samotny zapis
			sem_wait(semafor.wrt);
			fprintf(output, "%d\t: C 1\t: run\n", sharedMemory[0]++);
			sem_post(semafor.wrt);
			
		if( roller.rideTime > 0 )
		{
			usleep( timeGenerate(roller.rideTime) * 1000 );	// vygenerovani nahodneho casu z intervalu <0,RT>, pokud RT neni 0 ( vynasobeno 1000 kvuli prevodu na ms )
		}
		// Vyhrazeni si vylucneho prava pro zapis + samotny zapis
			sem_wait(semafor.wrt);
			fprintf(output, "%d\t: C 1\t: unload\n", sharedMemory[0]++);
			sem_post(semafor.wrt);

		sem_post(semafor.unboard);	// udeleni povoleni k vystoupeni
		sem_wait(semafor.done);		// podani zadosti k provedeni dalsi iterace
	}
	// inkrementování hodnoty pro pousouzení možnosti o ukončení
	sharedMemory[4]++;
	
	if( sharedMemory[4] == (roller.passengers + 1) )	// pokud je pocitadlo rovno poctu procesu pasazeru a voziku ( roller.passengers + 1 ) jedna se o posledni proces
	{
		for(long i = 0; i < roller.passengers; i++)	// otevreni semaforu pro vsechny predchozi procesy
			sem_post(semafor.exit);
	}
	else
	{
		sem_wait(semafor.exit);	// jinak se ceka na posledni proces
	}
	
	// Vyhrazeni si vylucneho prava pro zapis + samotny zapis
		sem_wait(semafor.wrt);
		fprintf(output, "%d\t: C 1\t: finished\n", sharedMemory[0]++);
		sem_post(semafor.wrt);
		
	munmap(sharedMemory, shmSize);	// odmapovani objektu ve sdilene pameti a nasledna kontrola
	
	return 0;	// pri uspechu vraci funkce 0
}

/// ******************************************************************************************************** ///
/// ******************************* Funkce zpracovavajici cinnost pasazera ********************************* ///

int doPassengerStuff(param_set roller, FILE *output)
{
	int shmID;				// promenna pro praci se sdilenou pameti
	int *sharedMemory;		// promenna pro praci se sdilenou pameti
	shmID = shm_open(shmName, O_RDWR, S_IRUSR | S_IWUSR);	// otevreni objektu ve sdilene pameti a nasledna kontrola uspesnosti
	if (shmID < 0)
	{
		return -1;
	}
	sharedMemory = mmap(NULL, shmSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);	// namapovani objektu ve sdilene pameti a nasledna kontrola uspesnosti
	if( sharedMemory == MAP_FAILED )
	{
		close(shmID);
		return -1;
	}
	close(shmID);	// nyni uz muzeme uzavrit
	
	// Vyhrazeni si vylucneho prava pro zapis + samotny zapis
		sem_wait(semafor.wrt);
		long procNum = sharedMemory[1]++;	// pomocna promenna, ktera udava cislo procesu
		fprintf(output, "%d\t: P %ld\t: started\n", sharedMemory[0]++, procNum );
		sem_post(semafor.wrt);
		
	sem_wait(semafor.board); // zadost o povoleni k nastupu

	// Vyhrazeni si vylucneho prava pro zapis + samotny zapis
		sem_wait(semafor.wrt);
		fprintf(output, "%d\t: P %ld\t: board\n", sharedMemory[0]++, procNum );
		sem_post(semafor.wrt);
		
	if( sharedMemory[2] == 1 )	// pokud je ve voziku posledni volne misto
	{
		// Vyhrazeni si vylucneho prava pro zapis + samotny zapis
			sem_wait(semafor.wrt);
			fprintf(output, "%d\t: P %ld\t: board last\n", sharedMemory[0]++, procNum );
			sem_post(semafor.wrt);

		sharedMemory[2]--; 		// snizeni poctu volnych mist ve voziku
		sharedMemory[3] = 1;	// "restartovani" listku
		// udeleni povoleni k jizde
		sem_post(semafor.ride);
	}
	else
	{	
		// Vyhrazeni si vylucneho prava pro zapis + samotny zapis
			sem_wait(semafor.wrt);
			fprintf(output, "%d\t: P %ld\t: board order %d\n", sharedMemory[0]++, procNum, sharedMemory[3]++ );
			sem_post(semafor.wrt);
			
		sharedMemory[2]--; // snizeni poctu volnych mist ve voziku
		// dalsi muze nastupvat
		sem_post(semafor.board);
	}
	// pozadavek pro vystoupeni z voziku
	sem_wait(semafor.unboard);
	// Vyhrazeni si vylucneho prava pro zapis + samotny zapis
		sem_wait(semafor.wrt);
		fprintf(output, "%d\t: P %ld\t: unboard\n", sharedMemory[0]++, procNum );
		sem_post(semafor.wrt);
		
	if( sharedMemory[2] == (roller.capacity - 1) ) // posledni pasazer ve voziku ( -1 kvuli tomu, ze tento proces je ten, ktery drzi posledni obsazene misto )
	{
		// Vyhrazeni si vylucneho prava pro zapis + samotny zapis
			sem_wait(semafor.wrt);
			fprintf(output, "%d\t: P %ld\t: unboard last\n", sharedMemory[0]++, procNum );
			sem_post(semafor.wrt);
			
		sharedMemory[2]++; 		// zvyseni poctu volnych mist ve voziku
		sharedMemory[3] = 1; 	// "restartovani" listku
		sem_post(semafor.done);	// udeleni povoleni k nastupu pro dalsi procesy
	}
	else
	{	
		// Vyhrazeni si vylucneho prava pro zapis + samotny zapis
			sem_wait(semafor.wrt);
			fprintf(output, "%d\t: P %ld\t: unboard order %d\n", sharedMemory[0]++, procNum, sharedMemory[3]++ );
			sem_post(semafor.wrt);
			
		sharedMemory[2]++; // zvyseni poctu volnych mist ve voziku
		sem_post(semafor.unboard);	// dalsi muze nastupvat
	}
	// inkrementování hodnoty pro pousouzení možnosti o ukončení
	sharedMemory[4]++;
	
	if( sharedMemory[4] == (roller.passengers + 1) )	// pokud je pocitadlo rovno poctu procesu pasazeru a voziku ( roller.passengers + 1 ) jedna se o posledni proces
	{
		for(long i = 0; i < roller.passengers; i++)	// otevreni semaforu pro vsechny predchozi procesy
			sem_post(semafor.exit);
	}
	else
	{
		sem_wait(semafor.exit);	// jinak se ceka na posledni proces
	}
	// Vyhrazeni si vylucneho prava pro zapis + samotny zapis
		sem_wait(semafor.wrt);
		fprintf(output, "%d\t: P %ld\t: finished\n", sharedMemory[0]++, procNum );
		sem_post(semafor.wrt);
		
	munmap(sharedMemory, shmSize);	// odmapovani sdilene pameti
	
	return 0;	// pri uspechu vraci funkce 0
}

/// ******************************************************************************************************** ///
/// ******************************************************************************************************** ///