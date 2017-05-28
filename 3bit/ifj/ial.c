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
 *  ial.c (implementace algoritmů pro předmět IAL)
 *
 * Autor: 
 *  Vít Ambrož (xambro15) - Hashovací tabulka
 *  David Bednařík (xbedna62) - Knuth-Morris-Prattův algoritmus
 *  Tomáš Aubrecht (xaubre02) - Shell-Sort
 *
 *****/
 

#include "ial.h"
#include "error.h"
#include "tsymbols.h"

int HTSIZE = MAX_HTSIZE;

/*
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíč a přidělit
** mu index v rozmezí 0..HTSize-1.  V ideálním případě by mělo dojít
** k rovnoměrnému rozptýlení těchto klíčů po celé tabulce.
** Naše hash funkce převede jednotlivé znaky stringu na integer, spočítá 
** a následně pomocí modulo získá hodnotu v rozsahu 0..HT_Size-1
*/

unsigned int hashCode ( char *key )
{
    int hash_key = 0; //proměnná pro výsledek hash funkce
    int tmp = 0;      //pomocná proměnná na převod z char na int
    unsigned int i;
    
    for(i = 0; i < strlen(key); i++) //průchod všemi znaky řetězce
    {
        tmp = key[i];
        hash_key = hash_key + tmp;
    }
    
    return (hash_key % HTSIZE);
}


//inicializace hashovací tabulky
void htInit ( tHTable* ptrht ) {
    
    int i;
    if( (*ptrht) != NULL)
    { 
        for(i = 0; i < HTSIZE; i++)
        {
            (*ptrht)[i]= NULL;   //kazda polozka je inicializovana na NULL
        } 
    }
}

/*
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není, 
** vrací se hodnota NULL.
*/
tHTItem* htSearch ( tHTable* ptrht, char* key ) {

    int i;
    tHTItem *pItem = NULL;             //hledany prvek
    
    i= hashCode(key);    //mapovani
    
    if( (*ptrht) != NULL)
    {
        pItem= (*ptrht)[i];
        
        while(pItem != NULL)
        {
            if(strcmp(pItem->key, key) == 0)  //nalezen prvek se stejnym key v TRP
            {
                return pItem;  //vraceni ukazatele na hledany prvek s danym key
            }
            else
                pItem = pItem->ptrnext;  //nasleduje dalsi prvek
        }
    }
    
    return NULL;   //prvek s danym key nebyl nalezen
}

/* 
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vkládá do tabulky ptrht položku s klíčem key a s daty
** data.  Protože jde o vyhledávací tabulku, nemůže být prvek se stejným
** klíčem uložen v tabulce více než jedenkrát.  Pokud se vkládá prvek,
** jehož klíč se již v tabulce nachází, aktualizuje jeho datovou část.
**/
void htInsert ( tHTable* ptrht, char* key, Class* data ) {

    int i;
    tHTItem *pItem = NULL;  //pomocny prvek pro zjisteni, zda se jiz v seznamu nenachazi prvek se stejnym key
    tHTItem *new;           //nove vkladany prvek
    
    if( (*ptrht) != NULL )
    {
        pItem = htSearch(ptrht, key);  //hleda se prvek, ktery ma stejny key
        
        if( pItem == NULL)             //prvek s danym key se v TRP nenachazi
        {
            if((new = (tHTItem*) malloc(sizeof (tHTItem))) == NULL)
            {
                Err = ERR_OTHER;
                return;   //nepovedena alokace pameti pro novy prvek
            }
            
            new->data = data;  //vlozeni dat a key
            
            if((new->key = (char*) malloc( (sizeof(char)) * strlen(key) + 1) ) == NULL)
            {
                Err = ERR_OTHER;
                return;   //nepovedena alokace pameti pro novy prvek
            }
            strcpy(new->key, key);
            
            i = hashCode(key);   //mapovani
            new->ptrnext = (*ptrht)[i];  //propojeni s puvodnim seznamem
            (*ptrht)[i] = new;
        }
        else        //prvek s danym key se v TRP jiz nachazi
        {
            pItem->data = data;   //aktualizace stavajicich dat
        }
    }
}

/*
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
*/
Class* htRead ( tHTable* ptrht, char* key ) {

    tHTItem *pItem = NULL;  //pomocny prvek pro zjisteni, zda se jiz v TRP nachazi prvek se stejnym key
    
    if( (*ptrht) != NULL )
    {
        pItem = htSearch(ptrht, key);  //hleda se prvek, ktery ma stejny key
        if( pItem != NULL)             //prvek s danym key se v TRP nenachazi
        {
            return pItem->data;     //ukazatel na data nalezeneho prvku
        }
    }
    return NULL;        //nebyl nalezen prvek se shodnym key
}


//Tato procedura vyjme položku s klíčem key z tabulky
void htDelete ( tHTable* ptrht, char* key ) {
    
    int i;
    tHTItem *pItem = NULL;     //pomocny prvek
    tHTItem *del_Item = NULL;  //ruseny prvek
    
    if( (*ptrht) != NULL )
    {
        i = hashCode(key);   //mapovani
        del_Item = (*ptrht)[i];
        
        while( del_Item != NULL )      //postupne se prochazi cela tabulka
        {
            if( strcmp(del_Item->key, key) == 0)  //nalezen prvek se stejnym key v TRP
            {
                if( pItem == NULL )
                {
                    (*ptrht)[i] = del_Item->ptrnext;   //predani ukazetele na nasledujici prvek po rusenem prvku
                }
                else
                {
                    pItem->ptrnext = del_Item->ptrnext;
                }
                free(del_Item->key);
                free_Data(del_Item->data);  ////využití funkce pro korektní uvolnění dat z tsymbols.c
                free(del_Item);     //uvolneni pameti daneho prvku
                del_Item = NULL;
            }
            else      //aktualni prvek nema stejny key
            {
                pItem = del_Item;
                del_Item = del_Item->ptrnext;   //pokracovani dalsim prvkem
            }
        }
    }
}

/* 
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/
void htClearAll ( tHTable* ptrht ) {

    int i;
    tHTItem *pItem = NULL;  //pomocny prvek
    tHTItem *del_Item;      //aktualne ruseny prvek

    if( (*ptrht) != NULL )
    {
        for(i=0; i<HTSIZE; i++) 
        {
            pItem= (*ptrht)[i];     //postupne se projdou vsechny indexy i
            while( pItem != NULL )
            {
                del_Item = pItem;
                pItem = pItem->ptrnext;  //predani ukazatele na dalsi prvek
                
                
                free_Data(del_Item->data);  //využití funkce pro korektní uvolnění dat z tsymbols.c
                if( del_Item->key != NULL )
                    free(del_Item->key);
                free(del_Item);          //uvolneni pameti
                del_Item= NULL;          //po kazdem zruseni pro jistotu vynulujeme 
            }
            (*ptrht)[i] = NULL;     //nyni jsou vsechny polozky na v seznamu na indexu i uvolneny
        }
    }
}

// Knuth-Morris-Prattův algoritmus *********************************************************

//Pomocná funkce pro Knuth-Morris-Prattův algoritmus
int *KMPGraf(char *p, int pl)
{
  int k, r;
  int *fail = malloc(sizeof(int) * pl);
  if (fail == NULL)
    return NULL;

  fail[0] = -1;
  for (k = 1; k < pl; k++) 
  {
    r = fail[k-1];
    while (r > -1 && p[r] != p[k-1])
      r = fail[r];
    fail[k] = r+1;
  }
  return fail;
}

//Samotná funkce pro vyhledavaní podřetězců v řetězci
int KMPMatch(char *t, int tl, char *p, int pl)
{
  int TInd = 0;
  int PInd = 0;
  int *fail = KMPGraf(p, pl);
  if(fail == NULL) // chyba v alokaci vracím -2
    return -2;

  while(TInd < tl && PInd < pl)
  {
    if(PInd == -1 || t[TInd] == p[PInd])
    {
      TInd++;
      PInd++;
    }
    else
    {
      PInd = fail[PInd];
    }
  }
  if(PInd == pl) // našel
  {
    free(fail);
    return TInd - pl;
  }
  else // nenašel vrací se hodnota -1
  {
    free(fail);
    return -1;
  }
}

// Shell-Sort řazení ***********************************************************************

const char* ShellSort(char *string)
{
    int j; // pomocná proměnná
    int l = strlen(string); // delka retezce
    int step = l / 2; // krok
    char tmp;

    while(step > 0) // cyklení dokud je nenulový krok
    {
        for(int i = step; i < l; i++)
        {
            j = i - step;
            
            while((j >= 0) && (string[j] > string[j+step]))
            {
                // prohození dvou symbolů -> seřazení
                tmp = string[j];
                string[j] = string[j+step];
                string[j+step] = tmp;
                j -= step;
            }
        }

        step /= 2; // zmenšení kroku
    }

    return string;
}
