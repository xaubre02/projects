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

// Casove konstanty - zakladni casova jednotka je minuta
// -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
#define HOUR  * 60         // hodina -> 60 minut
#define DAY   HOURS * 24   // den    -> 24 hodin
/*#define MONTH DAYS  * 30   // mesic  -> 30 dni
#define YEAR  DAYS  * 365  // rok    -> 365 dni*/

#define HOURS  HOUR
#define DAYS   DAY/*
#define MONTHS MONTH
#define YEARS  YEAR*/
// -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


#define OUTPUT "wine.out"
#define RUNTIME 1 DAY
#define PRIJEZD_HROZNU 25

#define PICKERS 8
#define POCET_KERU 960

#define KAPACITA_AUTA 300
#define KAPACITA_LISU 200
