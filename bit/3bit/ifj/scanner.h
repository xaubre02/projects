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
 * 	Lexikalni analyzátor (Scanner)
 *
 * Autor: 
 * 	Tomáš Aubrecht (xaubre02)
 *
 *****/

#ifndef _SCANNER_H_
#define _SCANNER_H_
#include "error.h"

#include <stdio.h>
#include <stdlib.h>

/*  MUST DO !!!
 ****************
 *	- pořešit oddělování tokenů - mezery, operátory apod.
 *	- ignorace nul není, bude provedena u převodu ze stringu na číslo
 */
 
 extern FILE* source;
 extern int line;

// výčet všech možných stavů konečného automatu
typedef enum{
	ss_empty,
	ss_identifier,
	ss_full_identifier,
	ss_full_id_second,
	ss_number,
	ss_decimal,
	ss_decimal_part,
	ss_exponent,
	ss_exponent_sign,
	ss_exponent_part,
	ss_divide,
	ss_less,
	ss_greater,
	ss_equal,
	ss_not,
	ss_literal,
	ss_line_comment,
	ss_block_comment,
	ss_comment_end,
	ss_comment_error,
	ss_sequence,
	ss_octal,
    ss_octal_last
} ScannerState;

// výčet všech možných typů tokenu
typedef enum{
	tt_empty,			// výchozí stav
//	tt_dot,				// .
	tt_comma,			// ,
	tt_semicolon,		// ;
	tt_bracket_left,	// (
	tt_bracket_right,	// )
	tt_curly_left,		// {
	tt_curly_right,		// }
	tt_mul, 			// *
	tt_div,				// /
	tt_plus,			// +
	tt_minus,			// -
	tt_less,			// <
	tt_greater,			// >
	tt_equal,			// =
	tt_lequal,			// <=
	tt_gequal,			// >=
	tt_same,			// ==
	tt_not_equal,		// !=
	tt_EOF,				// Konec souboru
	tt_invalid,			// Neplatný token
	tt_identifier,		// Jednoduchý identifikátor
	tt_full_id,			// Plně kvalifikovaný identifikátor
	tt_literal,			// Řetězec
	tt_keyword,			// Klíčové slovo
	tt_data_type,		// Datový typ
	tt_int,				// Datový typ int
	tt_double			// Datový typ double
} TokenType;

// výčet všech klíčových slov a výchozí hodnoty
typedef enum{
	key_none,
	key_boolean,
	key_break,
	key_class,
	key_continue,
	key_do,
	key_double,
	key_else,
	key_false,
	key_for,
	key_if,
	key_int,
	key_return,
	key_String,
	key_static,
	key_true,
	key_void,
	key_while
} KeywordType;

// struktura Tokenu
typedef struct{
	TokenType type;
	KeywordType keyword;
	int line;	// pozice v souboru
	int lenght; // délka řetězce
	int cap;	// kapacita stringu
	char *string;
} Token;

// Deklarace funkcí
	// funkce na otevření souboru
	void openFile(const char *filename);
	// funkce na zavření souboru
	void closeFile(void);
	// funkce na převod čísla z oktalové do desítkové soustavy
	int convert(int octalNumber);
	// funkce na inicializaci tokenu
	void initToken(Token *token);
	// funkce na uvolnění tokeun
	void clearToken(Token *token);
	// pomocná funkce na zkopírování obsahu tokenu do druhého tokenu
	void copyToken(Token *dst, Token *src);
	// funkce na přidání znaku do řetězce tokenu
	void addToToken(Token *token, int c);
	// funkce na naplnění tokenu
	void processToken(Token *token);
	// funkce na kontrolu, zdali řetězec není klíčové slovo
	void checkString(Token *token, int p);

#endif
