/*****
 * Název projektu:
 *  Implementace interpretu imperativního jazyka IFJ16
 *
 * Zadání:
 *  https://wis.fit.vutbr.cz/FIT/st/course-files-st.php/course/IFJ-IT/projects/ifj2016.pdf
 *
 * Tým:
 *  022, varianta a/3/II
 *
 * Členové:
 *  Vít Ambrož      (xambro15)
 *  Tomáš Aubrecht  (xaubre02)
 *  David Bednařík  (xbedna62)
 *  Andrej Čulen        (xculen02)
 *  
 * Odkaz na repozitář:
 *  https://github.com/xbedna62/IFJ16
 *
 * Modul:
 *  ial.h (implementace algoritmů pro předmět IAL)
 *
 * Autor: 
 *  Vít Ambrož (xambro15) - Hashovací tabulka
 *  David Bednařík (xbedna62) - Knuth-Morris-Prattův algoritmus
 *	Tomáš Aubrecht (xaubre02) - Shell-Sort
 *
 *****/


#ifndef _IAL_H_
#define _IAL_H_

#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "tsymbols.h"

//Maximální velikost pole pro implementaci vyhledávací tabulky
#define MAX_HTSIZE 59


//Datová položka TRP s explicitně řetězenými synonymy
 typedef struct s_tHTItem{
    struct s_Class *data;            //veskera data  
    struct s_tHTItem *ptrnext;         //ukazatel na další synonymum
    char* key;                         //klíčem je název identifikátoru
} tHTItem;

//TRP s explicitně zřetězenými synonymy
typedef struct s_tHTItem* tHTable[MAX_HTSIZE];

extern int HTSIZE;

//Funkce pro Hashovací tabulku
unsigned int hashCode ( char* key );

void htInit ( tHTable* ptrht );

tHTItem* htSearch ( tHTable* ptrht, char* key );

void htInsert ( tHTable* ptrht, char* key, Class *data );

Class* htRead ( tHTable* ptrht, char* key );

void htDelete ( tHTable* ptrht, char* key );

void htClearAll ( tHTable* ptrht );

// Knuth-Morris-Prattův algoritmus *********************************************************

int *KMPGrag(char *p, int pl);

int KMPMatch(char *t, int tl, char *p, int pl);

// Shell-Sort řazení ***********************************************************************

const char* ShellSort(char *string);

#endif
