
/* c016.c: **********************************************************}
{* Téma:  Tabulka s Rozptýlenými Položkami
**                      První implementace: Petr Přikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masařík, říjen 2014
**                              Radek Hranický, říjen 2014
**                              Radek Hranický, listopad 2015
**                              Radek Hranický, říjen 2016
**
** Vytvořete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Položkami = Hash table)
** s explicitně řetězenými synonymy. Tabulka je implementována polem
** lineárních seznamů synonym.
**
** Implementujte následující procedury a funkce.
**
**  HTInit ....... inicializuje tabulku před prvním použitím
**  HTInsert ..... vložení prvku
**  HTSearch ..... zjištění přítomnosti prvku v tabulce
**  HTDelete ..... zrušení prvku
**  HTRead ....... přečtení hodnoty prvku
**  HTClearAll ... zrušení obsahu celé tabulky (inicializace tabulky
**                 poté, co již byla použita)
**
** Definici typů naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelů na položky, jež obsahují složky
** klíče 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na další synonymum 'ptrnext'. Při implementaci funkcí
** uvažujte maximální rozměr pole HTSIZE.
**
** U všech procedur využívejte rozptylovou funkci hashCode.  Povšimněte si
** způsobu předávání parametrů a zamyslete se nad tím, zda je možné parametry
** předávat jiným způsobem (hodnotou/odkazem) a v případě, že jsou obě
** možnosti funkčně přípustné, jaké jsou výhody či nevýhody toho či onoho
** způsobu.
**
** V příkladech jsou použity položky, kde klíčem je řetězec, ke kterému
** je přidán obsah - reálné číslo.
*/

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*          -------
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíč a přidělit
** mu index v rozmezí 0..HTSize-1.  V ideálním případě by mělo dojít
** k rovnoměrnému rozptýlení těchto klíčů po celé tabulce.  V rámci
** pokusů se můžete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitně zřetězenými synonymy.  Tato procedura
** se volá pouze před prvním použitím tabulky.
*/

void htInit ( tHTable* ptrht ) {
	if(*ptrht != NULL) // pokud je platný ukazatel
	{
		for(int i = 0; i < HTSIZE; i++) // cyklus na inicializaci jednotlivých položek pole
		{
			(*ptrht)[i] = NULL;
		}
	}
}

/* TRP s explicitně zřetězenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není, 
** vrací se hodnota NULL.
**
*/

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {
	if(*ptrht != NULL) // pokud je to platný ukazatel na tabulku
	{
		tHTItem *tmp = (*ptrht)[hashCode(key)]; // pomocná proměnná představující položku v tabulce
		while(tmp != NULL) // cyklus do doby, dokud existuje položka
		{
			if(tmp->key == key) // hledaná položka byla nalezena
			{
				return tmp;
			}
			else
			{
				tmp = tmp->ptrnext; // přechod na další synonymum
			}
		}
	}
	// pokud je neplatný ukazatel na TRP nebo nebyl nalezen hledaný prvek
	return NULL;
}

/* 
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vkládá do tabulky ptrht položku s klíčem key a s daty
** data.  Protože jde o vyhledávací tabulku, nemůže být prvek se stejným
** klíčem uložen v tabulce více než jedenkrát.  Pokud se vkládá prvek,
** jehož klíč se již v tabulce nachází, aktualizujte jeho datovou část.
**
** Využijte dříve vytvořenou funkci htSearch.  Při vkládání nového
** prvku do seznamu synonym použijte co nejefektivnější způsob,
** tedy proveďte.vložení prvku na začátek seznamu.
**/

void htInsert ( tHTable* ptrht, tKey key, tData data ) {
	if(*ptrht != NULL) // pokud je to platný ukazatel na tabulku
	{
		tHTItem *tmp = htSearch(ptrht, key); // pomocná proměnná, která vyhledá daný klíč
		if(tmp == NULL) // položka s daným klíčem se v tabulce nenachází
		{
			int tmpCode = hashCode(key); // pomocný kód pro vytvoření indexu
			tmp = malloc(sizeof(struct tHTItem)); // vytvoření nové položky
			if(tmp == NULL) // kontrola, zdali se povedla alokace
			{
				//fprintf(stderr, "Allocation Error!\n");
			}
			else
			{
				tmp->key = key; // uložení klíče
				tmp->data = data; // uložení obsahu
				tmp->ptrnext = (*ptrht)[tmpCode]; // nastavení ukazatele na zbylé položky
				(*ptrht)[tmpCode] = tmp; // vložení nové položky na začátek seznamu
			}
		}
		else // položka s daným klíčem je v tabulce již obsažena
		{
			tmp->data = data; // aktualizace datové části
		}
	}
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
**
** Využijte dříve vytvořenou funkci HTSearch.
*/

tData* htRead ( tHTable* ptrht, tKey key ) {
	if(*ptrht != NULL) // pokud je to platný ukazatel na tabulku
	{
		tHTItem *tmp = htSearch(ptrht, key); // pomocná proměnná
		if(tmp != NULL)
		{
			return &tmp->data;
		}
	}
	// pokud je neplatný ukazatel na TRP nebo nebyla nalezena hledaná položka
	return NULL;
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vyjme položku s klíčem key z tabulky
** ptrht.  Uvolněnou položku korektně zrušte.  Pokud položka s uvedeným
** klíčem neexistuje, dělejte, jako kdyby se nic nestalo (tj. nedělejte
** nic).
**
** V tomto případě NEVYUŽÍVEJTE dříve vytvořenou funkci HTSearch.
*/

void htDelete ( tHTable* ptrht, tKey key ) {
	if(*ptrht != NULL) // pokud je to platný ukazatel na tabulku
	{
		int tmpCode = hashCode(key); // pomocný kód
		// dvě po sobě jdoucí pomocné položky
		tHTItem *act = (*ptrht)[tmpCode]; // aktuální
		tHTItem *prev = NULL; // předchozí
		while(act != NULL) // cyklus do doby, dokud existuje položka
		{
			if(act->key == key) // hledaná položka byla nalezena
			{
				if(prev == NULL) // není žádná předchozí položka
				{
					(*ptrht)[tmpCode] = act->ptrnext; // následující položka se stane první
				}
				else
				{
					prev->ptrnext = act->ptrnext; // předchozí položka bude ukazovat na položku za smaznou položkou
				}
				free(act); // uvolnění
				return; // návrat
			}
			else
			{
				// posun o jdnu položku
				prev = act;
				act = act->ptrnext;
			}
		}
	}
}

/* TRP s explicitně zřetězenými synonymy.
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/

void htClearAll ( tHTable* ptrht ) {
	if(*ptrht != NULL) // pokud je platný ukazatel
	{
		for(int i = 0; i < HTSIZE; i++) // cyklus na uvolnění všech položek pole
		{
			// dvě po sobě jdoucí pomocné položky
			tHTItem *act = (*ptrht)[i]; // aktuální
			tHTItem *prev = NULL; // předchozí položka
			while(act != NULL) //nalezení poslední položky
			{
				// posun o jdnu položku
				prev = act;
				act = act->ptrnext;
				free(prev); // uvolnění
			}
			(*ptrht)[i] = NULL; // inicializace
		}
	}
}
