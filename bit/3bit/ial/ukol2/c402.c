
/* c402.c: ********************************************************************}
{* Téma: Nerekurzivní implementace operací nad BVS 
**                                     Implementace: Petr Přikryl, prosinec 1994
**                                           Úpravy: Petr Přikryl, listopad 1997
**                                                     Petr Přikryl, květen 1998
**			  	                        Převod do jazyka C: Martin Tuček, srpen 2005
**                                         Úpravy: Bohuslav Křena, listopad 2009
**                                         Úpravy: Karel Masařík, říjen 2013
**                                         Úpravy: Radek Hranický, říjen 2014
**                                         Úpravy: Radek Hranický, listopad 2015
**                                         Úpravy: Radek Hranický, říjen 2016
**
** S využitím dynamického přidělování paměti, implementujte NEREKURZIVNĚ
** následující operace nad binárním vyhledávacím stromem (předpona BT znamená
** Binary Tree a je u identifikátorů uvedena kvůli možné kolizi s ostatními
** příklady):
**
**     BTInit .......... inicializace stromu
**     BTInsert ........ nerekurzivní vložení nového uzlu do stromu
**     BTPreorder ...... nerekurzivní průchod typu pre-order
**     BTInorder ....... nerekurzivní průchod typu in-order
**     BTPostorder ..... nerekurzivní průchod typu post-order
**     BTDisposeTree ... zruš všechny uzly stromu
**
** U všech funkcí, které využívají některý z průchodů stromem, implementujte
** pomocnou funkci pro nalezení nejlevějšího uzlu v podstromu.
**
** Přesné definice typů naleznete v souboru c402.h. Uzel stromu je typu tBTNode,
** ukazatel na něj je typu tBTNodePtr. Jeden uzel obsahuje položku int Cont,
** která současně slouží jako užitečný obsah i jako vyhledávací klíč 
** a ukazatele na levý a pravý podstrom (LPtr a RPtr).
**
** Příklad slouží zejména k procvičení nerekurzivních zápisů algoritmů
** nad stromy. Než začnete tento příklad řešit, prostudujte si důkladně
** principy převodu rekurzivních algoritmů na nerekurzivní. Programování
** je především inženýrská disciplína, kde opětné objevování Ameriky nemá
** místo. Pokud se Vám zdá, že by něco šlo zapsat optimálněji, promyslete
** si všechny detaily Vašeho řešení. Povšimněte si typického umístění akcí
** pro různé typy průchodů. Zamyslete se nad modifikací řešených algoritmů
** například pro výpočet počtu uzlů stromu, počtu listů stromu, výšky stromu
** nebo pro vytvoření zrcadlového obrazu stromu (pouze popřehazování ukazatelů
** bez vytváření nových uzlů a rušení starých).
**
** Při průchodech stromem použijte ke zpracování uzlu funkci BTWorkOut().
** Pro zjednodušení práce máte předem připraveny zásobníky pro hodnoty typu
** bool a tBTNodePtr. Pomocnou funkci BTWorkOut ani funkce pro práci
** s pomocnými zásobníky neupravujte 
** Pozor! Je třeba správně rozlišovat, kdy použít dereferenční operátor *
** (typicky při modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem 
** (např. při vyhledávání). V tomto příkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, použijeme dereferenci.
**/

#include "c402.h"
int solved;

void BTWorkOut (tBTNodePtr Ptr)		{
/*   ---------
** Pomocná funkce, kterou budete volat při průchodech stromem pro zpracování
** uzlu určeného ukazatelem Ptr. Tuto funkci neupravujte.
**/
			
	if (Ptr==NULL) 
    printf("Chyba: Funkce BTWorkOut byla volána s NULL argumentem!\n");
  else 
    printf("Výpis hodnoty daného uzlu> %d\n",Ptr->Cont);
}
	
/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu tBTNodePtr. Tyto funkce neupravujte.
**/

void SInitP (tStackP *S)  
/*   ------
** Inicializace zásobníku.
**/
{
	S->top = 0;  
}	

void SPushP (tStackP *S, tBTNodePtr ptr)
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
{ 
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
  if (S->top==MAXSTACK) 
    printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
  else {  
		S->top++;  
		S->a[S->top]=ptr;
	}
}	

tBTNodePtr STopPopP (tStackP *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
{
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
		return(NULL);	
	}	
	else {
		return (S->a[S->top--]);
	}	
}

bool SEmptyP (tStackP *S)
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
{
  return(S->top==0);
}	

/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu bool. Tyto funkce neupravujte.
*/

void SInitB (tStackB *S) {
/*   ------
** Inicializace zásobníku.
**/

	S->top = 0;  
}	

void SPushB (tStackB *S,bool val) {
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
	if (S->top==MAXSTACK) 
		printf("Chyba: Došlo k přetečení zásobníku pro boolean!\n");
	else {
		S->top++;  
		S->a[S->top]=val;
	}	
}

bool STopPopB (tStackB *S) {
/*   --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0) {
		printf("Chyba: Došlo k podtečení zásobníku pro boolean!\n");
		return(NULL);	
	}	
	else {  
		return(S->a[S->top--]); 
	}	
}

bool SEmptyB (tStackB *S) {
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
  return(S->top==0);
}

/* -------------------------------------------------------------------------- */
/*
** Následuje jádro domácí úlohy - funkce, které máte implementovat. 
*/

void BTInit (tBTNodePtr *RootPtr)	{
/*   ------
** Provede inicializaci binárního vyhledávacího stromu.
**
** Inicializaci smí programátor volat pouze před prvním použitím binárního
** stromu, protože neuvolňuje uzly neprázdného stromu (a ani to dělat nemůže,
** protože před inicializací jsou hodnoty nedefinované, tedy libovolné).
** Ke zrušení binárního stromu slouží procedura BTDisposeTree.
**	
** Všimněte si, že zde se poprvé v hlavičce objevuje typ ukazatel na ukazatel,	
** proto je třeba při práci s RootPtr použít dereferenční operátor *.
**/
	*RootPtr = NULL;
}

void BTInsert (tBTNodePtr *RootPtr, int Content) {
/*   --------
** Vloží do stromu nový uzel s hodnotou Content.
**
** Z pohledu vkládání chápejte vytvářený strom jako binární vyhledávací strom,
** kde uzly s hodnotou menší než má otec leží v levém podstromu a uzly větší
** leží vpravo. Pokud vkládaný uzel již existuje, neprovádí se nic (daná hodnota
** se ve stromu může vyskytnout nejvýše jednou). Pokud se vytváří nový uzel,
** vzniká vždy jako list stromu. Funkci implementujte nerekurzivně.
**/
	// pomocné proměnné pro lepší přehlednost
	tBTNodePtr tmp = *RootPtr;
	tBTNodePtr new;

	if(tmp == NULL) // pokud je strom prázdný
	{
		new = malloc(sizeof(struct tBTNode)); // alokování místa
		if(new == NULL) // kontrola úspěšnosti alokace
		{
			fprintf(stderr, "Allocation Error!\n");
			return;
		}
		// přiřazení hodnot
		new->Cont = Content;
		new->LPtr = NULL;
		new->RPtr = NULL;
		*RootPtr = new; // přiřazení ukazatele na nový uzel
	}
	else // pokud strom již obsahuje nějaký uzel
	{
		while(1)
		{
			if((tmp->Cont > Content) && (tmp->LPtr == NULL)) // pokud je uzel menší než otec a neexistuje žádný uzel vlevo
			{
				new = malloc(sizeof(struct tBTNode)); // alokování místa
				if(new == NULL) // kontrola úspěšnosti alokace
				{
					fprintf(stderr, "Allocation Error!\n");
					return;
				}
				// přiřazení hodnot
				new->Cont = Content;
				new->LPtr = NULL;
				new->RPtr = NULL;
				tmp->LPtr = new; // přiřazení ukazatele na nový uzel
				return; // ukončení cyklu
			}
			else if((tmp->Cont < Content) && (tmp->RPtr == NULL)) // pokud je uzel větší než otec a neexistuje žádný uzel vpravo
			{
				new = malloc(sizeof(struct tBTNode)); // alokování místa
				if(new == NULL) // kontrola úspěšnosti alokace
				{
					fprintf(stderr, "Allocation Error!\n");
					return;
				}
				// přiřazení hodnot
				new->Cont = Content;
				new->LPtr = NULL;
				new->RPtr = NULL;
				tmp->RPtr = new; // přiřazení ukazatele na nový uzel
				return; // ukončení cyklu
			}
			else if(tmp->Cont == Content) // pokud již uzel s touto hodnotou existuje, nic se neprovádí
			{
				return;
			}
			else // pokud již existuje uzel s hodnotou větší/menší než otec
			{
				if(tmp->Cont > Content)
				{
					tmp = tmp->LPtr; // přejdi vlevo
				}
				else
				{
					tmp = tmp->RPtr; // přejdi vpravo
				}
			}
		}
	}
}

/*                                  PREORDER                                  */

void Leftmost_Preorder (tBTNodePtr ptr, tStackP *Stack)	{
/*   -----------------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Preorder navštívené uzly zpracujeme voláním funkce BTWorkOut()
** a ukazatele na ně is uložíme do zásobníku.
**/
	while(ptr != NULL) // dokud existuje uzel
	{
		BTWorkOut(ptr); // zpracování ukazatele
		SPushP(Stack, ptr); // uložení ukazatele
		ptr = ptr->LPtr; // přechod na další uzel
	}
}

void BTPreorder (tBTNodePtr RootPtr)	{
/*   ----------
** Průchod stromem typu preorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Preorder a zásobníku ukazatelů. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut(). 
**/
	// vytvoření a inicializace zásobníku
	tStackP stack;
	SInitP(&stack);
	Leftmost_Preorder(RootPtr, &stack);

	while( !(SEmptyP(&stack)) ) // dokud v zásobníku něco je
	{
		Leftmost_Preorder(STopPopP(&stack)->RPtr, &stack); // průchody a načítání prvků
	}
}


/*                                  INORDER                                   */ 

void Leftmost_Inorder(tBTNodePtr ptr, tStackP *Stack)		{
/*   ----------------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Inorder ukládáme ukazatele na všechny navštívené uzly do
** zásobníku. 
**/
	while(ptr != NULL) // dokud existuje uzel
	{
		SPushP(Stack, ptr); // uložení ukazatele
		ptr = ptr->LPtr; // přechod na další uzel
	}
}

void BTInorder (tBTNodePtr RootPtr)	{
/*   ---------
** Průchod stromem typu inorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Inorder a zásobníku ukazatelů. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut(). 
**/	
	// vytvoření a inicializace zásobníku
	tStackP stack;
	SInitP(&stack);
	Leftmost_Inorder(RootPtr, &stack);

	while( !(SEmptyP(&stack)) ) // dokud v zásobníku něco je
	{
		RootPtr = STopPopP(&stack); // načtení prvku ze zásobníku
		BTWorkOut(RootPtr); // zpracování uzlu
		Leftmost_Inorder(RootPtr->RPtr, &stack); // průchody a uvolňování zásobníku
	}	
}

/*                                 POSTORDER                                  */ 

void Leftmost_Postorder (tBTNodePtr ptr, tStackP *StackP, tStackB *StackB) {
/*           --------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Postorder ukládáme ukazatele na navštívené uzly do zásobníku
** a současně do zásobníku bool hodnot ukládáme informaci, zda byl uzel
** navštíven poprvé a že se tedy ještě nemá zpracovávat. 
**/
	while(ptr != NULL) // dokud existuje uzel
	{
		SPushP(StackP, ptr); // uložení ukazatele
		SPushB(StackB, true); // uložení hodnoty průchodu
		ptr = ptr->LPtr; // přechod na další uzel
	}	
}

void BTPostorder (tBTNodePtr RootPtr)	{
/*           -----------
** Průchod stromem typu postorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Postorder, zásobníku ukazatelů a zásobníku hotdnot typu bool.
** Zpracování jednoho uzlu stromu realizujte jako volání funkce BTWorkOut(). 
**/
	// vytvoření a inicializace zásobníků
	tStackP stackP;
	tStackB stackB;
	SInitP(&stackP);	
	SInitB(&stackB);

	Leftmost_Postorder(RootPtr, &stackP, &stackB);

	while( !(SEmptyP(&stackP)) ) // dokud v zásobníku něco je
	{
		RootPtr = STopPopP(&stackP); // načtení uzlu
		SPushP(&stackP, RootPtr); // uložení ho zpátky na zásobník
		if(STopPopB(&stackB)) // ověření strany - levá/pravá
		{
			SPushB(&stackB, false); // uložení "zprava"
			Leftmost_Postorder(RootPtr->RPtr, &stackP, &stackB);
		}
		else // už jsem uzlem prošel zprava
		{
			STopPopP(&stackP); // posunutí zásobníku
			BTWorkOut(RootPtr); // zpracování uzlu
		}
	}
}


void BTDisposeTree (tBTNodePtr *RootPtr)	{
/*   -------------
** Zruší všechny uzly stromu a korektně uvolní jimi zabranou paměť.
**
** Funkci implementujte nerekurzivně s využitím zásobníku ukazatelů.
**/
	if(*RootPtr != NULL) // pokud není strom prázdný
	{
		tBTNodePtr tmp; // pomocná proměnná
		tStackP stack; // zásobník
		SInitP(&stack); // jeho inicializace
		while( (*RootPtr != NULL) || !(SEmptyP(&stack)) ) // dokud existuje ukazatel nebo není prázdný zásobník
		{
			if( (*RootPtr == NULL) && (!SEmptyP(&stack)) ) // pokud je uzel smazaný, ale zásobník není prázdný
			{
				*RootPtr = STopPopP(&stack); // přiřazení hodnoty z vrcholu zásobníku
			}
			else
			{
				if((*RootPtr)->RPtr != NULL) // pokud existuje pravý uzel - uložení na zásobník
				{
					SPushP(&stack, (*RootPtr)->RPtr);
				}
				tmp = *RootPtr;
				*RootPtr = (*RootPtr)->LPtr;
				free(tmp);
			}
		}
	}	
}

/* konec c402.c */