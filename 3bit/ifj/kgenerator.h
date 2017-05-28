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
 * 	Pomocné funkce pro generování kódu
 *
 * Autor: 
 * 	David Bednařík (xbedna62)
 *
 *****/

#ifndef _KGENERATOR_H_
#define _KGENERATOR_H_

#include "tsymbols.h"

typedef enum{
  I_NOP,
  // aritmetické instrukce
  I_MUL,
  I_DIV,
  I_PLUS,
  I_MINUS,
  I_LESS,
  I_GREATER,
  I_ASSIGN,
  I_LEQUAL,
  I_GEQUAL,
  I_SAME,
  I_NOT_EQUAL,

  // vestavěné funkce
  I_READ_INT,
  I_READ_DOUBLE,
  I_READ_STRING,
  I_PRINT,
  I_LENGTH,
  I_SUBSTR1,
  I_SUBSTR2,
  I_COMPARE,
  I_FIND,
  I_SORT,

  // skoky pro if
  I_TJUMP,
  I_FJUMP,

  // skoky pro while
  I_WJUMP,
  I_WFJUMP,

  // instrukce pro funkce
  I_FCEJUMP,
  I_FCEVJUMP,
  I_RETURN
} tInstr;
//-----------------------------------------------------------------------------------------
struct tLabel;
struct tListItem;

// Seznam
typedef struct tListItem{ // prvek seznamu
  tInstr ins;
  struct sVariable_Data *op1;
  struct sVariable_Data *op2;
  struct sVariable_Data *result;
  int jump;
  char *label;
  struct tListItem *rptr;
  struct tListItem *lptr;
} tListItem;

typedef struct tList{ // seznam
  struct tListItem *First;
  struct tListItem *Last;
  struct tListItem *Act;
} tList;
//-----------------------------------------------------------------------------------------
// Tabulka
typedef struct tTabLabel{ // struktura obsahující pole labelů plus proměné capacita pole a kolik prvků pole je obsazeno
  int cap;
  int used;
  struct tLabel *tl;
} tTabLabel;

typedef struct tLabel{ // prvek pole labelů, obsahuje label a seznam instrukcí
  char *label;
  struct tList l;
} tLabel;
//-----------------------------------------------------------------------------------------
// Zásobník
struct tItemL;

typedef struct tStackLabel{
  struct tItemL *top;                             
	int count;                                
} tStackLabel;

typedef struct tItemL{
  char *label;
  int typeFun;
  int end;
  int count;
  struct tItemL *lptr;
} tItemL;

// pomocná proměná pro vyčištění List itemu
void clearListItem(struct tListItem *item);

// inicializace List itemu
void initListItem(struct tListItem *item);

// Práce se zásobníkem ***********************************************************************************

// Inicializuje zasobník
void initStackLabel(struct tStackLabel *s);

// Vrácí 1 pokud je zásobník prázdný jínak vrací 0
int emptyStackLabel(const struct tStackLabel *s);

// Do proměnné c vloží data z vrcholu zásobníku
void topStackLabel(const struct tStackLabel *s, char **c, int *i, int *end, int *count);

// aktualizace proměné end
void actualizeStackLabel(const struct tStackLabel *s, int end, int count);

// odstraní vrchol zásobníku
void popStackLabel(struct tStackLabel *s);

// Vloží nový prvek na vrchol zásobníku
void pushStackLabel(struct tStackLabel *s, char *c, int i);

// Práce se seznamem *************************************************************************************
// inicializace seznamu
void initList(struct tList *l);

// vložení prvku do seznamu na konec
void insertLast(struct tList *l, const struct tListItem item);

// nastavení aktualního prvku na první
void first(struct tList *l);

// nastavení aktualního prvku na poslední
void last(struct tList *l);

// posunutí aktualního prvku na další
void succ(struct tList *l);

// posunutí aktualního prvku na předchozí
void pred(struct tList *l);

// vrací 0 když není aktivní a 1 když je aktivní
int active (tList *L);

// kopírování aktualního prvku
void copy(struct tList *l, struct tListItem *item);

// uvolnění seznamu
void disposeList(struct tList *l);

// Práce s tabulkou *******************************************************************************************************
// inicializace tabulky na 10 prvků
void initTab(struct tTabLabel *t);

void insertTab(struct tTabLabel *t, char *label, tInstr ins, struct sVariable_Data *op1, struct sVariable_Data *op2, struct sVariable_Data *result, char *jump);

// ziksání nasledujicí instrukce z daného labelu
// vrací 1 když je to poslední instrukce jinak 0 a vrací -1 když nenalezen label
int getItemTab(struct tTabLabel *t, char *label, struct tListItem *item);

// vrátí na daném labelu seznam instrukcí o jedno
void backInListTab(struct tTabLabel *t, char *label);

// zpět na první isntrukci v seznamu instrukcí
void firstInListTab(struct tTabLabel *t, char *label);

// uvolnění tabulky
void disposeTab(struct tTabLabel *t);

#endif
