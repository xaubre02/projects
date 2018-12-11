	
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Bohuslav Křena, říjen 2016
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu, 
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek, 
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku, 
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu, 
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem, 
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu, 
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
	return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
	L->First = NULL;
	L->Act = NULL;
	L->Last = NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
	tDLElemPtr tmpPtr; // pomocný ukazatel
	while(L->First != NULL)
	{
		tmpPtr = L->First->rptr; // uložení ukazatele na následující prvek
		free(L->First); // uvolnění prvního prvku
		L->First = tmpPtr; // nastavení ukazatele seznamu na následující prvek
	}
	L->Last = NULL; // zrušení aktivní položky, protože je seznam prázdný
	L->Act = NULL; // zrušení aktivní položky, protože je seznam prázdný
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	tDLElemPtr tmpFirst = (tDLElemPtr) malloc(sizeof(struct tDLElem)); // alokace potřebné paměti
	
	if(tmpFirst == NULL) // pokud alokace selhala, volámé funkci na zpracování chyby a končíme
	{	    
		DLError();
		return;
	}

	tmpFirst->data = val; // uložení hodnoty val do datové oblasti prvku

	if(L->First == NULL) // pokud byl seznam prázdný, ukazatele First a Last ukazují na tentýž prvek
	{
		L->First = tmpFirst;
		L->Last = tmpFirst;
	}
	else // seznam již obsahoval alespoň jednu položku
	{
		tmpFirst->rptr = L->First; // nastavení pravého ukazetele na první prvek seznamu -> bude z něj druhý
		L->First->lptr = tmpFirst; // nastavení levého ukazatele prvního prvku na nový prvek 
		L->First = tmpFirst; // nastavení nového prvku prvním
	}
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	
	tDLElemPtr tmpLast = (tDLElemPtr) malloc(sizeof(struct tDLElem)); // alokace potřebné paměti
	
	if(tmpLast == NULL) // pokud alokace selhala, volámé funkci na zpracování chyby a končíme
	{	    
		DLError();
		return;
	}

	tmpLast->data = val; // uložení hodnoty val do datové oblasti prvku

	if(L->First == NULL) // pokud byl seznam prázdný, ukazatele First a Last ukazují na tentýž prvek
	{
		L->First = tmpLast;
		L->Last = tmpLast;
	}
	else // seznam již obsahoval alespoň jednu položku
	{
		tmpLast->lptr = L->Last; // nastavení levého ukazetele na poslední prvek seznamu -> bude z něj předposlední
		L->Last->rptr = tmpLast; // nastavení pravého ukazatele posledního prvku na nový prvek 
		L->Last = tmpLast; // nastavení nového prvku posledním
	}
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	L->Act = L->First;
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	L->Act = L->Last;
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	if(L->First == NULL) // kontrola, zdali seznam není prázdný
	{
		DLError();
		return;
	}
	else
	{
		*val = L->First->data; // uložení hodnoty prvního prvku
	}
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	if(L->First == NULL) // kontrola, zdali seznam není prázdný
	{
		DLError();
		return;
	}
	else
	{
		*val = L->Last->data; // uložení hodnoty posledního prvku
	}
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
	if(L->First == NULL) // pokud je seznam prázdný, funkce dále nepokračuje
	{
		return;
	}
	else
	{
		if(L->Act == L->First)	// pokud byl první prvek aktivní
		{
			L->Act = NULL; // ztráta aktivity
		}

		tDLElemPtr tmpPtr = L->First; // pomocný ukazatel ukazující na první prvek 
		L->First = L->First->rptr; // nastavení prvního prvku seznamu na následující
		L->First->lptr = NULL; // nemá levého souseda, protože je první
		free(tmpPtr); //uvolnění prvního prvku seznamu
	}
}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/ 
	if(L->First == NULL) // pokud je seznam prázdný, funkce dále nepokračuje
	{
		return;
	}
	else
	{
		if(L->Act == L->Last)	// pokud byl první prvek aktivní
		{
			L->Act = NULL; // ztráta aktivity
		}

		tDLElemPtr tmpPtr = L->Last; // pomocný ukazatel ukazující na poslední prvek 
		L->Last = L->Last->lptr; // nastavení poslední prvku seznamu na předposlední
		L->Last->rptr = NULL; // nemá pravého souseda, protože je poslední
		free(tmpPtr); //uvolnění posledního prvku seznamu
	}
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
	if(L->Act == NULL || L->Act == L->Last) // Pokud není seznam L aktivní nebo pokud je aktivní poslední prvek seznamu L
	{
		return;
	}
	else
	{
		tDLElemPtr tmpPtr = L->Act->rptr; // pomocný ukazatel ukazující na prvek za aktivním prvkem 
		
		if(tmpPtr->rptr == NULL) // aktivní prvek bude zároveň prvkem posledním, protože uvolňovaný prvek je poslední
		{
			L->Act->rptr = NULL; // nemá pravého souseda
			L->Last = L->Act;
		}
		else
		{
			L->Act->rptr = tmpPtr->rptr; // nastavení ukazatele uvolňovaného prvku na prvek následující
			tmpPtr->rptr->lptr = L->Act; // nastavení levého ukazatele prvku následujícího za uvolňovaným prvkem na aktivní prvek
		}
		free(tmpPtr); // uvolnění potřebného prvku
	}
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
	if(L->Act == NULL || L->Act == L->First) // Pokud není seznam L aktivní nebo pokud je aktivní poslední prvek seznamu L
	{
		return;
	}
	else
	{
		tDLElemPtr tmpPtr = L->Act->lptr; // pomocný ukazatel ukazující na prvek před aktivním prvkem 
		
		if(tmpPtr->lptr == NULL) // aktivní prvek bude zároveň prvkem prvním, protože uvolňovaný prvek je první
		{
			L->Act->lptr = NULL; // nemá levého souseda
			L->First = L->Act;
		}
		else
		{
			L->Act->lptr = tmpPtr->lptr; // nastavení ukazatele uvolňovaného prvku na prvek předchozí
			tmpPtr->lptr->rptr = L->Act; // nastavení levého ukazatele prvku následujícího za uvolňovaným prvkem na aktivní prvek
		}
		free(tmpPtr); // uvolnění potřebného prvku
	}
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	if(L->Act == NULL)
	{
		return;
	}
	else
	{
		tDLElemPtr new = (tDLElemPtr) malloc(sizeof(struct tDLElem)); // alokace potřebné paměti
			
		if(new == NULL) // pokud alokace selhala, volámé funkci na zpracování chyby a končíme
		{	    
			DLError();
			return;
		}
		if(L->Act->rptr == NULL) // vložení na konec seznamu, kde bude daný prvek posldení
		{
			L->Last = new;
			new->rptr = NULL; // nemá následující prvek
		}
		else
		{
			L->Act->rptr->lptr = new; // následující prvek ukazuje na nový prvek z levé strany
			new->rptr = L->Act->rptr; // nový prvek ukazuje na následující prvek z pravé strany
		}
		new->data = val; // uložení hodnoty
		new->lptr = L->Act; // nový prvek ukazuje na aktivní prvek z levé strany
		L->Act->rptr = new; // aktivní prvek z pravé strany ukazuje na nový prvek
	}
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	if(L->Act == NULL)
	{
		return;
	}
	else
	{
		tDLElemPtr new = (tDLElemPtr) malloc(sizeof(struct tDLElem)); // alokace potřebné paměti
			
		if(new == NULL) // pokud alokace selhala, volámé funkci na zpracování chyby a končíme
		{	    
			DLError();
			return;
		}
		if(L->Act->lptr == NULL) // vložení na začátek seznamu, kde bude daný prvek prvním
		{
			L->First = new;
			new->lptr = NULL; // nemá předchozí prvek
		}
		else
		{
			L->Act->lptr->rptr = new; // předchozí prvek ukazuje na nový prvek z pravé strany
			new->lptr = L->Act->lptr; // nový prvek ukazuje na předchozí prvek z levé strany
		}
		new->data = val; // uložení hodnoty
		new->rptr = L->Act; // nový prvek ukazuje na aktivní prvek z pravé strany
		L->Act->lptr = new; // aktivní prvek z levé strany ukazuje na nový prvek
	}
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
	if(L->Act == NULL) // Pokud seznam L není aktivní, volá funkci DLError ().
	{	    
		DLError();
		return;
	}
	else
	{
		*val = L->Act->data;
	}
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
	if(L->Act == NULL) // Pokud seznam L není aktivní, nedělá nic.
	{	    
		return;
	}
	else
	{
		L->Act->data = val;
	}
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
	if(L->Act == NULL) // Pokud seznam L není aktivní, nedělá nic.
	{	    
		return;
	}
	else
	{
		L->Act = L->Act->rptr;
	}
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
	if(L->Act == NULL) // Pokud seznam L není aktivní, nedělá nic.
	{	    
		return;
	}
	else
	{
		L->Act = L->Act->lptr;
	}
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
	return L->Act == NULL ? 0 : 1;
}

/* Konec c206.c*/
