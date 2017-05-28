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


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "error.h"
#include "tsymbols.h"
#include "kgenerator.h"
#include "adt.h"

// pomocná proměná pro vyčištění List itemu
void clearListItem(struct tListItem *item)
{
  item->ins = I_NOP;
  item->op1 = NULL;
  item->op2 = NULL;
  item->result = NULL;
  if(item->label != NULL)
  {
    free(item->label);
  }
  item->label = NULL;
  item->jump = 0;
  item->rptr = NULL;
  item->lptr = NULL;
}

// inicializace List itemu
void initListItem(struct tListItem *item)
{
  item->ins = I_NOP;
  item->op1 = NULL;
  item->op2 = NULL;
  item->result = NULL;
  item->label = NULL;
  item->jump = 0;
  item->rptr = NULL;
  item->lptr = NULL;
}
// Práce se zásobníkem ***********************************************************************************

// Inicializuje zasobník
void initStackLabel(struct tStackLabel *s) 
{
  if(s != NULL)
  {
    s->count = 0;
    s->top = NULL;
  }
  else
    stackError();
}

// Vrácí 1 pokud je zásobník prázdný jínak vrací 0
int emptyStackLabel(const struct tStackLabel *s) 
{
  return s->count == 0 ? 1 : 0;
}

// Do proměnné c vloží data z vrcholu zásobníku
void topStackLabel(const struct tStackLabel *s, char **c, int *i, int *end, int *count) 
{
  *c = s->top->label;
  *i = s->top->typeFun;
  if(end != NULL)
  {
    *end = s->top->end;
  }
  if(count != NULL)
  {
    *count = s->top->count;
  }   
}

// aktualizace proměné end
void actualizeStackLabel(const struct tStackLabel *s, int end, int count)
{
  s->top->end = end;
  s->top->count = count;
}

// odstraní vrchol zásobníku
void popStackLabel(struct tStackLabel *s) 
{
  if(!emptyStackLabel(s))
  {
    struct tItemL *tmp = s->top;
    s->top = s->top->lptr;
    s->count--;
    free(tmp->label);
    free(tmp);
  }
}

// Vloží nový prvek na vrchol zásobníku
void pushStackLabel(struct tStackLabel *s, char *c, int i) 
{
  struct tItemL *new;
  if((new = malloc(sizeof(struct tItemL))) != NULL)
  {
    new->label = malloc(sizeof(char)*strlen(c)+1);
    if(new->label == NULL)
    {
      stackError();
      return;
    }
    strcpy(new->label, c);
    new->typeFun = i;
    new->end = 0;
    new->count = 0;
    new->lptr = s->top;
    s->top = new;
    s->count++;
  }
  else 
    stackError();
}  

// Práce se seznamem *************************************************************************************
// inicializace seznamu
void initList(struct tList *l)
{
  l->Act = NULL;
  l->First = NULL;
  l->Last = NULL;
}

// vložení prvku do seznamu na konec
void insertLast(struct tList *l, const struct tListItem item)
{
  struct tListItem *new = malloc(sizeof(struct tListItem));
  initListItem(new);
  if(new == NULL)
  {
    Err = ERR_OTHER;
    return;
  }
  new->ins = item.ins;
  new->op1 = item.op1;
  new->op2 = item.op2;
  new->result = item.result;
  new->jump = item.jump;
  if(item.label != NULL) // obsahuje řetezec
  {
    new->label = malloc(sizeof(char)*strlen(item.label)+1);
    if(new->label == NULL)
    {
      Err = ERR_OTHER;
      free(new);
      return;
    }
    strcpy(new->label, item.label);
  }
  else
  {
    new->label = NULL;
  }
  new->rptr = NULL;

  if(l->First == NULL) // vkládám první
  {
    l->Act = new;
    l->First = new;
    l->Last = new;
    new->lptr = NULL;
  }
  else
  {
    l->Last->rptr = new;
    new->lptr = l->Last;
    l->Last = new;
  }
}

// nastavení aktualního prvku na první
void first(struct tList *l)
{
  l->Act = l->First;
}

// nastavení aktualního prvku na poslední
void last(struct tList *l)
{
  l->Act = l->Last;
}

// posunutí aktualního prvku na další
void succ(struct tList *l)
{
  if(l->Act != NULL)
    l->Act = l->Act->rptr;
}

// posunutí aktualního prvku na předchozí
void pred(struct tList *l)
{
  if(l->Act != NULL)
    l->Act = l->Act->lptr;
}

// vrací 0 když není aktivní a 1 když je aktivní
int active (tList *L) {		
  return L->Act == NULL ? 0 : 1;
}

// kopírování aktualního prvku
void copy(struct tList *l, struct tListItem *item)
{
  if(l->Act != NULL)
  {
    item->ins = l->Act->ins;
    item->op1 = l->Act->op1;
    item->op2 = l->Act->op2;
    item->result = l->Act->result;
    item->jump = l->Act->jump;
    if(l->Act->label != NULL) // obsahuje řetezec
    {
      item->label = malloc(sizeof(char)*strlen(l->Act->label)+1);
      if(item->label == NULL)
      {
        Err = ERR_OTHER;
        return;
      }
      strcpy(item->label, l->Act->label);
    }
    else
    {
      item->label = NULL;
    }
    item->rptr = NULL;
    item->lptr = NULL;
  }
}

// uvolnění seznamu
void disposeList(struct tList *l)
{
  struct tListItem *tmp = NULL;
  while(l->First != NULL)
  {
    tmp = l->First;
    l->First = l->First->rptr;
    clearListItem(tmp);
    free(tmp);
  }
  l->Act = NULL;
  l->Last = NULL;
}

// Práce s tabulkou *******************************************************************************************************
// inicializace tabulky na 10 prvků
void initTab(struct tTabLabel *t)
{
  struct tLabel *label = malloc(sizeof(struct tLabel)*10);
  if(label == NULL)
  {
    Err = ERR_OTHER;
    return;  
  }
  t->tl = label;
  t->cap = 10;
  t->used = 0;
}

void insertTab(struct tTabLabel *t, char *label, tInstr ins, struct sVariable_Data *op1, struct sVariable_Data *op2, struct sVariable_Data *result, char *jump)
{
  if(t != NULL)
  {
    if(label != NULL)
    {
      // naplnění itemu vstupními informacemi
      struct tListItem item;
      initListItem(&item);
      item.ins = ins;
      item.op1 = op1;
      item.op2 = op2;
      item.result = result;
      if(jump != NULL)
      {
        item.jump = 1;
        item.label = malloc(sizeof(char)*strlen(jump)+1);
        if(item.label == NULL)
        {
          Err = ERR_OTHER;
          return;
        }
        strcpy(item.label, jump);
      }
      else
      {
        item.jump = 0;
        item.label = NULL;
      }
      item.rptr = NULL;
      item.lptr = NULL;
      // ------------------------------------------------------------------
      for(int i = 0; i < t->used; i++) // hledání již existujícího labelu
      {
        if(strcmp(label, t->tl[i].label) == 0) // našel jsem již existující label
        {
          insertLast(&(t->tl[i].l), item);
          clearListItem(&item); // vyčištění pomocného itemu
          return;
        }
      }  
      // nenašel se label tudiž se vloží nový -----------------------------
      if(t->cap == t->used) // pokud je málo místa vytvoří se další místo
      {
        t->tl = realloc(t->tl, sizeof(struct tLabel)*(t->cap + 10));
        if(t->tl == NULL)
        {
          Err = ERR_OTHER;
          return;
        }
        t->cap += 10;
      }
      // samotné vložení labelu s instrukcí
      t->tl[t->used].label = malloc(sizeof(char)*strlen(label)+1);
      if(t->tl[t->used].label == NULL)
      {
        Err = ERR_OTHER;
        return;
      }
      strcpy(t->tl[t->used].label, label);
      initList(&(t->tl[t->used].l));
      insertLast(&(t->tl[t->used].l), item);
      t->used++;
      clearListItem(&item); // vyčištění pomocného itemu
    }
  }
}

// ziksání nasledujicí instrukce z daného labelu
// vrací 1 když je to poslední instrukce jinak 0 a vrací -1 když nenalezen label
int getItemTab(struct tTabLabel *t, char *label, struct tListItem *item)
{
  int i;
  for(i = 0; i < t->used; i++) // hledání labelu
  {
    if(strcmp(label, t->tl[i].label) == 0) // našel jsem již existující label
    {
      break;
    }
  }  
  if(i == t->used) // nenašel
  {
    return -1;
  }
  copy(&(t->tl[i].l), item);
  succ(&(t->tl[i].l));
  if(active(&(t->tl[i].l)) == 0) // neaktivní seznam
  {
    first(&(t->tl[i].l)); // vracení aktivity na začátek
    return 1; // byla načtena poslední instrukce
  }
  return 0; // seznam je stále aktivní následuje další instrukce
}

// vrátí na daném labelu seznam instrukcí o jedno
void backInListTab(struct tTabLabel *t, char *label)
{
  int i;
  for(i = 0; i < t->used; i++) // hledání labelu
  {
    if(strcmp(label, t->tl[i].label) == 0) // našel
    {
      if(active(&(t->tl[i].l)) == 1)
      {
        pred(&(t->tl[i].l));
        if(active(&(t->tl[i].l)) == 0)
        {
          last(&(t->tl[i].l));
        }
      }      
      return;
    }
  }  
}

// zpět na první isntrukci v seznamu instrukcí
void firstInListTab(struct tTabLabel *t, char *label)
{
  int i;
  for(i = 0; i < t->used; i++) // hledání labelu
  {
    if(strcmp(label, t->tl[i].label) == 0) // našel
    {
      first(&(t->tl[i].l));     
      return;
    }
  }  
}

// uvolnění tabulky
void disposeTab(struct tTabLabel *t)
{
  for(int i=0; i < t->used; i++) // uvolnění iteml prvků (char label, seznam l)
  {
    disposeList(&(t->tl[i].l));
    free(t->tl[i].label);
  }
  free(t->tl);
  t->cap = 0;
  t->used = 0;
}
