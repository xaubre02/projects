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
 * 	Práce se zásobníkem a frontou
 *
 * Autor: 
 * 	David Bednařík (xbedna62)
 *  Vít Ambrož (xambro15) - sémantické zásobníky
 *
 *****/

#ifndef _STACK_H_
#define _STACK_H_

#include "scanner.h"

//********STACK pro syntaktickou analýza shora dolů*******************************
typedef enum{
    // neterminály (pravidla)
    S_TRIDA,
    S_TELO_TRIDY,
    S_TELO_TRIDY2,
    S_TELO_TRIDY3,
    S_PARAM,
    S_PARAM2,
    S_SEKVENCE_PRIKAZU,
    S_SLOZENY_PRIKAZ,
    S_PODMINKA,
    S_PODMINKA_ELSE,
    S_CYKLUS_WHILE,
    S_DEKLARACE_PROMENNE,
    S_INICIALIZACE,
    S_PRIRAZENI_VOLANI_FUNKCE,
    S_PRIRAZENI_VOLANI_FUNKCE2,
    S_ARG,
    S_ARG2,
    S_NAVRAT,
    S_NAVRAT2,
    S_VYRAZ,
    S_VYRAZ2,
    S_VYRAZ3,
    S_OPERATION,
    S_TYP,
    S_HODNOTA,
    S_HODNOTA2,
//*********************************************
    // klíčová slova
    S_CLASS,
    S_IF,
    S_ELSE,
    S_WHILE,
    S_RETURN,
    S_INT,
    S_DOUBLE,
    S_STRING,
    S_STATIC,
    S_VOID,
    S_TERM,
    S_ID_FULL_ID,
    S_ID,
    S_COMMA,
    S_SEMICOLON,
    S_BRACKET_LEFT,
    S_BRACKET_RIGHT,
    S_CURLY_LEFT,
    S_CURLY_RIGHT,
    S_MUL,
    S_DIV,
    S_PLUS,
    S_MINUS,
    S_LESS,
    S_GREATER,
    S_EQUAL,
    S_LEQUAL,
    S_GEQUAL,
    S_SAME,
    S_NOT_EQUAL,
} syntaxRule;

struct tItem;

typedef struct{
  struct tItem *top;                             
	int count;                                
} tStack;

typedef struct tItem{
  syntaxRule item;
  struct tItem *lptr;
} tItem;
                                 
void stackError ();
void stackInit ( tStack* s );
int stackEmpty ( const tStack* s );
void stackTop ( const tStack* s, syntaxRule* c );
void stackPop ( tStack* s );
void stackPush ( tStack* s, const syntaxRule c );
void stackCopy(tStack *s, tStack *d);

//********Que pomocná při ukládání posloupnosti tokenů pro předání syntaktické analýzy zdola nahoru (pro vyrazy)*******************************
struct tTokenItem;

typedef struct{
  struct tTokenItem *zac, *kon;              
} tTokenQue;

typedef struct tTokenItem{
  Token *item;
  struct tTokenItem *next;
} tTokenItem;
                                 
void queError ();
void tokenQueInit ( tTokenQue* s );
int tokenQueEmpty ( const tTokenQue* s );
void tokenQueFront ( const tTokenQue* s, Token* c );
void tokenQueRemove ( tTokenQue* s );
void tokenQueUp ( tTokenQue* s, const Token c );

//*******STACK pro syntaktickou analýzu zdola na horů pro výrazy**********************************************
typedef enum
{
  V,  // větší > redukuj
  M,  // menší < shiftuj
  R,  // rovno =
  X   // nic - chyba syntaktická
} precOp; // precedenční operace

typedef enum
{
  SV_MULTIPLY, // *
  SV_DIVIDE,   // /
  SV_PLUS,     // +
  SV_MINUS,    // -
  SV_LESS,   // <
  SV_GREATER,  // >
  SV_LEQUAL,  // <=
  SV_GEQUAL, // >=
  SV_SAME,    // ==
  SV_NEQUAL,   // !=
  SV_LBRACKET, // (
  SV_RBRACKET, // )
  SV_TERM,     // id, konstanta
  SV_DOLLAR,   // $ konec vyrazu
  SV_NETERM,   // neterminál
  SV_SHIFTUJ,   // pomocný item abych věděl po kdy mám redukovat při opreaci redukce
  SV_NOTHING
} precItem;
//*****************************************************************************************************************
struct tPrecItem;

typedef struct{
  int count;
  struct tPrecItem *top;
} tPrecStack;

typedef struct tPrecItem{
  precItem item;
  struct tPrecItem *lptr;
} tPrecItem;

void precStackInit ( tPrecStack* s );
int precStackEmpty ( const tPrecStack* s );
void precStackTop ( const tPrecStack* s, precItem* c );
void precStackPop ( tPrecStack* s );
void precStackPush ( tPrecStack* s, const precItem c );

//semantický zásobník při práci s výrazy**************************************************************************
typedef struct tSemItem{
  Token item;
  struct tSemItem *lptr;
} tSemItem;

typedef struct{
  int count;
  struct tSemItem *top;
} tSemStack;

void SemStackInit ( tSemStack* s );
int SemStackEmpty ( const tSemStack* s );
void SemStackTop ( const tSemStack* s, Token* c );
void SemStackPop ( tSemStack* s );
void SemStackPush ( tSemStack* s, Token* c );


//semantický zásobník při práci s vnořenými podmínkami*************************************************************
typedef struct tJumpItem{
  int item;
  struct tJumpItem *lptr;
} tJumpItem;

typedef struct{
  int count;
  struct tJumpItem *top;
} tJumpStack;

void JumpStackInit ( tJumpStack* s );
int JumpStackEmpty ( const tJumpStack* s );
void JumpStackTop ( const tJumpStack* s, int *i );
void JumpStackPop ( tJumpStack* s );
void JumpStackPush ( tJumpStack* s, int i );

#endif
