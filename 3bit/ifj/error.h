/*****
 * Název projektu:
 *	Implementace interpretu imperativního jazyka IFJ16
 *
 * Zadání:
 * 	https://wis.fit.vutbr.cz/FIT/st/course-files-st.php/course/IFJ-IT/projects/ifj2016.pdf
 *
 * Tým:
 * 	022, varianta a/3/II
 *
 * Členové:
 *	Vít Ambrož		(xambro15)
 *	Tomáš Aubrecht	(xaubre02)
 *	David Bednařík	(xbedna62)
 *	Andrej Čulen		(xculen02)
 *	
 * Odkaz na repozitář:
 * 	https://github.com/xbedna62/IFJ16
 *
 * Modul:
 * 	Error
 *
 * Autor: 
 * 	Tomáš Aubrecht (xaubre02)
 *
 *****/


#ifndef _ERROR_H_
#define _ERROR_H_

#include <stdio.h>


// výčet všech chyb
typedef enum
{
	ERR_NONE,	// bez chyby
	ERR_LEX,	// chyba lexikalni analýzy
	ERR_SYN,	// chyba syntaktické analýzy
	ERR_SEM1,	// chyba sémantická, nedefinovaná třída/funkce/proměnná, pokus o redefinici třídy/funkce/proměnné
	ERR_SEM2,	// chyba sémantická, typová kompatibilita v aritmetických, řetězcových a relačních výrazech, případně špatný počet či typ parametrů u volání funkce
  ERR_SEM3 = 6, // ostatní semantické chyby
  ERR_RUN1 = 7, // běhová chyba při načitání číslené hodnoty ze vstupu
  ERR_RUN2 = 8, // běhová chyba při práci s neinicializovanou proměnnou
  ERR_RUN3 = 9, // běhová chyba dělení nulou
  ERR_RUN4 = 10, // ostatní běhové chyby
  ERR_OTHER = 99 // interní chyba překladače
} ErrorType;

// globalní proměnné
extern ErrorType Err;

// funkce na zpracování chyby
int checkError();

#endif
