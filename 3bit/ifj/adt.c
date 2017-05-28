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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "adt.h"
#include "scanner.h"



void stackError (){
	fprintf(stderr, "Stack error.\n");
  Err = ERR_OTHER;
}

//********STACK pro syntaktickou analýza shora dolů*******************************
// Inicializuje zasobník
void stackInit ( tStack* s ) {
  if(s != NULL)
  {
    s->count = 0;
    s->top = NULL;
  }
  else
    stackError();
}

// Vrácí 1 pokud je zásobník prázdný jínak vrací 0
int stackEmpty ( const tStack* s ) {
  return s->count == 0 ? 1 : 0;
}

// Do proměnné c vloží data z vrcholu zásobníku
void stackTop ( const tStack* s, syntaxRule* c ) {
  *c = s->top->item;
}

// odstraní vrchol zásobníku
void stackPop ( tStack* s ) {
  if(!stackEmpty(s))
  {
    tItem *tmp = s->top;
    s->top = s->top->lptr;
    s->count--;
    free(tmp);
  }
}

// Vloží nový prvek na vrchol zásobníku
void stackPush ( tStack* s, const syntaxRule c ) {
  tItem *new;
  if((new = malloc(sizeof(tItem))) != NULL)
  {
    new->item = c;
    new->lptr = s->top;
    s->top = new;
    s->count++;
  }
  else 
    stackError();
}  

// Zkopíruje obsah zásobníku s do zásobníku d
void stackCopy(tStack *s, tStack *d) // zkopiruje obsah zasobniku s do zasobniku d
{
  tStack tmp;
  stackInit(&tmp);
  while(stackEmpty(s) != 1)
  {
    stackPush(&tmp, s->top->item);
    stackPop(s);
  }
  while(stackEmpty(&tmp) != 1)
  {
    stackPush(s, tmp.top->item);
    stackPush(d, tmp.top->item);
    stackPop(&tmp);
  }
}
//********Que pomocná při ukládání posloupnosti tokenů pro předání syntaktické analýzy zdola nahoru (pro vyrazy)*******************************
void queError()
{
  fprintf(stderr, "Que error.\n");
  Err = ERR_OTHER;
}

// Inicializace Fronty
void tokenQueInit ( tTokenQue* q ) {
  if(q != NULL)
  {
    q->zac = NULL;
    q->kon = NULL;
  }
  else
    queError();
}

// Vrácí 1 pokud je fronta prázdná jínak vrací 0
int tokenQueEmpty ( const tTokenQue* q ) {
  return q->zac == NULL ? 1 : 0;
}

// Do proměnné c vloží data ze začátku fronty
void tokenQueFront ( const tTokenQue* q, Token* c ) {
  c->type = q->zac->item->type;
  c->keyword = q->zac->item->keyword;
  c->line = q->zac->item->line;
  c->lenght = q->zac->item->lenght;
  c->cap = q->zac->item->cap;
  c->string = q->zac->item->string;
}

// odstraní první prvek fronty
void tokenQueRemove ( tTokenQue* q ) {
  if(q->zac != NULL)
  {
    tTokenItem *tmp = q->zac;
    clearToken(q->zac->item); // vyčištění tokenu
    free(q->zac->item); // uvolnění tokenů
    if(q->zac == q->kon) // ruším jediný prvek, který je ve frontě
      q->kon = NULL;
    q->zac = q->zac->next;
    free(tmp); // uvolnění prvku fronty
  }
}

// Vloží nový prvek na konec fronty
void tokenQueUp ( tTokenQue* q, const Token c ) {
  tTokenItem *new;
  if((new = malloc(sizeof(tTokenItem))) != NULL)
  {
    Token *tmp = malloc(sizeof(Token));
    if(tmp != NULL)
    {
      initToken(tmp);
      new->item = tmp;
      if(c.lenght != 0)
      {
        new->item->string = malloc(sizeof(char)*c.cap);
        if(new->item->string == NULL)
        {
          stackError();
          return;
        }
        strncpy(new->item->string, c.string, c.lenght);
      }
      else
      {
        new->item->string = NULL;
      }
      new->item->type = c.type;
      new->item->keyword = c.keyword;
      new->item->line = c.line;
      new->item->lenght = c.lenght;
      new->item->cap = c.cap;
      new->next = NULL;
      if(q->zac == NULL) // fronta je prázndá
        q->zac = new;
      else // obsahuje nejméně jeden prvek
        q->kon->next = new;
      q->kon = new;
    }
    else
    {
      queError(); 
    }
  }
  else 
    queError();
}

//*******STACK pro syntaktickou analýzu zdola na horů pro výrazy**********************************************
// Inicializuje zasobník
void precStackInit ( tPrecStack* s ) {
  if(s != NULL)
  {
    s->count = 0;
    s->top = NULL;
  }
}

// Vrácí 1 pokud je zásobník prázdný jínak vrací 0
int precStackEmpty ( const tPrecStack* s ) {
  return s->count == 0 ? 1 : 0;
}

// Do proměnné c vloží data z vrcholu zásobníku
void precStackTop ( const tPrecStack* s, precItem* c ) {
  *c = s->top->item;
}

// odstraní vrchol zásobníku
void precStackPop ( tPrecStack* s ) {
  if(!precStackEmpty(s))
  {
    struct tPrecItem *tmp = s->top;
    s->top = s->top->lptr;
    s->count--;
    free(tmp);
  }
}

// Vloží nový prvek na vrchol zásobníku
void precStackPush ( tPrecStack* s, const precItem c ) {
  tPrecItem *new;
  if((new = malloc(sizeof(tPrecItem))) != NULL)
  {
    new->item = c;
    new->lptr = s->top;
    s->top = new;
    s->count++;
  }
  else
  {
    stackError();
  }
} 


//*******STACK pro semántické akce u výrazů**********************************************
// Inicializuje zasobník
void SemStackInit ( tSemStack* s ) {
  if(s != NULL)
  {
    s->count = 0;
    s->top = NULL;
  }
}

// Vrácí 1 pokud je zásobník prázdný jínak vrací 0
int SemStackEmpty ( const tSemStack* s ) {
  return s->count == 0 ? 1 : 0;
}

// Do proměnné c vloží data z vrcholu zásobníku
void SemStackTop ( const tSemStack* s, Token* c ) {
  copyToken(c, &s->top->item);
}

// odstraní vrchol zásobníku
void SemStackPop ( tSemStack* s ) {
  if(!SemStackEmpty(s))
  {
    tSemItem *tmp = s->top;
    s->top = s->top->lptr;
    s->count--;
    
    clearToken(&tmp->item);
    free(tmp);
  }
}

// Vloží nový prvek na vrchol zásobníku
void SemStackPush ( tSemStack* s, Token* c ) {
  tSemItem *new;
  if( (new = malloc(sizeof(tSemItem)) ) != NULL)
  {
    //new->item = malloc(sizeof(Token));
    initToken(&new->item);
    copyToken(&new->item, c);
    
    new->lptr = s->top;
    s->top = new;
    s->count++;
  }
  else
  {
    stackError();
  }
}


//*******STACK pro získání příslušného návěští u IF_ELSE**********************************************
// Inicializuje zasobník
void JumpStackInit ( tJumpStack* s ) {
  if(s != NULL)
  {
    s->count = 0;
    s->top = NULL;
  }
}

// Vrácí 1 pokud je zásobník prázdný jínak vrací 0
int JumpStackEmpty ( const tJumpStack* s ) {
  return s->count == 0 ? 1 : 0;
}

// Do proměnné i vloží data z vrcholu zásobníku
void JumpStackTop ( const tJumpStack* s, int *i ) {
  *i = s->top->item;
}

// odstraní vrchol zásobníku
void JumpStackPop ( tJumpStack* s ) {
  if(!JumpStackEmpty(s))
  {
    tJumpItem *tmp = s->top;
    s->top = s->top->lptr;
    s->count--;
    
    free(tmp);
  }
}

// Vloží nový prvek na vrchol zásobníku
void JumpStackPush ( tJumpStack* s, int i ) {
  tJumpItem *new;
  if( (new = malloc(sizeof(tJumpItem)) ) != NULL)
  {
    new->item = i;
    new->lptr = s->top;
    s->top = new;
    s->count++;
  }
  else
  {
    stackError();
  }
}



