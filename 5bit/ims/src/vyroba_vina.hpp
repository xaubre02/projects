#pragma once
/**************************************
* -+-+-+-+-+-+-+-+-+-+-+-+-
* - Predmet: IMS
* - Projekt: Vyroba vina
* - Datum: 2017-12-01
* - Autor: Tomas Aubrecht
* - Login: xaubre02
* -+-+-+-+-+-+-+-+-+-+-+-+-
***************************************/

#include "simlib.h"
#include <string>


// Casove konstanty - zakladni casova jednotka je minuta
// -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
#define SEC    / 60
#define HOUR   * 60
#define HOURS  HOUR
#define DAY    HOURS * 24
#define DAYS   DAY
// -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


Facility Auto("Pracovni auto pro prepravu hroznu");
Facility Odzrnovac("Odzrnovac");
Facility Lis("Vodni lis");
Facility Cerpadlo("Cerpadlo");

Store Kolecka("Kolecka", 2);

// vychozi parametry programu
std::string output("experiment.out");
uint pocet_sberacu = 3;
uint kapacita_auta = 10;
uint kapacita_lisu = 120;

// meziprodukty procesu vyroby vina
uint zrale_hrozny = 750;
uint sklizenych_beden = 0;
uint bedny_ke_zpracovani = 0;

uint odpad = 0;
uint rmut = 0;
uint most = 0;

// priznaky stavu cinnosti
bool sber_ukoncen = false;
bool transport_ukoncen = false;
bool zpracovani_ukonceno = false;

// Trida reprezentujici pracovnika
class Pracovnik : public Process 
{
    double time;                // zaznamenani pocatecniho casu zpracovavani
    double pauza;               // zaznamenani pocatecniho casu pauzy
    
    void Behavior(void);        // chovani pracovnika
    void Odzrnit(void);         // odzrneni jedne bedny hroznu
    void Lisovat(void);         // lisovani rmutu
    void Precerpat(void);       // precerpani mostu do sudu
    void ZpracovatOdpad(void);  // vyvezeni odpadu
    void DatSiPauzu(void);      // pracovnik je na prestavce
};

// Trida reprezentujici sberace
class Sberac : public Process 
{
    double time;                // zaznamenani pocatecniho casu sbirani
    
    void Behavior(void);        // chovani sberace
    void Sklidit(void);         // sklizeni hroznu
    void Prepravit(void);       // prepraveni beden s hroznama ke zpracovani
    void PremistitSe(void);     // presun sberace ke zpracovani -> novy proces pracovnika, proces sberace konci
};

void parseParams(int argc, char* argv[]); // funkce pro zakladni zpracovani argumentu programu
void printData(void);           // funkce pro vypis ziskanych hodnot

// objekty pro zaznam statistik
Stat doba_sberu("Doba sberu");
Stat doba_zpracovani("Doba zpracovani");
Stat doba_pauzy("Doba stravena na pauze");
