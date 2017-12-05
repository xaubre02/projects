/**************************************
* -+-+-+-+-+-+-+-+-+-+-+-+-
* - Predmet: IMS
* - Projekt: Vyroba vina
* - Datum: 2017-12-01
* - Autor: Tomas Aubrecht
* - Login: xaubre02
* -+-+-+-+-+-+-+-+-+-+-+-+-
***************************************/
#pragma once

// Casove konstanty - zakladni casova jednotka je sekunda
// -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
#define MIN    * 60
#define HOUR   MIN * 60
#define HOURS  HOUR
#define DAY    HOURS * 24
#define DAYS   DAY
// -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


#define POCET_SBERACU 4U
#define POCET_PRACOVNIKU 1U


#define OUTPUT "vinobrani.out"
#define RUNTIME 3 DAYS


#define KAPACITA_AUTA 12    // Do auta se vleze 12 beden
#define KAPACITA_BEDNY 25   // Jedna bedna obsahuje 25 kilo hroznu

#define KAPACITA_LISU 200

// objekty pro zaznam statistik
Histogram delka_vyroby("Delka prace pracovnika", 0, 30 MIN, 20);
Stat doba_sberu("Doba sberu");
Stat doba_zpracovani("Doba zpracovani");
Stat doba_pauzy("Doba stravena na pauze");

Facility Auto("Pick-up pro prepravu hroznu");
Facility Odzrnovac("Odzrnovac");
Facility Lis("Vodni lis");

Store Hadice("Hadice", 3);
Store Kolecka("Kolecka", 2);

uint bedny_ke_zpracovani = 0;
uint sklizenych_beden = 0;

uint sklizene_hrozny = 0;
uint zrale_hrozny = 600;

uint odpad = 0;
uint rmut = 0;
uint most = 0;

uint sberacu = 0;
uint pracovniku = 0;

uint sudy = 0;

bool sber_ukoncen = false;
bool transport_ukoncen = false;
bool zpracovani_ukonceno = false;
