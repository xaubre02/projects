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
 * 	Parser - synt.analyzátor + sem.analyzátor + generování isntrukcí
 *
 * Autor: 
 *  David Bednařík (xbedna62) - prediktivní synt. analýza + precedenční synt. analýza pro výrazy
 *  Vít Ambrož (xambro15) - sémantické akce, generování tříadresného kódu
 *
 *****/

#ifndef _PARSER_H_
#define _PARSER_H_
#include "adt.h"
#include "error.h"
#include "ial.h"
#include "kgenerator.h"
#include "interpret.h"

#define true 1
#define false 0

extern int ROUND;

ErrorType parser_twoRounds();
ErrorType parser();
precItem translate(Token token);
ErrorType parserExp(tTokenQue *q);

#endif
