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
 *  Vít Ambrož    (xambro15)
 *  Tomáš Aubrecht  (xaubre02)
 *  David Bednařík  (xbedna62)
 *  Andrej Čulen    (xculen02)
 *  
 * Odkaz na repozitář:
 *  https://github.com/xbedna62/IFJ16
 *
 * Modul:
 *  Parser - synt.analyzátor + sem.analyzátor + generování isntrukcí
 *
 * Autor: 
 *  David Bednařík (xbedna62) - prediktivní synt. analýza + precedenční synt. analýza pro výrazy
 *  Vít Ambrož (xambro15) - sémantické akce, generování tříadresného kódu
 *
 *****/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"
#include "parser.h"
#include "error.h"
#include "ial.h"
#include "tsymbols.h"
#include "kgenerator.h"
#include "interpret.h"

int ROUND = 0;  //proměnná pro určení aktuálního průchodu

FILE* source;   //extern proměnné ze scanneru
int line;

int IF;     //extern proměnná z tsymbols.c

tHTable *TS_Array[MAX_HTSIZE];   //pole tabulek symbolu lokalni urovne pro funkce
int position_in_arr = 0;   //index v poli tabulek symbolu
int n_functions = 0;   //celkový počet funkcí

tHTable *global_table = NULL;  //globální tabulka symbolů

tHTable *tmp_table = NULL;     //hashovací tabulka pro uchování dat o proměnných pro vytvoření instrukce

struct tTabLabel *instr_table = NULL;  //tabulka pro ukládání labelů a instrukcí
struct tStackLabel label_stack;        //zásobník Labelů

//proměnné při vytváření nové instrukce do tabulky labelů
tInstr operator = I_NOP;          //operátor pro aritmetické instrukce
struct sVariable_Data *op1 = NULL;
struct sVariable_Data *op2 = NULL;
struct sVariable_Data *result = NULL;
struct sVariable_Data *opTmp = NULL;
struct sVariable_Data *value_return = NULL;

int substr = 0;     //proměnná pro korektní vytvoření vest. funkce ifj16.substr
int OPERACE = false;  //pokud je operace nastavena na true, bude se pracovat s hodnotami v PrecSA

Token tokenResult;  //ulozeni L-operandu
Token tokenStatic;  //ulozeni key_static
Token tokenJump;    //ulozeni if
Token tokenWhile;   //ulozeni while
Token tokenReturn;  //ulozeni return
Token tokenFun;     //ulozeni id funkce pri jeji definici

tJumpStack jump_stack;    //zásobník integerů potřebný při vnořených podmínkách
//pomocné proměnné potřebné při řešení podmínek a cyklů pro správné sem. akce
int WHILE_CLEAR = 0;
int IF_CLEAR = 0;
int IF_IN_WHILE = 0;
int START_IF = false;
int START_WHILE = false;

Class *AKT_TRIDA = NULL;   //globalni promenna, v jake tride se aktualne nachazime

//dvouprůchodová prediktivní synt. analýza
ErrorType parser_twoRounds()
{
  ErrorType chyba;
  //prvni pruchod
  ROUND = 1;
  chyba = parser();   //ziskani vsech deklaraci a definici, nalezeni redefinic
  
  //pokud byla nalezena chyba behem prvniho pruchodu, do druheho pruchodu se nepokracuje
  if( chyba != ERR_NONE )
  {
    return chyba;
  }
  else
  {
    ROUND = 2;         //nyni bude probihat druhy pruchod
    fseek(source, 0, SEEK_SET);  //nastaveni pointeru v našem souboru na začátek pro další průchod
    line = 1;                    //nastaveni pozice pro údaje od scanneru pro další průchod
  
    chyba = parser();  //v druhem pruchodu semanticke kontroly krome redefinic, generovani 3AC
  }

  return chyba;   //navratova hodnota po druhem pruchodu
}


// prediktivní syntaktická analýza
ErrorType parser()
{
  ErrorType chyba = ERR_NONE;
  Token token;
  tStack s;
  tTokenQue q; // fronta pro ukladaní posloupnosti tokenů při výrazu pro následné poslání do parserExp pro syntaktickou analýzu výrazů
  syntaxRule top;

  int inside_class = false;
  int inside_fun = false;
  int inside_sekv = 0;
  Token tokenTmp;     //ulozeni dat. typu
  Token tokenID;      //ulozeni ID/full.ID
  Token tokenParam;   //ve chvili, kdy se budou hledat parametry/argumenty funkce, do tohoto Tokenu bude uloženo ID funkce
  Token tokenTerm;    //ulozeni Termu
  
  int n_args = 0;   //pomocná proměnná pro určování počtu argumentů
  
  if( ROUND == 1 )  //prvni pruchod
  {
    if ( (global_table = (tHTable*) malloc(sizeof(tHTable))) == NULL)
    {
      return ERR_OTHER;
    }
    htInit(global_table);     //k dispozici je nová tabulka symbolů glob. urovně
    
    if ( (tmp_table = (tHTable*) malloc(sizeof(tHTable))) == NULL)
    {
      chyba = ERR_OTHER;
    }
    htInit(tmp_table);     //k dispozici je nová tabulka symbolů pro uložení konstant a mezivýsledků při výrazech
  }
  
  if( ROUND == 2 )  //druhy pruchod
  {    
    if ( (instr_table = (tTabLabel*) malloc(sizeof(tTabLabel))) == NULL)
    {
      chyba = ERR_OTHER;
    }
    initTab(instr_table);  //k dispozici je nová tabulka labelů+instrukcí
    
    initStackLabel(&label_stack); //inicializace zasobníku labelů
    JumpStackInit(&jump_stack);   //inicializace zasobníku pro konstrukce if/else
  }
  
  tokenQueInit(&q);
  stackInit(&s);
  stackPush(&s, S_TRIDA); // vložím na vrchol zasobníku kořen <trida>
  if(Err != ERR_NONE) // chyba push na zásobník odchycena
  {
    return Err;
  }

  initToken(&token);  //hlavni token pro synt. analýzu
  //nastaveni pomocných tokenů na pocatecni hodnoty
  initToken(&tokenID);
  initToken(&tokenParam);
  initToken(&tokenTerm);
  initToken(&tokenStatic);
  initToken(&tokenTmp);
  initToken(&tokenFun);
  initToken(&tokenJump);
  initToken(&tokenWhile);
  initToken(&tokenReturn);
  initToken(&tokenResult);
  
  processToken(&token);
  int inExp = 0;
  while(1)
  {
    if(token.type == tt_invalid) // když je token neplatný chyba lexikalní analýzy
    {
      chyba = ERR_LEX;
      break;
    }
//**********************************************************************************************************   
    if(stackEmpty(&s) == 1) // kontrola zda je zásobník pázdný
    {
      if(token.type == tt_EOF) // pokude je zásobník prázdný a jsem na konci souboru = úspešný konec
      {
        chyba = ERR_NONE;
      }
      else // pokude je zasobník prázdný ale token není konec souboru chyba
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván Konec souboru.\n", token.line);
      }
      break;
    }
    else // zasobník není prázdný získej co je na vrcholu a pokracuj
    {
      stackTop(&s, &top);
    }
//**********************************************************************************************************    
    // Uplatnění pravidel LL gramatiky => operace expanze
    if(top == S_TRIDA)
    {
      if(token.keyword == key_class) // pravidlo <trida> -> class ID { <telo_tridy> } <trida> {class}
      {
        stackPop(&s);
        stackPush(&s, S_TRIDA);
        stackPush(&s, S_CURLY_RIGHT);
        stackPush(&s, S_TELO_TRIDY);
        stackPush(&s, S_CURLY_LEFT);
        stackPush(&s, S_ID);
        stackPush(&s, S_CLASS);
        
        //semant
        inside_class = true;    //nyni se pohybujeme uvnitr dane tridy
      }
      else if(token.type == tt_EOF) // <trida> -> eps {EOF}
      {
        stackPop(&s); // eps            
        
        //semant
        if( ROUND == 1 )
        {
          tHTItem *item;
          if( (item = htSearch(global_table, "Main:C") ) == NULL )  //trida main bude ulozena pod klicem Main:C
          {
            chyba = ERR_SEM1;
            fprintf(stderr, "Chyba. Nenalezena class Main!\n");
          }         
          if( (item = htSearch(global_table, "Main.run") ) == NULL )
          {
            chyba = ERR_SEM1;
            fprintf(stderr, "Chyba. Nenalezena funkce void run v class Main!\n");
          }
          else if( (item->data->p_uni.function->n_params != 0) || (item->data->p_uni.function->returnType != variable_void) )
          {
            chyba = ERR_SEM1;
            fprintf(stderr, "Chyba. Nenalezena funkce void run!\n");
          }
        }
        //end semant
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekávána class.\n", token.line);
      }
      inExp = 0;
    }
    else if(top == S_TELO_TRIDY)
    {
      if(token.keyword == key_static) // <telo_tridy> -> static <telo_tridy2> {static}
      {
        stackPop(&s);
        stackPush(&s, S_TELO_TRIDY2);
        stackPush(&s, S_STATIC);
      }
      else if(token.type == tt_curly_right) // <telo_tridy> -> eps { '}' }
      { 
        stackPop(&s); // eps
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván static nebo }.\n", token.line);
      }
      inExp = 0;
    }
    else if(top == S_TELO_TRIDY2)
    {
      if(token.keyword == key_int || token.keyword == key_double || token.keyword == key_String) // <telo_tridy2> -> <typ> ID <telo_tridy3> {int,double,String}
      {
        stackPop(&s);
        stackPush(&s, S_TELO_TRIDY3);
        stackPush(&s, S_ID);
        stackPush(&s, S_TYP);
      }
      else if(token.keyword == key_void) // <telo_tridy2> -> void ID ( <param> ) { <sekvence_prikazu> } <telo_tridy> {void}
      {
        stackPop(&s);
        stackPush(&s, S_TELO_TRIDY);
        stackPush(&s, S_CURLY_RIGHT);
        stackPush(&s, S_SEKVENCE_PRIKAZU);
        stackPush(&s, S_CURLY_LEFT);
        stackPush(&s, S_BRACKET_RIGHT);
        stackPush(&s, S_PARAM);
        stackPush(&s, S_BRACKET_LEFT);
        stackPush(&s, S_ID);
        stackPush(&s, S_VOID);
        
        //semant
        if( ROUND == 1 && (TS_Array[position_in_arr] == NULL ) )  //(local_fun_table == NULL )
        {
          if ( (TS_Array[position_in_arr] = (tHTable*) malloc(sizeof(tHTable))) == NULL)
          {
            Err = ERR_OTHER;
          }
          htInit(TS_Array[position_in_arr]);     //k dispozici je nová tabulka symbolů lokalní úrovně pro funkci
        }
        position_in_arr++;    //posuneme index v poli lok. tabulek symbolů
        //end semant
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván static, }, datový typ nebo void.\n", token.line);
      }
      inExp = 0;
    }
    else if(top == S_TELO_TRIDY3)
    {
      if(token.type == tt_equal) // <telo_tridy3> -> = <hodnota> ; <telo_tridy> {=}
      {
        stackPop(&s);
        stackPush(&s, S_TELO_TRIDY);
        stackPush(&s, S_SEMICOLON);
        stackPush(&s, S_HODNOTA);
        stackPush(&s, S_EQUAL);
      }
      else if(token.type == tt_semicolon) // <telo_tridy3> -> ; <telo_tridy> {;}
      {
        stackPop(&s);
        stackPush(&s, S_TELO_TRIDY);
        stackPush(&s, S_SEMICOLON);
      }
      else if(token.type == tt_bracket_left) // <telo_tridy3> -> ( <param> ) { <sekvence_prikazu> } <telo_tridy> {(}
      {
        stackPop(&s);
        stackPush(&s, S_TELO_TRIDY);
        stackPush(&s, S_CURLY_RIGHT);
        stackPush(&s, S_SEKVENCE_PRIKAZU);
        stackPush(&s, S_CURLY_LEFT);
        stackPush(&s, S_BRACKET_RIGHT);
        stackPush(&s, S_PARAM);
        stackPush(&s, S_BRACKET_LEFT);
        
        //semant
        if( ROUND == 1 && (TS_Array[position_in_arr] == NULL ) )
        {
          if ( (TS_Array[position_in_arr] = (tHTable*) malloc(sizeof(tHTable))) == NULL)
          {
            Err = ERR_OTHER;
          }
          htInit(TS_Array[position_in_arr]);     //k dispozici je nová tabulka symbolů lokalní úrovně pro funkci
        }
        position_in_arr++;    //posuneme index v poli lok. tabulek symbolů
        //end semant
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván ;, = nebo (.\n", token.line);
      }
      inExp = 0;
    }
    else if(top == S_PARAM)
    {
      if(token.keyword == key_int || token.keyword == key_double || token.keyword == key_String) // <param> -> <typ> ID <param2> {int, double, String}
      {
        stackPop(&s);
        stackPush(&s, S_PARAM2);
        stackPush(&s, S_ID);
        stackPush(&s, S_TYP);
      }
      else if(token.type == tt_bracket_right) // <param> -> eps {)}
      {
        stackPop(&s);
        
        //semant
        if( ROUND == 1 )
        {
          char *tmp_string;
          tmp_string = add_full_ID(tokenParam.string, AKT_TRIDA->class_name );  //full.id
          tHTItem* item;

          if(tokenID.string == NULL)
          {
            if( (item = htSearch(global_table, tmp_string)) != NULL )  //identifikator funkce nalezen v glob. tabulce symbolu
            {
              item->data->p_uni.function->n_params = 0;  //tato funkce nema zadne parametry
            }
            else
            {
              chyba = ERR_SEM2;
              fprintf(stderr, "Chyba. Řádek %d : Neplatné parametry při definici funkce.\n", token.line);
            }
          }
          clearToken(&tokenID);
          clearToken(&tokenParam);
          free(tmp_string);
        }
        inside_fun = true;    //nyni se pohybujeme uvnitr dane funkce
        //end semant
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván datový typ nebo ).\n", token.line);
      }
      inExp = 0;
    }
    else if(top == S_PARAM2)
    {
      if(token.type == tt_comma) // <param2> -> , <typ> ID <param2>  {,}
      {
        stackPop(&s);
        stackPush(&s, S_PARAM2);
        stackPush(&s, S_ID);
        stackPush(&s, S_TYP);
        stackPush(&s, S_COMMA);
        
        //semant
        if( ROUND == 1 )
        {
          tHTItem* item;
          
          if(tokenID.type == tt_identifier)  //parametr je ID
          {
            char *tmp_string;
            tmp_string = add_full_ID(tokenParam.string, AKT_TRIDA->class_name );
            if( (item = htSearch(global_table, tmp_string)) != NULL )  //identifikator funkce nalezen v glob. tabulce symbolu
            {
              add_Params(item->data->p_uni.function, tokenTmp.keyword, tokenID.string);
              Class *data_par = get_VarData(&tokenTmp, tokenID.string);   //vytvorime zaznam o datovem typu a identifikatoru
              htInsert( TS_Array[position_in_arr - 1], tokenID.string, data_par );   //vlozime ji jako polozku do lok. tabulky symbolu 
            }
            else
            {
              Err = ERR_SEM3;
              fprintf(stderr, "Chyba. Řádek %d : Neplatna definice funkce.\n", token.line);
            }
            free(tmp_string);
          }
          else
          {
            Err = ERR_SEM3;
            fprintf(stderr, "Chyba. Řádek %d : Neplatna definice funkce.\n", token.line);
          }
        }
        clearToken(&tokenID);
        clearToken(&tokenTmp);
        //end semant
      }
      else if(token.type == tt_bracket_right) // <param2> -> eps {)}
      {
        stackPop(&s);
        
        //semant
        if( ROUND == 1 )
        {
          tHTItem* item;

          if(tokenID.type == tt_identifier)  //parametr je ID
          {
            char *tmp_string;
            tmp_string = add_full_ID(tokenParam.string, AKT_TRIDA->class_name );
            if( (item = htSearch(global_table, tmp_string)) != NULL )  //identifikator funkce nalezen v glob. tabulce symbolu
            {
              add_Params(item->data->p_uni.function, tokenTmp.keyword, tokenID.string);   //pridame parametry do tabulky symbolu k dane funkci 
              Class *data_par = get_VarData(&tokenTmp, tokenID.string); //vytvorime zaznam o datovem typu a identifikatoru 
              htInsert ( TS_Array[position_in_arr - 1], tokenID.string, data_par );   //vlozime ji jako polozku do lok. tabulky symbolu
            }
            else
            {
              chyba = ERR_SEM3;
              fprintf(stderr, "Chyba. Řádek %d : Neplatna definice funkce.\n", token.line);
            }
            free(tmp_string);
          }
        }
        clearToken(&tokenID);
        clearToken(&tokenTmp);
        clearToken(&tokenParam);
        inside_fun = true;    //nyni se pohybujeme uvnitr dane funkce
        //end semant
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván , nebo ).\n", token.line);
      }
      inExp = 0;
    }
    else if(top == S_SEKVENCE_PRIKAZU)
    {
      if(token.keyword == key_if) // <sekvence_prikazu> -> <podminka> <sekvence_prikazu> {if}
      {
        stackPop(&s);
        stackPush(&s, S_SEKVENCE_PRIKAZU);
        stackPush(&s, S_PODMINKA);        
      }
      else if(token.keyword == key_while) // <sekvence_prikazu> -> <while> <sekvence_prikazu> {while}
      {
        stackPop(&s);
        stackPush(&s, S_SEKVENCE_PRIKAZU);
        stackPush(&s, S_CYKLUS_WHILE);        
      }
      else if(token.type == tt_curly_left) // <sekvence_prikazu> -> { <slozeny_prikaz> } <sekvence_prikazu> {{}
      {
        stackPop(&s);
        stackPush(&s, S_SEKVENCE_PRIKAZU); 
        stackPush(&s, S_CURLY_RIGHT); 
        stackPush(&s, S_SLOZENY_PRIKAZ);
        stackPush(&s, S_CURLY_LEFT);
        
        //semant
        inside_sekv++;    
      }
      else if(token.keyword == key_int || token.keyword == key_double || token.keyword == key_String || token.keyword == key_static) // <deklarace_promene> <sekvence_prikazu> {static, int, double, String}
      {
        stackPop(&s);
        stackPush(&s, S_SEKVENCE_PRIKAZU);
        stackPush(&s, S_DEKLARACE_PROMENNE);
      }
      else if(token.type == tt_identifier || token.type == tt_full_id) // <sekvence_prikazu> -> <prirazeni||volani_funkce> <sekvence_prikazu> {ID, full_id}
      {
        stackPop(&s);
        stackPush(&s, S_SEKVENCE_PRIKAZU);
        stackPush(&s, S_PRIRAZENI_VOLANI_FUNKCE);
      }
      else if(token.keyword == key_return) // <sekvence_prikazu> -> <narvat> <sekvence_prikazu> {return}
      {
        stackPop(&s);
        stackPush(&s, S_SEKVENCE_PRIKAZU);
        stackPush(&s, S_NAVRAT);
      }
      else if(token.type == tt_curly_right) // sekvence_prikazu> -> eps {}}
      {
        stackPop(&s);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván if, while, {, static, datový typ, identifikátor, return, }.\n", token.line);
      }
      inExp = 0;
    }
    else if(top == S_SLOZENY_PRIKAZ)
    {
      if(token.keyword == key_if) // <slozeny_prikaz> -> <podminka> <slozeny_prikaz> {if}
      {
        stackPop(&s);
        stackPush(&s, S_SLOZENY_PRIKAZ);
        stackPush(&s, S_PODMINKA);
      }
      else if(token.keyword == key_while) // <slozeny_prikaz> -> <while> <slozeny_prikaz> {while}
      {
        stackPop(&s);
        stackPush(&s, S_SLOZENY_PRIKAZ);
        stackPush(&s, S_CYKLUS_WHILE);
      }
      else if(token.type == tt_identifier || token.type == tt_full_id) // <slozeny_prikaz> -> <prirazeni||volani_funkce> <slozeny_prikaz>  {ID, full_id}
      {
        stackPop(&s);
        stackPush(&s, S_SLOZENY_PRIKAZ);
        stackPush(&s, S_PRIRAZENI_VOLANI_FUNKCE);
      }
      else if(token.keyword == key_return) // <slozeny_prikaz> -> <navrat> <slozeny_prikaz>  {return}
      {
        stackPop(&s);
        stackPush(&s, S_SLOZENY_PRIKAZ);
        stackPush(&s, S_NAVRAT);
      }
      else if(token.type == tt_curly_right) // slozeny_prikaz> -> eps  {}}
      {
        stackPop(&s);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván if, while, identifikátor nebo }.\n", token.line);
      }
      inExp = 0;
    }
    else if(top == S_PODMINKA)
    {
      if(token.keyword == key_if) // <podminka> -> if ( <vyraz> ) { <slozeny_prikaz> } <else> {if}
      {
        stackPop(&s);
        stackPush(&s, S_PODMINKA_ELSE);
        stackPush(&s, S_CURLY_RIGHT);
        stackPush(&s, S_SLOZENY_PRIKAZ);
        stackPush(&s, S_CURLY_LEFT);
        stackPush(&s, S_BRACKET_RIGHT);
        stackPush(&s, S_VYRAZ);
        stackPush(&s, S_BRACKET_LEFT);
        stackPush(&s, S_IF);
        
        //semant
        inside_sekv++;
        if( ROUND == 2 )
          START_IF = true;
        //end semant
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván if.\n", token.line);
      }
      inExp = 0;
    }
    else if(top == S_PODMINKA_ELSE)
    {
      if(token.keyword == key_else) // <else> -> else { <slozeny_prikaz> } {else}
      {
        stackPop(&s);
        stackPush(&s, S_CURLY_RIGHT);
        stackPush(&s, S_SLOZENY_PRIKAZ);
        stackPush(&s, S_CURLY_LEFT);
        stackPush(&s, S_ELSE);
        
        //semant
        inside_sekv++;
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván else nebo }.\n", token.line);
      }
      inExp = 0;
    }
    else if(top == S_CYKLUS_WHILE) // <while> -> while ( <vyraz> ) { <slozeny_prikaz> } {while}
    {
      if(token.keyword == key_while)
      {
        stackPop(&s);
        stackPush(&s, S_CURLY_RIGHT);
        stackPush(&s, S_SLOZENY_PRIKAZ);
        stackPush(&s, S_CURLY_LEFT);
        stackPush(&s, S_BRACKET_RIGHT);
        stackPush(&s, S_VYRAZ);
        stackPush(&s, S_BRACKET_LEFT);
        stackPush(&s, S_WHILE);
        
        //semant
        inside_sekv++;
        if( ROUND == 2 )
          START_WHILE = true;
        //end semant
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván while.\n", token.line);
      }
      inExp = 0;
    }
    else if(top == S_DEKLARACE_PROMENNE)
    {
      if(token.keyword == key_static) // <deklarace_promene> static <typ> id <inicializace> {static}
      {
        stackPop(&s);
        stackPush(&s, S_INICIALIZACE);
        stackPush(&s, S_ID);
        stackPush(&s, S_TYP);
        stackPush(&s, S_STATIC);
      }
      else if(token.keyword == key_int || token.keyword == key_double || token.keyword == key_String) // <deklarace_promene> <typ> id <inicializace> {int, double, String}
      {
        stackPop(&s);
        stackPush(&s, S_INICIALIZACE);
        stackPush(&s, S_ID);
        stackPush(&s, S_TYP);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván static nebo datový typ.\n", token.line);
      }
      inExp = 0;
    }
    else if(top == S_INICIALIZACE)
    {
      if(token.type == tt_semicolon) // <inicializace> -> ; {;}
      {
        stackPop(&s);
        stackPush(&s, S_SEMICOLON);
      }
      else if(token.type == tt_equal) // <inicializace> -> = <hodnota> ; {=}
      {
        stackPop(&s);
        stackPush(&s, S_SEMICOLON);
        stackPush(&s, S_HODNOTA);
        stackPush(&s, S_EQUAL);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván ; nebo =.\n", token.line);
      }
      inExp = 0;
    }
    else if(top == S_PRIRAZENI_VOLANI_FUNKCE)
    {
      if(token.type == tt_identifier || token.type == tt_full_id) // <prirazeni||volani_funkce> -> id <prirazeni||volani_funkce2> {ID, full_id}
      {
        stackPop(&s);
        stackPush(&s, S_PRIRAZENI_VOLANI_FUNKCE2);
        stackPush(&s, S_ID_FULL_ID);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván identifikátor.\n", token.line);
      }
      inExp = 0;
    }
    else if(top == S_PRIRAZENI_VOLANI_FUNKCE2)
    {
      if(token.type == tt_equal) // <prirazeni||volani_funkce2> -> = <hodnota> ; {=}
      {
        stackPop(&s);
        stackPush(&s, S_SEMICOLON);
        stackPush(&s, S_HODNOTA);
        stackPush(&s, S_EQUAL);
      }
      else if(token.type == tt_bracket_left) // <prirazeni||volani_funkce2> -> ( <arg> ) ; {(} 
      {
        stackPop(&s);
        stackPush(&s, S_SEMICOLON);
        stackPush(&s, S_BRACKET_RIGHT);
        stackPush(&s, S_ARG);    
        stackPush(&s, S_BRACKET_LEFT);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáváno = nebo (.\n", token.line);
      }
      inExp = 0;
    }
    else if(top == S_ARG)
    {
      if(token.type == tt_identifier || token.type == tt_full_id || token.type == tt_int || token.type == tt_double || token.type == tt_literal) // <arg> -> term <arg2> {term}
      {
        stackPop(&s);
        stackPush(&s, S_ARG2);
        stackPush(&s, S_TERM);
      }
      else if(token.type == tt_bracket_right) // <arg> -> eps {)}
      {
        stackPop(&s);
        
        //semant
        if( ROUND == 2 )
        {
          //volání vestavěné funkce bez parametrů
          if( strcmp(tokenParam.string, "ifj16.readString") == 0 || strcmp(tokenParam.string, "ifj16.readDouble") == 0 
            || strcmp(tokenParam.string, "ifj16.readInt") == 0)
          {
            if( emptyStackLabel(&label_stack) != 1 )
            {
              tHTItem *item;         
              Class *new = Create_Var_to_Table();  //vytvoření nové proměnné
                
              Token tokenKey;
              initToken(&tokenKey);
              Create_Token_string( &tokenKey, "read" );
                  
              if( Err == ERR_NONE )
              {
                htInsert ( tmp_table, tokenKey.string, new );  //vložení nové proměnné do TS
                item = htSearch( tmp_table, tokenKey.string );
                result = item->data->p_uni.variable;
              }
              clearToken(&tokenKey);
              
              char *akt_label = NULL;
              int TypeFun;
              topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
              
              //generování instrukcí vest. funkcí
              if( strcmp(tokenParam.string, "ifj16.readInt") == 0 )
                insertTab( instr_table, akt_label, I_READ_INT, NULL, NULL, result, NULL);
              if( strcmp(tokenParam.string, "ifj16.readDouble") == 0 )
                insertTab( instr_table, akt_label, I_READ_DOUBLE, NULL, NULL, result, NULL);
              if( strcmp(tokenParam.string, "ifj16.readString") == 0 )
                insertTab( instr_table, akt_label, I_READ_STRING, NULL, NULL, result, NULL);
                
              if( tokenResult.type == tt_identifier )
              {
                char *tmp_string;
                tmp_string = add_full_ID( tokenResult.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem full.ID
                if( (item = htSearch(TS_Array[position_in_arr - 1], tokenResult.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                {
                  if( (item = htSearch(global_table, tmp_string)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                  {
                    chyba = ERR_SEM1;   //nedeklarovana promenna
                    fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                  }
                }
                if( chyba == ERR_NONE )
                {
                  op1 = item->data->p_uni.variable;  //op1 bude L-operand
                }
                insertTab( instr_table, akt_label, I_ASSIGN, op1, result, NULL, NULL);  //přiřazení návratové hodnoty funkce
                
                free(tmp_string);
              }
              
              else if( tokenResult.type == tt_full_id )
              {
                if( (item = htSearch(global_table, tokenResult.string)) == NULL )  //muze se jednat o full_id
                {
                  chyba = ERR_SEM1;   //nedeklarovana promenna
                  fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                }
                if( chyba == ERR_NONE )
                    op1 = item->data->p_uni.variable;  //op1 bude full.id
                
                insertTab( instr_table, akt_label, I_ASSIGN, op1, result, NULL, NULL);  //přiřazení návratové hodnoty funkce
              }
              clearToken(&tokenResult);
            } 
          }
          else if( strcmp(tokenParam.string, "ifj16.find") == 0 || strcmp(tokenParam.string, "ifj16.print") == 0 
            || strcmp(tokenParam.string, "ifj16.sort") == 0 || strcmp(tokenParam.string, "ifj16.compare") == 0 
            || strcmp(tokenParam.string, "ifj16.substr") == 0 || strcmp(tokenParam.string, "ifj16.lenght") == 0 )
          {
            chyba = ERR_SEM2;   //spatny pocet argumentu
            fprintf(stderr, "Chyba. Řádek %d : Chybný počet argumentů!\n", token.line);
          } 
          else   //jedná se o uživatelskou funkci bez parametrů
          {
            tHTItem *item;
            
            if( tokenParam.type == tt_identifier )
            {
              char *tmp_string;
              tmp_string = add_full_ID( tokenParam.string, AKT_TRIDA->class_name );  //jedna se o funkci-> klic Class.ID
              if( (item = htSearch(global_table, tmp_string) ) == NULL )  //funkce nenalezena v glob. tabulce symbolu
              {
                chyba = ERR_SEM1;   //nedefinovana funkce
                fprintf(stderr, "Chyba. Řádek %d : nedefinovaná funkce.\n", token.line);
              }
              else if( item->data->p_uni.function->n_params != 0 )  //neodpovídající počet parametrů
              {
                chyba = ERR_SEM2;   //spatny pocet argumentu
                fprintf(stderr, "Chyba. Řádek %d : Chybný počet argumentů!\n", token.line);
              }
              
              if( chyba == ERR_NONE )
              {
                char *akt_label = NULL;
                int TypeFun;
                topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
                
                if( item->data->p_uni.function->returnType == variable_void ) 
                  insertTab( instr_table, akt_label, I_FCEVJUMP, NULL, NULL, NULL, tmp_string);  //instrukce pro void funkci
                else
                {
                  char *tmp_string2;
                  insertTab( instr_table, akt_label, I_FCEJUMP, NULL, NULL, NULL, tmp_string);  //instrukce pro non-void funkce
                  
                  if( tokenResult.type == tt_identifier )
                  {
                    tmp_string2 = add_full_ID( tokenResult.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem full.ID
                    if( (item = htSearch(TS_Array[position_in_arr - 1], tokenResult.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                    {
                      if( (item = htSearch(global_table, tmp_string2)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                      {
                        chyba = ERR_SEM1;   //nedeklarovana promenna
                        fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                      }
                    }
                    if( chyba == ERR_NONE )
                    {
                      op1 = item->data->p_uni.variable;  //L-operand bude ID
                      
                      item = htSearch(tmp_table, tmp_string);
                      value_return = item->data->p_uni.variable;  //návratová hodnota

                      insertTab( instr_table, akt_label, I_ASSIGN, op1, value_return, NULL, NULL);
                    }
                    free(tmp_string2);
                    value_return = NULL;
                  }
                  
                  else if( tokenResult.type == tt_full_id )
                  {
                    if( (item = htSearch(global_table, tokenResult.string)) == NULL )  //muze se jednat o full_id
                    {
                      chyba = ERR_SEM1;   //nedeklarovana promenna
                      fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                    }
                    if( chyba == ERR_NONE )
                    {
                      op1 = item->data->p_uni.variable;  //L-operand bude full.ID
                      
                      item = htSearch(tmp_table, tmp_string);
                      value_return = item->data->p_uni.variable;  //návratová hodnota
                      
                      insertTab( instr_table, akt_label, I_ASSIGN, op1, value_return, NULL, NULL);
                    }
                    value_return = NULL;
                  }
                  clearToken(&tokenResult);
                }
              }
              free(tmp_string);
            }
            
            else if( tokenParam.type == tt_full_id )  //volání funkce pomocí full.ID
            {
              if( (item = htSearch(global_table, tokenParam.string) ) == NULL )  //funkce nenalezena v glob. tabulce symbolu
              {
                chyba = ERR_SEM1;   //nedefinovana funkce
                fprintf(stderr, "Chyba. Řádek %d : nedefinovaná funkce.\n", token.line);
              }
              else if( item->data->p_uni.function->n_params != 0 )
              {
                chyba = ERR_SEM2;   //spatny pocet argumentu teto funkce
                fprintf(stderr, "Chyba. Řádek %d : Chybný počet argumentů!\n", token.line);
              }
              if( chyba == ERR_NONE )
              {
                char *akt_label = NULL;
                int TypeFun;
                topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
                
                if( item->data->p_uni.function->returnType == variable_void ) 
                  insertTab( instr_table, akt_label, I_FCEVJUMP, NULL, NULL, NULL, tokenParam.string);  //void funkce
                else
                {
                  insertTab( instr_table, akt_label, I_FCEJUMP, NULL, NULL, NULL, tokenParam.string);  //non.void funkce
                  
                  if( tokenResult.type == tt_identifier )
                  {
                    char *tmp_string2;
                    tmp_string2 = add_full_ID( tokenResult.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem ID:V
                    if( (item = htSearch(TS_Array[position_in_arr - 1], tokenResult.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                    {
                      if( (item = htSearch(global_table, tmp_string2)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                      {
                        chyba = ERR_SEM1;   //nedeklarovana promenna
                        fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                      }
                    }
                    if( chyba == ERR_NONE )
                    {
                      op1 = item->data->p_uni.variable;  //L-operand bude ID
                    }
                    item = htSearch(tmp_table, tokenParam.string);
                    value_return = item->data->p_uni.variable;  //návratová hodnota
                    
                    insertTab( instr_table, akt_label, I_ASSIGN, op1, value_return, NULL, NULL);
                    
                    free(tmp_string2);
                    value_return = NULL;
                  }
                  
                  else if( tokenResult.type == tt_full_id )
                  {
                    if( (item = htSearch(global_table, tokenResult.string)) == NULL )  //muze se jednat o full_id
                    {
                      chyba = ERR_SEM1;   //nedeklarovana promenna
                      fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                    }
                    if( chyba == ERR_NONE )
                        op1 = item->data->p_uni.variable;  //L-operand bude full.ID
                    
                    item = htSearch(tmp_table, tokenParam.string);
                    value_return = item->data->p_uni.variable;  //návratová hodnota
                    
                    insertTab( instr_table, akt_label, I_ASSIGN, op1, value_return, NULL, NULL);
                    value_return = NULL;
                  }
                  clearToken(&tokenResult);
                }
              }
            }
          }
          clearToken(&tokenParam);
        }
        
        n_args = 0;  //obnovime nasi promennou pro pocet argumentu na poc. hodnotu
        //semant end
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekávána identifikátor, literál, číslo.\n", token.line);
      }
      inExp = 0;
    }
    else if(top == S_ARG2)
    {
      if(token.type == tt_bracket_right) // <arg2> -> eps {)}
      {
        stackPop(&s);
        
        //semant
        if( ROUND == 2 )
        {     
          n_args++;  //inkrementujeme pocet argumentu teto funkce
          
          if( strcmp(tokenParam.string, "ifj16.print") == 0 || strcmp(tokenParam.string, "ifj16.sort") == 0 
          || strcmp(tokenParam.string, "ifj16.lenght") == 0 )  //nedořešeno lenght a sort
          {
            tHTItem *item;  
            if( tokenTerm.type == tt_int || tokenTerm.type == tt_double || tokenTerm.type == tt_literal )
            {
              Class *new = Create_Const_to_Table(&tokenTerm);  //vytvoření nového literálu
              
              Token tokenKey;
              initToken(&tokenKey);
              Create_Token_string( &tokenKey, "print" );
                    
              if( Err == ERR_NONE )
              {
                htInsert ( tmp_table, tokenKey.string, new );
                item = htSearch( tmp_table, tokenKey.string );
                op1 = item->data->p_uni.variable;  //uložen ukazatel do TS, kde je příslušný literál
              }
              clearToken(&tokenKey);
            }
            
            else if( tokenTerm.type == tt_identifier )
            {
              char *tmp_string;
              tmp_string = add_full_ID( tokenTerm.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem ID:V
              if( (item = htSearch(TS_Array[position_in_arr - 1], tokenTerm.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
              {
                if( (item = htSearch(global_table, tmp_string)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                {
                  chyba = ERR_SEM1;   //nedeklarovana promenna
                  fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                }
              }
              if( chyba == ERR_NONE )
              {
                op1 = item->data->p_uni.variable;  //uložen ukazatel do TS na příslušný ID
              }
              free(tmp_string);
            }
            
            else if( tokenTerm.type == tt_full_id )
            {
              if( (item = htSearch(global_table, tokenTerm.string)) == NULL )  //muze se jednat o full_id
              {
                chyba = ERR_SEM1;   //nedeklarovana promenna
                fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
              }
              if( chyba == ERR_NONE )
                  op1 = item->data->p_uni.variable;  //uložen ukazatel do TS na příslušný full.ID
            }
            
            Class *new = Create_Var_to_Table();   //vytvoření nové proměnné pro výsledek
            Token tokenKey;
            initToken(&tokenKey);
            Create_Token_string( &tokenKey, "result" );
                  
            if( Err == ERR_NONE )
            {
              htInsert ( tmp_table, tokenKey.string, new );
              item = htSearch( tmp_table, tokenKey.string );
              result = item->data->p_uni.variable;   //získání ukazatele do TS pro návratovou hodnotu
            }
            clearToken(&tokenKey);
            
            
            char *akt_label = NULL;
            int TypeFun;
            topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
            
            if( chyba == ERR_NONE )
            { 
              if( strcmp(tokenParam.string, "ifj16.print") == 0 )
                insertTab( instr_table, akt_label, I_PRINT, op1, NULL, NULL, NULL);
              if( strcmp(tokenParam.string, "ifj16.lenght") == 0 )
                insertTab( instr_table, akt_label, I_LENGTH, op1, NULL, result, NULL);
              if( strcmp(tokenParam.string, "ifj16.sort") == 0 )
                insertTab( instr_table, akt_label, I_SORT, op1, NULL, result, NULL);
              
              if( strcmp(tokenParam.string, "ifj16.sort") == 0 || strcmp(tokenParam.string, "ifj16.lenght") == 0 )
              {
                if( tokenResult.type == tt_identifier )
                {
                  char *tmp_string;
                  tmp_string = add_full_ID( tokenResult.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem ID:V
                  if( (item = htSearch(TS_Array[position_in_arr - 1], tokenResult.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                  {
                    if( (item = htSearch(global_table, tmp_string)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                    {
                      chyba = ERR_SEM1;   //nedeklarovana promenna
                      fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                    }
                  }
                  if( chyba == ERR_NONE )
                  {
                    op1 = item->data->p_uni.variable;  //L-operand bude ID
                  }
                  insertTab( instr_table, akt_label, I_ASSIGN, op1, result, NULL, NULL);
                  
                  free(tmp_string);
                }
                
                else if( tokenResult.type == tt_full_id )
                {
                  if( (item = htSearch(global_table, tokenResult.string)) == NULL )  //muze se jednat o full_id
                  {
                    chyba = ERR_SEM1;   //nedeklarovana promenna
                    fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                  }
                  if( chyba == ERR_NONE )
                      op1 = item->data->p_uni.variable;  //L-operand bude full.ID
                  
                  insertTab( instr_table, akt_label, I_ASSIGN, op1, result, NULL, NULL);
                }
                clearToken(&tokenResult);
              }
            }
          }
          else if( strcmp(tokenParam.string, "ifj16.find") == 0 || strcmp(tokenParam.string, "ifj16.compare") == 0 
          || strcmp(tokenParam.string, "ifj16.substr") == 0 )
          {
            if( strcmp(tokenParam.string, "ifj16.substr") == 0 )  //vestavěná fce substr
            {
              tHTItem *item;
              if( substr == 3 )  //řešení posledního argumentu
              {  
                if( tokenTerm.type == tt_int || tokenTerm.type == tt_double || tokenTerm.type == tt_literal )
                {
                  Class *new = Create_Const_to_Table(&tokenTerm);  //vytvoření literálu
                  
                  Token tokenKey;
                  initToken(&tokenKey);
                  Create_Token_string( &tokenKey, "param" );
                      
                  if( Err == ERR_NONE )
                  {
                    htInsert ( tmp_table, tokenKey.string, new );  //vložení do pomocné TS
                    item = htSearch( tmp_table, tokenKey.string );
                    op2 = item->data->p_uni.variable;      //uložení ukazatele
                  }
                  clearToken(&tokenKey);
                  substr = 4;            //navýšení proměnné pro identifikaci fáze v této funkci
                }
                
                else if( tokenTerm.type == tt_identifier )
                {
                  char *tmp_string;
                  tmp_string = add_full_ID( tokenTerm.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem ID:V
                  if( (item = htSearch(TS_Array[position_in_arr - 1], tokenTerm.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                  {
                    if( (item = htSearch(global_table, tmp_string)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                    {
                      chyba = ERR_SEM1;   //nedeklarovana promenna
                      fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                    }
                  }
                  if( chyba == ERR_NONE )
                  {
                    op2 = item->data->p_uni.variable;   //uložení ukazatele do TS na poslední argument
                  }
                  free(tmp_string);
                  substr = 4;    //navýšení proměnné pro identifikaci fáze v této funkci
                }
                
                else if( tokenTerm.type == tt_full_id )
                {
                  if( (item = htSearch(global_table, tokenTerm.string)) == NULL )  //muze se jednat o full_id
                  {
                    chyba = ERR_SEM1;   //nedeklarovana promenna
                    fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                  }
                  if( chyba == ERR_NONE )
                      op2 = item->data->p_uni.variable;  //uložení ukazatele do TS na poslední argument

                  substr = 4;  //navýšení proměnné pro identifikaci fáze v této funkci
                }
              }

                
              if( substr == 4 )   //samotné generování instrukce pro tuto funkci
              {
                Class *new = Create_Var_to_Table();
                Token tokenKey;
                initToken(&tokenKey);
                Create_Token_string( &tokenKey, "result" );
                      
                if( Err == ERR_NONE )
                {
                  htInsert ( tmp_table, tokenKey.string, new );
                  item = htSearch( tmp_table, tokenKey.string );
                  result = item->data->p_uni.variable;    //ukazatel na návratovou hodnotu této funkce
                }
                clearToken(&tokenKey);
                
                char *akt_label = NULL;
                int TypeFun;
                topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
                
                //generování druhé instrukce této funkce
                insertTab( instr_table, akt_label, I_SUBSTR2, opTmp, op2, result, NULL);

                if( tokenResult.type == tt_identifier )
                {
                  char *tmp_string;
                  tmp_string = add_full_ID( tokenResult.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem full.ID
                  if( (item = htSearch(TS_Array[position_in_arr - 1], tokenResult.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                  {
                    if( (item = htSearch(global_table, tmp_string)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                    {
                      chyba = ERR_SEM1;   //nedeklarovana promenna
                      fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                    }
                  }
                  if( chyba == ERR_NONE )
                  {
                    op1 = item->data->p_uni.variable;  //L-operand bude ID
                  }
                  insertTab( instr_table, akt_label, I_ASSIGN, op1, result, NULL, NULL);
                    
                  free(tmp_string);
                }
                  
                else if( tokenResult.type == tt_full_id )
                {
                  if( (item = htSearch(global_table, tokenResult.string)) == NULL )  //muze se jednat o full_id
                  {
                    chyba = ERR_SEM1;   //nedeklarovana promenna
                    fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                  }
                  if( chyba == ERR_NONE )
                    op1 = item->data->p_uni.variable;  //L-operand bude Full.id
                    
                  insertTab( instr_table, akt_label, I_ASSIGN, op1, result, NULL, NULL);
                }
                clearToken(&tokenResult);
              }
              else
              {
                chyba = ERR_SEM2;   //spatny pocet argumentu
                fprintf(stderr, "Chyba. Řádek %d : Chybný počet argumentů!\n", token.line);
              }
              substr = 0;
            }
            else
            {
              tHTItem *item;  
              if( tokenTerm.type == tt_int || tokenTerm.type == tt_double || tokenTerm.type == tt_literal )
              {
                Class *new = Create_Const_to_Table(&tokenTerm);  //vytvoření literálu
                
                Token tokenKey;
                initToken(&tokenKey);
                Create_Token_string( &tokenKey, "param" );
                      
                if( Err == ERR_NONE )
                {
                  htInsert ( tmp_table, tokenKey.string, new );  //vložení do pomocné TS
                  item = htSearch( tmp_table, tokenKey.string );
                  op2 = item->data->p_uni.variable;    //získání ukazatele do pomocné TS na druhý argument
                }
                clearToken(&tokenKey);
              }
              
              else if( tokenTerm.type == tt_identifier )
              {
                char *tmp_string;
                tmp_string = add_full_ID( tokenTerm.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem ID:V
                if( (item = htSearch(TS_Array[position_in_arr - 1], tokenTerm.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                {
                  if( (item = htSearch(global_table, tmp_string)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                  {
                    chyba = ERR_SEM1;   //nedeklarovana promenna
                    fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                  }
                }
                if( chyba == ERR_NONE )
                {
                  op2 = item->data->p_uni.variable;  //získání ukazatele do pomocné TS na druhý argument
                }
                free(tmp_string);
              }
              
              else if( tokenTerm.type == tt_full_id )
              {
                if( (item = htSearch(global_table, tokenTerm.string)) == NULL )  //muze se jednat o full_id
                {
                  chyba = ERR_SEM1;   //nedeklarovana promenna
                  fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                }
                if( chyba == ERR_NONE )
                    op2 = item->data->p_uni.variable;  //získání ukazatele do pomocné TS na druhý argument
              }
              
              Class *new = Create_Var_to_Table();  //vytvoření proměnné pro návratovou hodnotu
              Token tokenKey;
              initToken(&tokenKey);
              Create_Token_string( &tokenKey, "result" );
                    
              if( Err == ERR_NONE )
              {
                htInsert ( tmp_table, tokenKey.string, new );
                item = htSearch( tmp_table, tokenKey.string );
                result = item->data->p_uni.variable;    //získání ukazatele do pomocné TS na návratovou hodnotu
              }
              clearToken(&tokenKey);
              
              
              char *akt_label = NULL;
              int TypeFun;
              topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
              
              if( chyba == ERR_NONE )
              { 
                if( strcmp(tokenParam.string, "ifj16.compare") == 0 )
                  insertTab( instr_table, akt_label, I_COMPARE, op1, op2, result, NULL);
                if( strcmp(tokenParam.string, "ifj16.find") == 0 )
                  insertTab( instr_table, akt_label, I_FIND, op1, op2, result, NULL);
                
                if( tokenResult.type == tt_identifier )
                {
                  char *tmp_string;
                  tmp_string = add_full_ID( tokenResult.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem ID:V
                  if( (item = htSearch(TS_Array[position_in_arr - 1], tokenResult.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                  {
                    if( (item = htSearch(global_table, tmp_string)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                    {
                      chyba = ERR_SEM1;   //nedeklarovana promenna
                      fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                    }
                  }
                  if( chyba == ERR_NONE )
                  {
                    op1 = item->data->p_uni.variable;   //L-operand bude ID
                  }
                  insertTab( instr_table, akt_label, I_ASSIGN, op1, result, NULL, NULL);
                    
                  free(tmp_string);
                }
                  
                else if( tokenResult.type == tt_full_id )
                {
                  if( (item = htSearch(global_table, tokenResult.string)) == NULL )  //muze se jednat o full_id
                  {
                    chyba = ERR_SEM1;   //nedeklarovana promenna
                    fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                  }
                  if( chyba == ERR_NONE )
                    op1 = item->data->p_uni.variable;   //L-operand bude full.ID
                    
                  insertTab( instr_table, akt_label, I_ASSIGN, op1, result, NULL, NULL);
                }
                clearToken(&tokenResult);
              }
            }
          }
          else if( strcmp(tokenParam.string, "ifj16.readString") == 0 || strcmp(tokenParam.string, "ifj16.readDouble") == 0 
          || strcmp(tokenParam.string, "ifj16.readInt") == 0 )
          {
            chyba = ERR_SEM2;   //spatny pocet argumentu
            fprintf(stderr, "Chyba. Řádek %d : Chybný počet argumentů!\n", token.line);
          }
          else   //volání uživatelské funkce s alespoň jedním argumentem
          {
            tHTItem *item;
            
            char *akt_label = NULL;
            int TypeFun;
            topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );  //aktuální label
            
            if( tokenParam.type == tt_identifier )
            {
              char *tmp_string;
              tmp_string = add_full_ID( tokenParam.string, AKT_TRIDA->class_name );  //jedna se u funkci-> klic Class.ID
              if( (item = htSearch(global_table, tmp_string) ) == NULL )  //funkce nenalezena v glob. tabulce symbolu
              {
                chyba = ERR_SEM1;   //nedefinovana funkce
                fprintf(stderr, "Chyba. Řádek %d : nedefinovaná funkce.\n", token.line);
              }
              else if( item->data->p_uni.function->n_params != n_args )
              {
                chyba = ERR_SEM2;   //spatny pocet argumentu
                fprintf(stderr, "Chyba. Řádek %d : Chybný počet argumentů!\n", token.line);
              }
              else if( tokenTerm.type != tt_empty )
              {
                tHTItem* item2;
                char *tmp_string2;
                chyba = check_argument_type( item->data, tokenTerm, (n_args-1) );  //kontrola typu argumentu
                if( chyba != ERR_NONE )
                {
                  chyba = ERR_SEM2;   //chybny typ argumentu
                  fprintf(stderr, "Chyba. Řádek %d : Chybný typ argumentu funkce!\n", token.line);
                }
                
                else if( tokenTerm.type == tt_identifier )  //jedna se o jednoduchy identifikator v argumentu
                {
                  tmp_string2 = add_full_ID( tokenTerm.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem ID:V
                  if( (item2 = htSearch(TS_Array[position_in_arr - 1], tokenTerm.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                  {
                    if( (item2 = htSearch(global_table, tmp_string2)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                    {
                      chyba = ERR_SEM1;   //nedeklarovana promenna
                      fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                    }
                    else if( (item->data->p_uni.function->Params[n_args-1].param_type) != (item2->data->p_uni.variable->var_type) )
                    {
                      chyba = ERR_SEM2;   //chybny typ argumentu
                      fprintf(stderr, "Chyba. Řádek %d : Chybný typ argumentu funkce!\n", token.line);
                    }
                  }
                  else if( (item->data->p_uni.function->Params[n_args-1].param_type) != (item2->data->p_uni.variable->var_type) )
                  {
                    chyba = ERR_SEM2;   //chybny typ argumentu
                    fprintf(stderr, "Chyba. Řádek %d : Chybný typ argumentu funkce!\n", token.line);
                  }
                  free(tmp_string2);
                  
                  if( chyba == ERR_NONE )
                  {
                    tHTItem *item3;
                    item3 = htSearch(TS_Array[item->data->p_uni.function->position_arr], item->data->p_uni.function->Params[n_args-1].name);  //nalezení argumentu v lok. TS
                    
                    if( item3 != NULL )
                      insertTab( instr_table, akt_label, I_ASSIGN, item3->data->p_uni.variable, item2->data->p_uni.variable, NULL, NULL);
                    
                    if( item->data->p_uni.function->returnType == variable_void ) 
                      insertTab( instr_table, akt_label, I_FCEVJUMP, NULL, NULL, NULL, tmp_string);  //void funkce
                    else
                    {
                      insertTab( instr_table, akt_label, I_FCEJUMP, NULL, NULL, NULL, tmp_string);   //non-void funkce
                      
                      if( tokenResult.type == tt_identifier )
                      {
                        tmp_string2 = add_full_ID( tokenResult.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem full.ID
                        if( (item = htSearch(TS_Array[position_in_arr - 1], tokenResult.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                        {
                          if( (item = htSearch(global_table, tmp_string2)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                          {
                            chyba = ERR_SEM1;   //nedeklarovana promenna
                            fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                          }
                        }
                        if( chyba == ERR_NONE )
                        {
                          op1 = item->data->p_uni.variable;   //L-operand bude ID
                        }
                        item = htSearch(tmp_table, tmp_string);
                        value_return = item->data->p_uni.variable;  //přiřazovat se bude návr. hodnota funkce
                        
                        insertTab( instr_table, akt_label, I_ASSIGN, op1, value_return, NULL, NULL);
                        
                        free(tmp_string2);
                        value_return = NULL;
                      }
                      
                      else if( tokenResult.type == tt_full_id )
                      {
                        if( (item = htSearch(global_table, tokenResult.string)) == NULL )  //muze se jednat o full_id
                        {
                          chyba = ERR_SEM1;   //nedeklarovana promenna
                          fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                        }
                        if( chyba == ERR_NONE )
                            op1 = item->data->p_uni.variable;  //L-operand bude full.ID
                        
                        item = htSearch(tmp_table, tmp_string);
                        value_return = item->data->p_uni.variable;  //přiřazovat se bude návr. hodnota funkce
                        
                        insertTab( instr_table, akt_label, I_ASSIGN, op1, value_return, NULL, NULL);
                        value_return = NULL;
                      }
                      clearToken(&tokenResult);
                    }
                  } 
                }
                
                else if( tokenTerm.type == tt_full_id )   //jedna se o full identifikator v argumentu
                {
                  if( (item2 = htSearch(global_table, tokenTerm.string)) == NULL )
                  {
                    chyba = ERR_SEM1;   //nedeklarovana promenna
                    fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                  }
                  else if( (item->data->p_uni.function->Params[n_args-1].param_type) != (item2->data->p_uni.variable->var_type) )
                  {
                    chyba = ERR_SEM2;   //chybny typ argumentu
                    fprintf(stderr, "Chyba. Řádek %d : Chybný typ argumentu funkce!\n", token.line);
                  }
                  
                  if( chyba == ERR_NONE )
                  {
                    tHTItem *item3;
                    item3 = htSearch(TS_Array[item->data->p_uni.function->position_arr], item->data->p_uni.function->Params[n_args-1].name); //nalezení parametru v lok. TS
                    
                    if( item3 != NULL )
                      insertTab( instr_table, akt_label, I_ASSIGN, item3->data->p_uni.variable, item2->data->p_uni.variable, NULL, NULL);
                    
                    if( item->data->p_uni.function->returnType == variable_void ) 
                      insertTab( instr_table, akt_label, I_FCEVJUMP, NULL, NULL, NULL, tmp_string);   //void funkce
                    else
                    {
                      insertTab( instr_table, akt_label, I_FCEJUMP, NULL, NULL, NULL, tmp_string);   //non-void funkce
                      
                      if( tokenResult.type == tt_identifier )
                      {
                        tmp_string2 = add_full_ID( tokenResult.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem ID:V
                        if( (item = htSearch(TS_Array[position_in_arr - 1], tokenResult.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                        {
                          if( (item = htSearch(global_table, tmp_string2)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                          {
                            chyba = ERR_SEM1;   //nedeklarovana promenna
                            fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                          }
                        }
                        if( chyba == ERR_NONE )
                        {
                          op1 = item->data->p_uni.variable;   //L-operand bude ID
                        }
                        item = htSearch(tmp_table, tmp_string);
                        value_return = item->data->p_uni.variable;  //získání ukazatele na návratovou hodnotu
                        
                        insertTab( instr_table, akt_label, I_ASSIGN, op1, value_return, NULL, NULL);
                        
                        free(tmp_string2);
                        value_return = NULL;
                      }
                      
                      else if( tokenResult.type == tt_full_id )
                      {
                        if( (item = htSearch(global_table, tokenResult.string)) == NULL )  //muze se jednat o full_id
                        {
                          chyba = ERR_SEM1;   //nedeklarovana promenna
                          fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                        }
                        if( chyba == ERR_NONE )
                            op1 = item->data->p_uni.variable;  //L-operand bude full.ID
                        
                        item = htSearch(tmp_table, tmp_string);
                        value_return = item->data->p_uni.variable;  //získání ukazatele na návratovou hodnotu
                        
                        insertTab( instr_table, akt_label, I_ASSIGN, op1, value_return, NULL, NULL);
                        value_return = NULL;
                      }
                      clearToken(&tokenResult);
                    }
                  }
                }
                
                else if( tokenTerm.type == tt_int || tokenTerm.type == tt_double || tokenTerm.type == tt_literal )
                {
                  Class *new = Create_Const_to_Table(&tokenTerm);
                        
                  Token tokenKey;
                  initToken(&tokenKey);
                  Create_Token_string( &tokenKey, "const" );
                  
                  if( Err == ERR_NONE )
                  {
                    tHTItem *item4;
                    htInsert ( tmp_table, tokenKey.string, new );
                    item4 = htSearch( tmp_table, tokenKey.string );
                    op2 = item4->data->p_uni.variable;     //získání ukazatele na nově vytvořený literál 
                    
                    tHTItem *item3;
                    item3 = htSearch(TS_Array[item->data->p_uni.function->position_arr], item->data->p_uni.function->Params[n_args-1].name);  //nalezení argumentu v lok. tabulce symbolů
                      
                    if( item3 != NULL )
                      insertTab( instr_table, akt_label, I_ASSIGN, item3->data->p_uni.variable, op2, NULL, NULL);
                    
                    if( item->data->p_uni.function->returnType == variable_void ) 
                      insertTab( instr_table, akt_label, I_FCEVJUMP, NULL, NULL, NULL, tmp_string);   //void funkce
                    else
                    {
                      insertTab( instr_table, akt_label, I_FCEJUMP, NULL, NULL, NULL, tmp_string);   //non.void funkce
                      
                      if( tokenResult.type == tt_identifier )
                      {
                        tmp_string2 = add_full_ID( tokenResult.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem ID:V
                        if( (item = htSearch(TS_Array[position_in_arr - 1], tokenResult.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                        {
                          if( (item = htSearch(global_table, tmp_string2)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                          {
                            chyba = ERR_SEM1;   //nedeklarovana promenna
                            fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                          }
                        }
                        if( chyba == ERR_NONE )
                        {
                          op1 = item->data->p_uni.variable;   //L-operand bude ID
                        }
                        item = htSearch(tmp_table, tmp_string);
                        value_return = item->data->p_uni.variable;  //získání ukazatele na návratovou hodnotu
                        
                        insertTab( instr_table, akt_label, I_ASSIGN, op1, value_return, NULL, NULL);
                        
                        free(tmp_string2);
                        value_return = NULL;
                      }
                      
                      else if( tokenResult.type == tt_full_id )
                      {
                        if( (item = htSearch(global_table, tokenResult.string)) == NULL )  //muze se jednat o full_id
                        {
                          chyba = ERR_SEM1;   //nedeklarovana promenna
                          fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                        }
                        if( chyba == ERR_NONE )
                            op1 = item->data->p_uni.variable;  //L-operand bude full.ID
                        item = htSearch(tmp_table, tmp_string);
                        value_return = item->data->p_uni.variable;   //získání ukazatele na návratovou hodnotu
                        
                        insertTab( instr_table, akt_label, I_ASSIGN, op1, value_return, NULL, NULL);
                        value_return = NULL;
                      }
                      clearToken(&tokenResult);
                    }
                  }
                  clearToken(&tokenKey);
                }
              }
              free(tmp_string);
            }
            else if( tokenParam.type == tt_full_id )
            {
              if( (item = htSearch(global_table, tokenParam.string) ) == NULL )  //funkce nenalezena v glob. tabulce symbolu
              {
                chyba = ERR_SEM1;   //nedefinovana funkce
                fprintf(stderr, "Chyba. Řádek %d : nedefinovaná funkce.\n", token.line);
              }
              
              else if( item->data->p_uni.function->n_params != n_args )
              {
                chyba = ERR_SEM2;   //spatny pocet argumentu teto funkci
                fprintf(stderr, "Chyba. Řádek %d : Chybný počet argumentů!\n", token.line);
              }
              
              else if( tokenTerm.type != tt_empty )
              {
                tHTItem* item2;
                char *tmp_string2;
                chyba = check_argument_type( item->data, tokenTerm, (n_args-1) );
                if( chyba != ERR_NONE )
                {
                  chyba = ERR_SEM2;   //chybny typ argumentu
                  fprintf(stderr, "Chyba. Řádek %d : Chybný typ argumentu funkce!\n", token.line);
                }
                
                else if( tokenTerm.type == tt_identifier )  //jedna se o jednoduchy identifikator v argumentu
                {
                  tmp_string2 = add_full_ID( tokenTerm.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem full.ID
                  if( (item2 = htSearch(TS_Array[position_in_arr - 1], tokenTerm.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                  {
                    if( (item2 = htSearch(global_table, tmp_string2)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                    {
                      chyba = ERR_SEM1;   //nedeklarovana promenna
                      fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                    }
                    else if( (item->data->p_uni.function->Params[n_args-1].param_type) != (item2->data->p_uni.variable->var_type) )
                    {
                      chyba = ERR_SEM2;   //chybny typ argumentu
                      fprintf(stderr, "Chyba. Řádek %d : Chybný typ argumentu funkce!\n", token.line);
                    }
                  }
                  else if( (item->data->p_uni.function->Params[n_args-1].param_type) != (item2->data->p_uni.variable->var_type) )
                  {
                    chyba = ERR_SEM2;   //chybny typ argumentu
                    fprintf(stderr, "Chyba. Řádek %d : Chybný typ argumentu funkce!\n", token.line);
                  }
                  free(tmp_string2);
                  
                  if( chyba == ERR_NONE )
                  {
                    tHTItem *item3;
                    item3 = htSearch(TS_Array[item->data->p_uni.function->position_arr], item->data->p_uni.function->Params[n_args-1].name);  //nalezení argumentu v lok. TS
                    
                    if( item3 != NULL )
                      insertTab( instr_table, akt_label, I_ASSIGN, item3->data->p_uni.variable, item2->data->p_uni.variable, NULL, NULL);
                    
                    if( item->data->p_uni.function->returnType == variable_void ) 
                      insertTab( instr_table, akt_label, I_FCEVJUMP, NULL, NULL, NULL, tokenParam.string);  //void funkce
                    else
                    {
                      insertTab( instr_table, akt_label, I_FCEJUMP, NULL, NULL, NULL, tokenParam.string);  //non-void funkce
                      
                      if( tokenResult.type == tt_identifier )
                      {
                        tmp_string2 = add_full_ID( tokenResult.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem full.ID
                        if( (item = htSearch(TS_Array[position_in_arr - 1], tokenResult.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                        {
                          if( (item = htSearch(global_table, tmp_string2)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                          {
                            chyba = ERR_SEM1;   //nedeklarovana promenna
                            fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                          }
                        }
                        if( chyba == ERR_NONE )
                        {
                          op1 = item->data->p_uni.variable;  //L-operand bude ID
                        }
                        item = htSearch(tmp_table, tokenParam.string);
                        value_return = item->data->p_uni.variable;  //získání ukazatele do pomocné TS na návratovou hodnotu
                        
                        insertTab( instr_table, akt_label, I_ASSIGN, op1, value_return, NULL, NULL);
                        
                        free(tmp_string2);
                        value_return = NULL;
                      }
                      
                      else if( tokenResult.type == tt_full_id )
                      {
                        if( (item = htSearch(global_table, tokenResult.string)) == NULL )  //muze se jednat o full_id
                        {
                          chyba = ERR_SEM1;   //nedeklarovana promenna
                          fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                        }
                        if( chyba == ERR_NONE )
                            op1 = item->data->p_uni.variable;  //L-operand bude full.ID
                        
                        item = htSearch(tmp_table, tokenParam.string);
                        value_return = item->data->p_uni.variable;  //získání do pomocné TS ukazatele na návratovou hodnotu
                        
                        insertTab( instr_table, akt_label, I_ASSIGN, op1, value_return, NULL, NULL);
                        value_return = NULL;
                      }
                      clearToken(&tokenResult);
                    }
                  }
                }
                
                else if( tokenTerm.type == tt_full_id )   //jedna se o full identifikator v argumentu
                {
                  if( (item2 = htSearch(global_table, tokenTerm.string)) == NULL )
                  {
                    chyba = ERR_SEM1;   //nedeklarovana promenna
                    fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                  }
                  else if( (item->data->p_uni.function->Params[n_args-1].param_type) != (item2->data->p_uni.variable->var_type) )
                  {
                    chyba = ERR_SEM2;   //chybny typ argumentu
                    fprintf(stderr, "Chyba. Řádek %d : Chybný typ argumentu funkce!\n", token.line);
                  }
                  
                  if( chyba == ERR_NONE )
                  {
                    tHTItem *item3;
                    item3 = htSearch(TS_Array[item->data->p_uni.function->position_arr], item->data->p_uni.function->Params[n_args-1].name);  //nalezení argumentu v lok. TS
                    
                    if( item3 != NULL )
                      insertTab( instr_table, akt_label, I_ASSIGN, item3->data->p_uni.variable, item2->data->p_uni.variable, NULL, NULL);
                    
                    if( item->data->p_uni.function->returnType == variable_void ) 
                      insertTab( instr_table, akt_label, I_FCEVJUMP, NULL, NULL, NULL, tokenParam.string);  //void funkce
                    else
                    {
                      insertTab( instr_table, akt_label, I_FCEJUMP, NULL, NULL, NULL, tokenParam.string);  //non-void funkce
                      
                      if( tokenResult.type == tt_identifier )
                      {
                        tmp_string2 = add_full_ID( tokenResult.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem full.ID
                        if( (item = htSearch(TS_Array[position_in_arr - 1], tokenResult.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                        {
                          if( (item = htSearch(global_table, tmp_string2)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                          {
                            chyba = ERR_SEM1;   //nedeklarovana promenna
                            fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                          }
                        }
                        if( chyba == ERR_NONE )
                        {
                          op1 = item->data->p_uni.variable;   //L-operand bude ID
                        }
                        item = htSearch(tmp_table, tokenParam.string);
                        value_return = item->data->p_uni.variable;  //získání ukazatele do pomocné TS na návratovou hodnotu
                        
                        insertTab( instr_table, akt_label, I_ASSIGN, op1, value_return, NULL, NULL);
                        
                        free(tmp_string2);
                        value_return = NULL;
                      }
                      
                      else if( tokenResult.type == tt_full_id )
                      {
                        if( (item = htSearch(global_table, tokenResult.string)) == NULL )  //muze se jednat o full_id
                        {
                          chyba = ERR_SEM1;   //nedeklarovana promenna
                          fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                        }
                        if( chyba == ERR_NONE )
                            op1 = item->data->p_uni.variable;   //L-operand bude full.ID
                        
                        item = htSearch(tmp_table, tokenParam.string);
                        value_return = item->data->p_uni.variable;    //získání ukazatele do pomocné TS na návratovou hodnotu
                        
                        insertTab( instr_table, akt_label, I_ASSIGN, op1, value_return, NULL, NULL);
                        value_return = NULL;
                      }
                      clearToken(&tokenResult);
                    }
                  }
                }
                else if( tokenTerm.type == tt_int || tokenTerm.type == tt_double || tokenTerm.type == tt_literal ) //literal v argumentu
                {
                  Class *new = Create_Const_to_Table(&tokenTerm);
                        
                  Token tokenKey;
                  initToken(&tokenKey);
                  Create_Token_string( &tokenKey, "const" );
                        
                  if( Err == ERR_NONE )
                  {
                    tHTItem *item4;
                    htInsert ( tmp_table, tokenKey.string, new );
                    item4 = htSearch( tmp_table, tokenKey.string );
                    op2 = item4->data->p_uni.variable;   //získání ukazatele na literál do pomocné TS
                          
                    tHTItem *item3;
                    item3 = htSearch(TS_Array[item->data->p_uni.function->position_arr], item->data->p_uni.function->Params[n_args-1].name);  //nalezení argumentu v lok. TS
                          
                    if( item3 != NULL )
                      insertTab( instr_table, akt_label, I_ASSIGN, item3->data->p_uni.variable, op2, NULL, NULL);
                    
                    if( item->data->p_uni.function->returnType == variable_void ) 
                      insertTab( instr_table, akt_label, I_FCEVJUMP, NULL, NULL, NULL, tokenParam.string);   //void funkce
                    else
                    {
                      insertTab( instr_table, akt_label, I_FCEJUMP, NULL, NULL, NULL, tokenParam.string);  //non.void funkce
                      
                      if( tokenResult.type == tt_identifier )
                      {
                        tmp_string2 = add_full_ID( tokenResult.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem ID:V
                        if( (item = htSearch(TS_Array[position_in_arr - 1], tokenResult.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                        {
                          if( (item = htSearch(global_table, tmp_string2)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                          {
                            chyba = ERR_SEM1;   //nedeklarovana promenna
                            fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                          }
                        }
                        if( chyba == ERR_NONE )
                        {
                          op1 = item->data->p_uni.variable;    //L-operand bude ID
                        }
                        item = htSearch(tmp_table, tokenParam.string);
                        value_return = item->data->p_uni.variable;  //získání ukazatele do pomocné TS na návratovou hodnotu
                        
                        insertTab( instr_table, akt_label, I_ASSIGN, op1, value_return, NULL, NULL);
                        
                        free(tmp_string2);
                        value_return = NULL;
                      }
                      
                      else if( tokenResult.type == tt_full_id )
                      {
                        if( (item = htSearch(global_table, tokenResult.string)) == NULL )  //muze se jednat o full_id
                        {
                          chyba = ERR_SEM1;   //nedeklarovana promenna
                          fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                        }
                        if( chyba == ERR_NONE )
                            op1 = item->data->p_uni.variable;  //L-operand bude full.ID
                        
                        item = htSearch(tmp_table, tokenParam.string);
                        value_return = item->data->p_uni.variable;    //získání ukazatele do pomocné TS na návratovou hodnotu
                        
                        insertTab( instr_table, akt_label, I_ASSIGN, op1, value_return, NULL, NULL);
                        value_return = NULL;
                      }
                      clearToken(&tokenResult);
                    }
                  }
                  clearToken(&tokenKey);
                }    
              }
            }
          }
        }
        clearToken(&tokenParam);
        clearToken(&tokenTerm);
        //end semant
        
        n_args = 0;  //obnovime nasi promennou pro pocet argumentu na poc. hodnotu
      }
      else if(token.type == tt_comma) // <arg2> -> , term <arg> {,}
      {
        stackPop(&s);
        stackPush(&s, S_ARG2);
        stackPush(&s, S_TERM);
        stackPush(&s, S_COMMA);
        
        //semant
        if( ROUND == 2 )
        {
          n_args++;  //inkrementujeme pocet argumentu teto funkce
          
          if( strcmp(tokenParam.string, "ifj16.find") == 0 || strcmp(tokenParam.string, "ifj16.compare") == 0 
            || strcmp(tokenParam.string, "ifj16.substr") == 0 )
          {
            if( strcmp(tokenParam.string, "ifj16.substr") == 0 )  
            {
              if( substr == 0 )   //jde o prvni parametr funkce ifj16.substr
              {
                tHTItem *item;  
                if( tokenTerm.type == tt_int || tokenTerm.type == tt_double || tokenTerm.type == tt_literal )
                {
                  Class *new = Create_Const_to_Table(&tokenTerm);
                  
                  Token tokenKey;
                  initToken(&tokenKey);
                  Create_Token_string( &tokenKey, "param" );
                        
                  if( Err == ERR_NONE )
                  {
                    htInsert ( tmp_table, tokenKey.string, new );
                    item = htSearch( tmp_table, tokenKey.string );
                    op1 = item->data->p_uni.variable;             //ulozeni ukazatele do pomocne TS na literal
                  }
                  clearToken(&tokenKey);
                  substr = 1;       //zvýšení proměnné, která indikuje stav parametrů ifj16.substr
                }
                
                else if( tokenTerm.type == tt_identifier )
                {
                  char *tmp_string;
                  tmp_string = add_full_ID( tokenTerm.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem ID:V
                  if( (item = htSearch(TS_Array[position_in_arr - 1], tokenTerm.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                  {
                    if( (item = htSearch(global_table, tmp_string)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                    {
                      chyba = ERR_SEM1;   //nedeklarovana promenna
                      fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                    }
                  }
                  if( chyba == ERR_NONE )
                  {
                    op1 = item->data->p_uni.variable;   //ulozeni ukazatele do pomocne TS na ID
                  }
                  free(tmp_string);
                  substr = 1;         //zvýšení proměnné, která indikuje stav parametrů ifj16.substr
                }
                
                else if( tokenTerm.type == tt_full_id )
                {
                  if( (item = htSearch(global_table, tokenTerm.string)) == NULL )  //muze se jednat o full_id
                  {
                    chyba = ERR_SEM1;   //nedeklarovana promenna
                    fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                  }
                  if( chyba == ERR_NONE )
                      op1 = item->data->p_uni.variable;  //ulozeni ukazatele do pomocne TS na full.ID

                  substr = 1;     //zvýšení proměnné, která indikuje stav parametrů ifj16.substr
                }
              }
              else if( substr == 1 )  //jedna se o druhy parametr funkce ifj16.substr
              {
                tHTItem *item;  
                if( tokenTerm.type == tt_int || tokenTerm.type == tt_double || tokenTerm.type == tt_literal )
                {
                  Class *new = Create_Const_to_Table(&tokenTerm);
                  
                  Token tokenKey;
                  initToken(&tokenKey);
                  Create_Token_string( &tokenKey, "param" );
                        
                  if( Err == ERR_NONE )
                  {
                    htInsert ( tmp_table, tokenKey.string, new );
                    item = htSearch( tmp_table, tokenKey.string );
                    op2 = item->data->p_uni.variable;    //ulozeni ukazatele do pomocne TS na literal
                  }
                  clearToken(&tokenKey);
                  substr = 2;            //zvýšení proměnné, která indikuje stav parametrů ifj16.substr
                }
                
                else if( tokenTerm.type == tt_identifier )
                {
                  char *tmp_string;
                  tmp_string = add_full_ID( tokenTerm.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem ID:V
                  if( (item = htSearch(TS_Array[position_in_arr - 1], tokenTerm.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                  {
                    if( (item = htSearch(global_table, tmp_string)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                    {
                      chyba = ERR_SEM1;   //nedeklarovana promenna
                      fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                    }
                  }
                  if( chyba == ERR_NONE )
                  {
                    op2 = item->data->p_uni.variable;  //ulozeni ukazatele do pomocne TS na ID
                  }
                  free(tmp_string);
                  substr = 2;     //zvýšení proměnné, která indikuje stav parametrů ifj16.substr
                }
                
                else if( tokenTerm.type == tt_full_id )
                {
                  if( (item = htSearch(global_table, tokenTerm.string)) == NULL )  //muze se jednat o full_id
                  {
                    chyba = ERR_SEM1;   //nedeklarovana promenna
                    fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                  }
                  if( chyba == ERR_NONE )
                      op2 = item->data->p_uni.variable;   //ulozeni ukazatele do pomocne TS na full.ID

                  substr = 2;         //zvýšení proměnné, která indikuje stav parametrů ifj16.substr
                }
                
                if( substr == 2 )   //byly nalezeny již dva parametry, generování první instrukce pro substr
                {
                  Class *new = Create_Var_to_Table();
                  Token tokenKey;
                  initToken(&tokenKey);
                  Create_Token_string( &tokenKey, "opTmp" );
                        
                  if( Err == ERR_NONE )
                  {
                    htInsert ( tmp_table, tokenKey.string, new );
                    item = htSearch( tmp_table, tokenKey.string );
                    opTmp = item->data->p_uni.variable;   //uložení ukazatele do pomocné TS na novou proměnnou
                  }
                  clearToken(&tokenKey);
                  
                  char *akt_label = NULL;
                  int TypeFun;
                  topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
                  insertTab( instr_table, akt_label, I_SUBSTR1, op1, op2, opTmp, NULL);
                  
                  substr = 3;    //zvýšení proměnné, která indikuje stav parametrů ifj16.substr
                }
              }
            }
            else  //jedná se o funkce find, compare
            {
              tHTItem *item;  
              if( tokenTerm.type == tt_int || tokenTerm.type == tt_double || tokenTerm.type == tt_literal )
              {
                Class *new = Create_Const_to_Table(&tokenTerm);
                
                Token tokenKey;
                initToken(&tokenKey);
                Create_Token_string( &tokenKey, "param" );
                      
                if( Err == ERR_NONE )
                {
                  htInsert ( tmp_table, tokenKey.string, new );
                  item = htSearch( tmp_table, tokenKey.string );
                  op1 = item->data->p_uni.variable;    //uložení ukazatele do pomocné TS na první parametr
                }
                clearToken(&tokenKey);
              }
              
              else if( tokenTerm.type == tt_identifier )
              {
                char *tmp_string;
                tmp_string = add_full_ID( tokenTerm.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem ID:V
                if( (item = htSearch(TS_Array[position_in_arr - 1], tokenTerm.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                {
                  if( (item = htSearch(global_table, tmp_string)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                  {
                    chyba = ERR_SEM1;   //nedeklarovana promenna
                    fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                  }
                }
                if( chyba == ERR_NONE )
                {
                  op1 = item->data->p_uni.variable;  //uložení ukazatele do pomocné TS na první parametr
                }
                free(tmp_string);
              }
              
              else if( tokenTerm.type == tt_full_id )
              {
                if( (item = htSearch(global_table, tokenTerm.string)) == NULL )  //muze se jednat o full_id
                {
                  chyba = ERR_SEM1;   //nedeklarovana promenna
                  fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                }
                if( chyba == ERR_NONE )
                    op1 = item->data->p_uni.variable;  //uložení ukazatele do pomocné TS na první parametr
              }
            }
          }
          else if( strcmp(tokenParam.string, "ifj16.readString") == 0 || strcmp(tokenParam.string, "ifj16.readDouble") == 0 
            || strcmp(tokenParam.string, "ifj16.readInt") == 0 || strcmp(tokenParam.string, "ifj16.lenght") == 0
            || strcmp(tokenParam.string, "ifj16.print") == 0 || strcmp(tokenParam.string, "ifj16.sort") == 0 )
          {
            chyba = ERR_SEM2;   //spatny pocet argumentu
            fprintf(stderr, "Chyba. Řádek %d : Chybný počet argumentů!\n", token.line);
          }
          else   //uložení parametru uživatlských funkcí
          {
            tHTItem *item;
            
            char *akt_label = NULL;
            int TypeFun;
            topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
            
            if( tokenParam.type == tt_identifier )  //funkce je volána v rámci třídy pomocí ID
            {
              char *tmp_string;
              tmp_string = add_full_ID( tokenParam.string, AKT_TRIDA->class_name );  //jedna se u funkci-> klic Class.ID:F
              if( (item = htSearch(global_table, tmp_string) ) == NULL )  //funkce nenalezena v glob. tabulce symbolu
              {
                chyba = ERR_SEM1;   //nedefinovana funkce
                fprintf(stderr, "Chyba. Řádek %d : nedefinovaná funkce.\n", token.line);
              }
              else if( tokenTerm.type != tt_empty )
              {
                tHTItem* item2;
                char *tmp_string2;
                chyba = check_argument_type( item->data, tokenTerm, (n_args-1) );
                if( chyba != ERR_NONE )
                {
                  chyba = ERR_SEM2;   //chybny typ argumentu
                  fprintf(stderr, "Chyba. Řádek %d : Chybný typ argumentu funkce!\n", token.line);
                }
                
                else if( tokenTerm.type == tt_identifier )  //jedna se o jednoduchy identifikator v argumentu
                {
                  tmp_string2 = add_full_ID( tokenTerm.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem full.ID
                  if( (item2 = htSearch(TS_Array[position_in_arr - 1], tokenTerm.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                  {
                    if( (item2 = htSearch(global_table, tmp_string2)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                    {
                      chyba = ERR_SEM1;   //nedeklarovana promenna
                      fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                    }
                    else if( (item->data->p_uni.function->Params[n_args-1].param_type) != (item2->data->p_uni.variable->var_type) )
                    {
                      chyba = ERR_SEM2;   //chybny typ argumentu
                      fprintf(stderr, "Chyba. Řádek %d : Chybný typ argumentu funkce!\n", token.line);
                    }
                  }
                  else if( (item->data->p_uni.function->Params[n_args-1].param_type) != (item2->data->p_uni.variable->var_type) )
                  {
                    chyba = ERR_SEM2;   //chybny typ argumentu
                    fprintf(stderr, "Chyba. Řádek %d : Chybný typ argumentu funkce!\n", token.line);
                  }
                  free(tmp_string2);
                  
                  if( chyba == ERR_NONE )
                  {
                    tHTItem *item3; 
                    item3 = htSearch(TS_Array[item->data->p_uni.function->position_arr], item->data->p_uni.function->Params[n_args-1].name);
                    
                    if( item3 != NULL )
                      insertTab( instr_table, akt_label, I_ASSIGN, item3->data->p_uni.variable, item2->data->p_uni.variable, NULL, NULL);
                  }
                }
                
                else if( tokenTerm.type == tt_full_id )   //jedna se o full identifikator v argumentu
                {
                  if( (item2 = htSearch(global_table, tokenTerm.string)) == NULL )  //muze se jednat o full_id
                  {
                    chyba = ERR_SEM1;   //nedeklarovana promenna
                    fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                  }
                  else if( (item->data->p_uni.function->Params[n_args-1].param_type) != (item2->data->p_uni.variable->var_type) )
                  {
                    chyba = ERR_SEM2;   //chybny typ argumentu
                    fprintf(stderr, "Chyba. Řádek %d : Chybný typ argumentu funkce!\n", token.line);
                  }
                  
                  if( chyba == ERR_NONE )
                  {
                    tHTItem *item3;
                    item3 = htSearch(TS_Array[item->data->p_uni.function->position_arr], item->data->p_uni.function->Params[n_args-1].name);
                    
                    if( item3 != NULL )
                      insertTab( instr_table, akt_label, I_ASSIGN, item3->data->p_uni.variable, item2->data->p_uni.variable, NULL, NULL);
                  }
                }
                
                else if( tokenTerm.type == tt_int || tokenTerm.type == tt_double || tokenTerm.type == tt_literal )
                {
                  Class *new = Create_Const_to_Table(&tokenTerm);
                  
                  Token tokenKey;
                  initToken(&tokenKey);
                  Create_Token_string( &tokenKey, "const" );
                  
                  if( Err == ERR_NONE )
                  {
                    tHTItem *item4;
                    htInsert ( tmp_table, tokenKey.string, new );
                    item4 = htSearch( tmp_table, tokenKey.string );
                    op2 = item4->data->p_uni.variable;
                    
                    tHTItem *item3;
                    item3 = htSearch(TS_Array[item->data->p_uni.function->position_arr], item->data->p_uni.function->Params[n_args-1].name);
                    
                    if( item3 != NULL )
                      insertTab( instr_table, akt_label, I_ASSIGN, item3->data->p_uni.variable, op2, NULL, NULL);
                  }
                  clearToken(&tokenKey);
                }
              }
              free(tmp_string);
            }
            else if( tokenParam.type == tt_full_id )  //funkce je volána pomocí full.ID
            {
              if( (item = htSearch(global_table, tokenParam.string) ) == NULL )  //funkce nenalezena v glob. tabulce symbolu
              {
                chyba = ERR_SEM1;   //nedefinovana funkce
                fprintf(stderr, "Chyba. Řádek %d : nedefinovaná funkce.\n", token.line);
              }
              else if( tokenTerm.type != tt_empty )
              {
                tHTItem* item2;
                char *tmp_string2;
                chyba = check_argument_type( item->data, tokenTerm, (n_args-1) );
                if( chyba != ERR_NONE )
                {
                  chyba = ERR_SEM2;   //chybny typ argumentu
                  fprintf(stderr, "Chyba. Řádek %d : Chybný typ argumentu funkce!\n", token.line);
                }
                
                else if( tokenTerm.type == tt_identifier )  //jedna se o jednoduchy identifikator v argumentu
                {
                  tmp_string2 = add_full_ID( tokenTerm.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem ID:V
                  if( (item2 = htSearch(TS_Array[position_in_arr - 1], tokenTerm.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                  {
                    if( (item2 = htSearch(global_table, tmp_string2)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                    {
                      chyba = ERR_SEM1;   //nedeklarovana promenna
                      fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                    }
                    else if( (item->data->p_uni.function->Params[n_args-1].param_type) != (item2->data->p_uni.variable->var_type) )
                    {
                      chyba = ERR_SEM2;   //chybny typ argumentu
                      fprintf(stderr, "Chyba. Řádek %d : Chybny typ argumentu funkce!\n", token.line);
                    }
                  }
                  else if( (item->data->p_uni.function->Params[n_args-1].param_type) != (item2->data->p_uni.variable->var_type) )
                  {
                    chyba = ERR_SEM2;   //chybny typ argumentu
                    fprintf(stderr, "TADYChyba. Řádek %d : Chybny typ argumentu funkce!\n", token.line);
                  }
                  free(tmp_string2);
                  
                  if( chyba == ERR_NONE )
                  {
                    tHTItem *item3;
                    item3 = htSearch(TS_Array[item->data->p_uni.function->position_arr], item->data->p_uni.function->Params[n_args-1].name);
                    
                    if( item3 != NULL )
                      insertTab( instr_table, akt_label, I_ASSIGN, item3->data->p_uni.variable, item2->data->p_uni.variable, NULL, NULL);
                  }
                }
                
                else if( tokenTerm.type == tt_full_id )   //jedna se o full identifikator v argumentu
                {
                  if( (item2 = htSearch(global_table, tokenTerm.string)) == NULL )  //muze se jednat o full_id
                  {
                    chyba = ERR_SEM1;   //nedeklarovana promenna
                    fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                  }
                  else if( (item->data->p_uni.function->Params[n_args-1].param_type) != (item2->data->p_uni.variable->var_type) )
                  {
                    chyba = ERR_SEM2;   //chybny typ argumentu
                    fprintf(stderr, "Chyba. Řádek %d : Chybný typ argumentu funkce!\n", token.line);
                  }
                  
                  if( chyba == ERR_NONE )
                  {
                    tHTItem *item3;
                    item3 = htSearch(TS_Array[item->data->p_uni.function->position_arr], item->data->p_uni.function->Params[n_args-1].name);
                    
                    if( item3 != NULL )
                      insertTab( instr_table, akt_label, I_ASSIGN, item3->data->p_uni.variable, item2->data->p_uni.variable, NULL, NULL);
                  }
                }
                
                else if( tokenTerm.type == tt_int || tokenTerm.type == tt_double || tokenTerm.type == tt_literal )
                {
                  Class *new = Create_Const_to_Table(&tokenTerm);   //jedna se o literal v argumentu
                  
                  Token tokenKey;
                  initToken(&tokenKey);
                  Create_Token_string( &tokenKey, "const" );
                  
                  if( Err == ERR_NONE )
                  {
                    tHTItem *item4;
                    htInsert ( tmp_table, tokenKey.string, new );
                    item4 = htSearch( tmp_table, tokenKey.string );
                    op2 = item4->data->p_uni.variable;
                    
                    tHTItem *item3;
                    item3 = htSearch(TS_Array[item->data->p_uni.function->position_arr], item->data->p_uni.function->Params[n_args-1].name);
                    
                    if( item3 != NULL )
                      insertTab( instr_table, akt_label, I_ASSIGN, item3->data->p_uni.variable, op2, NULL, NULL);
                  }
                  clearToken(&tokenKey);
                }
              }
            }
          }
        }
        clearToken(&tokenTerm);
        //end semant
      }
      else if(token.type == tt_plus) // <arg2> -> + term <arg> {+}
      {
        stackPop(&s);
        stackPush(&s, S_ARG2);
        stackPush(&s, S_TERM);
        stackPush(&s, S_PLUS);
        
        //semant
        if( ROUND == 2 )
        {
          if( strcmp(tokenParam.string, "ifj16.print") == 0 )   //konkatenace u funkce ifj16.print
          {
            tHTItem *item;  
            if( tokenTerm.type == tt_int || tokenTerm.type == tt_double || tokenTerm.type == tt_literal )
            {
              Class *new = Create_Const_to_Table(&tokenTerm);
                
              Token tokenKey;
              initToken(&tokenKey);
              Create_Token_string( &tokenKey, "arg" );
                     
              if( Err == ERR_NONE )
              {
                htInsert ( tmp_table, tokenKey.string, new );
                item = htSearch( tmp_table, tokenKey.string );
                op1 = item->data->p_uni.variable;          //získání ukazatele na literál do pomocné TS
              }
              clearToken(&tokenKey);
            }
              
            else if( tokenTerm.type == tt_identifier )
            {
              char *tmp_string;
              tmp_string = add_full_ID( tokenTerm.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem ID:V
              if( (item = htSearch(TS_Array[position_in_arr - 1], tokenTerm.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
              {
                if( (item = htSearch(global_table, tmp_string)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                {
                  chyba = ERR_SEM1;   //nedeklarovana promenna
                  fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                }
              }
              if( chyba == ERR_NONE )
              {
                op1 = item->data->p_uni.variable;   //získání ukazatele na ID
              }
              free(tmp_string);
            }
              
            else if( tokenTerm.type == tt_full_id )
            {
              if( (item = htSearch(global_table, tokenTerm.string)) == NULL )  //muze se jednat o full_id
              {
                chyba = ERR_SEM1;   //nedeklarovana promenna
                fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
              }
              if( chyba == ERR_NONE )
                op1 = item->data->p_uni.variable;   //získání ukazatele na full.ID
            }          
              
            char *akt_label = NULL;
            int TypeFun;
            topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );  //aktuální label
              
            if( chyba == ERR_NONE )
              insertTab( instr_table, akt_label, I_PRINT, op1, NULL, NULL, NULL);   //provedení instrukce pro funkci ifj16.print
          }
          else
          {
            chyba = ERR_SEM3;
            fprintf(stderr, "Chyba. Řádek %d : Konkatenace je provedena pouze u ifj.print!\n", token.line);
          }
        }
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekávána +, ) nebo ,.\n", token.line);
      }
      inExp = 0;
    }
    else if(top == S_NAVRAT)
    {
      if(token.keyword == key_return) // <navrat> -> return <navrat2> {return}
      {
        stackPop(&s);
        stackPush(&s, S_NAVRAT2);
        stackPush(&s, S_RETURN);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván return.\n", token.line);
      }
      inExp = 0;
    }
    else if(top == S_NAVRAT2)
    {
       if(token.type == tt_semicolon) // <navrat2> -> ; {;}
      {
        stackPop(&s);
        stackPush(&s, S_SEMICOLON);
        
        //semant
        if( ROUND == 2 )
        {
          char *akt_label = NULL;
          int TypeFun;
          topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );  //aktuální label
          
          tHTItem *item;
          Class *new = Create_Var_to_Table();
          Token tokenKey;
          initToken(&tokenKey);
          Create_Token_string( &tokenKey, "FunRes" );
                        
          if( Err == ERR_NONE )
          {
            htInsert ( tmp_table, tokenKey.string, new );
            item = htSearch( tmp_table, tokenKey.string );
            result = item->data->p_uni.variable;   //získání ukazatele do pomocné TS, na nově vytvořenou proměnnou
          }
          clearToken(&tokenKey);
          result->inicialized = 0;  //nastavení inicializace na 0, (return;)
          
          //generování instrukce return, správně pro void, chyba pro non-void
          insertTab( instr_table, akt_label, I_RETURN, result, NULL, NULL, NULL);
          clearToken(&tokenReturn);
          
          result = NULL;
        }
        //end semant
      }
      else if(token.type == tt_identifier || token.type == tt_full_id || token.type == tt_literal || token.type == tt_int || token.type == tt_double || token.type == tt_bracket_left) // <navrat2> -> <hodnota> ; {ID, FULL_ID, num, literal, (}
      {
        stackPop(&s);
        stackPush(&s, S_SEMICOLON);
        stackPush(&s, S_HODNOTA);
      } 
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván ;, identifikátor, číslo, literál.\n", token.line);
      }
      inExp = 0;
    }
    else if(top == S_VYRAZ)
    {
      if(token.type == tt_bracket_left) // vyraz> -> ( <vyraz> <vyraz3> {(}
      {
        stackPop(&s);
        stackPush(&s, S_VYRAZ3);
        stackPush(&s, S_VYRAZ);
        stackPush(&s, S_BRACKET_LEFT);
        
        //semant
        if( ROUND == 2 )  
          OPERACE = true;   //vyhodnocování bude provedeno v parserExp
        
        tokenQueUp(&q, token);
      }
      else if(token.type == tt_identifier || token.type == tt_full_id || token.type == tt_literal ||
       token.type == tt_int || token.type == tt_double) // <vyraz> -> term <vyraz2> {id, full_id, num, literal}
      {
        stackPop(&s);
        stackPush(&s, S_VYRAZ2);
        stackPush(&s, S_TERM);
      
        tokenQueUp(&q, token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván true, false, identifikátor, číslo, literál.\n", token.line);
      }
      inExp = 1;
    }
    else if(top == S_VYRAZ3) // <vyraz3> -> ) <vyraz2> {)}
    {
      if(token.type == tt_bracket_right)
      {
        stackPop(&s);
        stackPush(&s, S_VYRAZ2);
        stackPush(&s, S_BRACKET_RIGHT);
        
        //semant
        if( ROUND == 2 )
          OPERACE = true;   //vyhodnocování bude provedeno v parserExp

        tokenQueUp(&q, token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekávána ).\n", token.line);
      }
      inExp = 1;
    }
    else if(top == S_VYRAZ2)
    {
      if(token.type == tt_mul || token.type == tt_div || token.type == tt_plus || token.type == tt_minus ||
       token.type == tt_less || token.type == tt_greater || token.type == tt_lequal ||
       token.type == tt_gequal || token.type == tt_same || token.type == tt_not_equal) // <vyraz2> -> <operation> <vyraz>  {+,-,*,/,==,!=,<,>,<=,>=}
      {
        stackPop(&s);
        stackPush(&s, S_VYRAZ);
        stackPush(&s, S_OPERATION);
        
        //semant
        if( ROUND == 2 )
          OPERACE = true;   //vyhodnocování bude provedeno v parserExp

        tokenQueUp(&q, token);
      }
      else if(token.type == tt_bracket_right || token.type == tt_semicolon) // <vyraz2> -> eps {), ;}
      {
        stackPop(&s);
        
        //semant
        if( token.type == tt_semicolon && OPERACE == false)   //zde získáme hodnotu, v parserExp už se jednoduché přiřazení řešit nebude
        {
          if( ROUND == 2 )
          {
            tHTItem* item;
            tHTItem* item2;
            char *tmp_string;
            
            if( tokenReturn.keyword == key_return )  //řešení v případě konstanty u returnu
            {
              if( tokenFun.type == tt_identifier )   //stále probíhá definice funkce, její název je uložen v tokenFun
              {
                char *akt_label = NULL;
                int TypeFun;
                topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL ); //aktuální label
                
                if( TypeFun == 0 )  //jedná se o void funkci, kde výraz nepřipadá v úvahu
                {
                  chyba = ERR_SEM2;
                  fprintf(stderr, "Chyba. Řádek %d : Očekáván return;\n", token.line);
                }
                    
                tmp_string = add_full_ID( tokenFun.string, AKT_TRIDA->class_name );
                item = htSearch( global_table, tmp_string );
                
                if( tokenTerm.type == tt_int || tokenTerm.type == tt_double || tokenTerm.type == tt_literal )
                {
                  Class *new = Create_Const_to_Table(&tokenTerm);
                  
                  Token tokenKey;
                  initToken(&tokenKey);
                  Create_Token_string( &tokenKey, "const" );
                  
                  if( Err == ERR_NONE )
                  {
                    htInsert ( tmp_table, tokenKey.string, new );
                    item2 = htSearch( tmp_table, tokenKey.string );
                    
                    tHTItem *item3;
                    
                    if( Err == ERR_NONE )
                    {
                      if( (item3 = htSearch( tmp_table, tmp_string)) != NULL )
                        item3->data->p_uni.variable->var_type = item2->data->p_uni.variable->var_type;
                    }
                    
                    value_return = item3->data->p_uni.variable;  //získání ukazatele na návratovou hodnotu
                    
                    //instrukce pro získání hodnoty konstanty a následný return
                    insertTab( instr_table, akt_label, I_ASSIGN, value_return, item2->data->p_uni.variable, NULL, NULL);
                    insertTab( instr_table, akt_label, I_RETURN, value_return, NULL, NULL, NULL);
                  }
                  clearToken(&tokenKey);
                }
                free(tmp_string);
              }
              clearToken(&tokenReturn);
            }
            
            else if( tokenResult.type == tt_identifier )  //jedna se o jednoduchy identifikator
            {
              tmp_string = add_full_ID( tokenResult.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem full.ID
              if( (item = htSearch(TS_Array[position_in_arr - 1], tokenResult.string)) == NULL )
              {
                item = htSearch(global_table, tmp_string);
              }
              
              if( tokenTerm.type == tt_int || tokenTerm.type == tt_double || tokenTerm.type == tt_literal )
              {
                item->data->p_uni.variable->inicialized = 1;              
                op1 = item->data->p_uni.variable;
                
                Class *new = Create_Const_to_Table(&tokenTerm);
                
                Token tokenKey;
                initToken(&tokenKey);
                Create_Token_string( &tokenKey, "const" );
                
                if( Err == ERR_NONE )
                {
                  htInsert ( tmp_table, tokenKey.string, new );
                  item = htSearch( tmp_table, tokenKey.string );
                  op2 = item->data->p_uni.variable;   //uložení ukazatele do pomocné TS na literál
                }
                if( tokenStatic.keyword == key_static )  //na úrovni třídy
                {
                  insertTab( instr_table, "CLASS", I_ASSIGN, op1, op2, NULL, NULL);
                  clearToken(&tokenStatic);
                }
                else  //na úrovni funkce
                {
                  char *akt_label = NULL;
                  int TypeFun;
                  topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
                  insertTab( instr_table, akt_label, I_ASSIGN, op1, op2, NULL, NULL);
                }
                clearToken(&tokenKey);
              }
              free(tmp_string);
              clearToken(&tokenResult);
            }
            
            else if( tokenResult.type == tt_full_id )   //jedna se o full identifikator
            {
              if( (item = htSearch(global_table, tokenResult.string)) == NULL )
              {
                chyba = ERR_SEM1;   //nedeklarovana promenna
                fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
              }
              else
              {
                if( tokenTerm.type == tt_int || tokenTerm.type == tt_double || tokenTerm.type == tt_literal )
                {
                  item->data->p_uni.variable->inicialized = 1;
                  op1 = item->data->p_uni.variable;
                  
                  Class *new = Create_Const_to_Table(&tokenTerm);
                  
                  Token tokenKey;
                  initToken(&tokenKey);
                  Create_Token_string( &tokenKey, "const" );
                  
                  if( Err == ERR_NONE )
                  {
                    htInsert ( tmp_table, tokenKey.string, new );
                    item = htSearch( tmp_table, tokenKey.string );
                    op2 = item->data->p_uni.variable;   //uložení ukazatele do pomocné TS na literál
                  }
                  
                  if( tokenStatic.keyword == key_static )  //na úrovni třídy
                  {
                    insertTab( instr_table, "CLASS", I_ASSIGN, op1, op2, NULL, NULL);
                    clearToken(&tokenStatic);
                  }
                  else   //na úrovni funkce
                  {
                    char *akt_label = NULL;
                    int TypeFun;
                    topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
                    insertTab( instr_table, akt_label, I_ASSIGN, op1, op2, NULL, NULL);
                  }
                  
                  clearToken(&tokenKey);
                }
              }
              clearToken(&tokenResult); 
            } 
          }
        }

        clearToken(&tokenTerm);
        clearToken(&tokenTmp);
        //end semant
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekávána ), ; nebo operace.\n", token.line);
      }
      inExp = 1;
    }
    else if(top == S_HODNOTA)
    {
      if(token.type == tt_bracket_left || token.type == tt_int || token.type == tt_double || token.type == tt_literal) // <hodnota> -> <vyraz> {(, num, literal}
      {
        stackPop(&s);
        stackPush(&s, S_VYRAZ);
      }   
      else if(token.type == tt_identifier || token.type == tt_full_id) // <hodnota> -> ID_FULL_ID <hodnota2> {id, full_id}
      {
        stackPop(&s);
        stackPush(&s, S_HODNOTA2);
        stackPush(&s, S_ID_FULL_ID);
        // uložení tokenů na stack pro případ že bude následovat pravidlo vyraz
        tokenQueUp(&q, token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván identifikátor, (, číslo nebo literál.\n", token.line);
      }
      inExp = 1;
    }
    else if(top == S_HODNOTA2)
    {
      if(token.type == tt_mul || token.type == tt_div || token.type == tt_plus || token.type == tt_minus ||
       token.type == tt_less || token.type == tt_greater || token.type == tt_lequal || token.type == tt_gequal ||
        token.type == tt_same || token.type == tt_not_equal) // <hodnota2> -> <vyraz2> {+,-,*,/,==,!=,<,>,<=,>=}
      {
        stackPop(&s);
        stackPush(&s, S_VYRAZ2);
      }
      else if(token.type == tt_bracket_left) // <hodnota2> -> ( <arg> ) {(}
      {
        stackPop(&s);
        stackPush(&s, S_BRACKET_RIGHT);
        stackPush(&s, S_ARG);
        stackPush(&s, S_BRACKET_LEFT);
        // v případě že nenastalo pravidlo pro vyraz odstraním token ze zasobniku tokenů
        if(tokenQueEmpty(&q) != 1)
          tokenQueRemove(&q);
      }
      else if(token.type == tt_semicolon) // <hodnota2> -> eps {;}
      {
        stackPop(&s);
        // v případě že nenastalo pravidlo pro vyraz odstraním token ze zasobniku tokenů

        //semant
        if( ROUND == 2 )
        {
          tHTItem* item;
          tHTItem* item2;
          char *tmp_string;
          char *tmp_string2;
        
          if( tokenReturn.keyword == key_return )
          {
            if( tokenID.type == tt_identifier )   //jedná se o return ID;
            {
              tmp_string2 = add_full_ID( tokenID.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem full.ID
              if( (item2 = htSearch(TS_Array[position_in_arr - 1], tokenID.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
              {
                if( (item2 = htSearch(global_table, tmp_string2)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                {
                  chyba = ERR_SEM1;   //nedeklarovana promenna
                  fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                }
              }
              if( chyba == ERR_NONE )
              {
                char *akt_label = NULL;
                int TypeFun;
                topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
                
                if( TypeFun == 0 )   //jedná se o void funkci
                {
                  chyba = ERR_SEM2;
                  fprintf(stderr, "Chyba. Řádek %d : Očekáván return;\n", token.line);
                }
                
                tmp_string = add_full_ID( tokenFun.string, AKT_TRIDA->class_name );
                item = htSearch( global_table, tmp_string );
                
                tHTItem *item3;
                    
                if( Err == ERR_NONE )
                {
                  if( (item3 = htSearch( tmp_table, tmp_string)) != NULL )
                  item3->data->p_uni.variable->var_type = item2->data->p_uni.variable->var_type;
                }
                
                value_return = item3->data->p_uni.variable;   //získání ukazatele do pomocné TS, kde je již proměnná pro navr. hodnotu
                
                //generování instrukcí pro správný return
                insertTab( instr_table, akt_label, I_ASSIGN, value_return, item2->data->p_uni.variable, NULL, NULL);
                insertTab( instr_table, akt_label, I_RETURN, value_return, NULL, NULL, NULL);
                
                free(tmp_string);
                result = NULL;
              }
              free(tmp_string2);
            }
            else if( tokenID.type == tt_full_id )   //jedná se o return full.ID;
            {
              if( (item2 = htSearch(global_table, tokenID.string)) == NULL )
              {
                chyba = ERR_SEM1;   //nedeklarovana promenna
                fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
              }
              if( chyba == ERR_NONE )
              {
                char *akt_label = NULL;
                int TypeFun;
                topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
                
                if( TypeFun == 0 )   //jedná se o void funkci
                {
                  chyba = ERR_SEM2;
                  fprintf(stderr, "Chyba. Řádek %d : Očekáván return;\n", token.line);
                }
                
                tmp_string = add_full_ID( tokenFun.string, AKT_TRIDA->class_name );
                item = htSearch( global_table, tmp_string );
                
                tHTItem *item3;
                    
                if( Err == ERR_NONE )
                {
                  if( (item3 = htSearch( tmp_table, tmp_string)) != NULL )
                  item3->data->p_uni.variable->var_type = item2->data->p_uni.variable->var_type;
                }
                    
                value_return = item3->data->p_uni.variable;  //získání ukazatele do pomocné TS, kde je již proměnná pro navr. hodnotu
                
                //generování instrukcí pro správný return
                insertTab( instr_table, akt_label, I_ASSIGN, value_return, item2->data->p_uni.variable, NULL, NULL);
                insertTab( instr_table, akt_label, I_RETURN, value_return, NULL, NULL, NULL);
                
                free(tmp_string);
              }
            }
            clearToken(&tokenReturn);
          }
          
          else if( tokenResult.type == tt_identifier )  //jedna se o jednoduchy identifikator
          {
            tmp_string = add_full_ID( tokenResult.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem full.ID
            if( (item = htSearch(TS_Array[position_in_arr - 1], tokenResult.string)) == NULL )
            {
              item = htSearch(global_table, tmp_string);
            }
          
            if( tokenID.type == tt_identifier )
            {
              tmp_string2 = add_full_ID( tokenID.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem full.ID
              if( (item2 = htSearch(TS_Array[position_in_arr - 1], tokenID.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
              {
                if( (item2 = htSearch(global_table, tmp_string2)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                {
                  chyba = ERR_SEM1;   //nedeklarovana promenna
                  fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                }
              }
              if( chyba == ERR_NONE )
              {
                op1 = item->data->p_uni.variable;   //L-operand
                op2 = item2->data->p_uni.variable;  //ID proměnná jejíž hodnota se přiřazuje
                
                if( tokenStatic.keyword == key_static )  //na úrovni třídy
                {
                  insertTab( instr_table, "CLASS", I_ASSIGN, op1, op2, NULL, NULL);
                  clearToken(&tokenStatic);
                }
                else   //na úrovni funkce
                {
                  char *akt_label = NULL;
                  int TypeFun;
                  topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
                  insertTab( instr_table, akt_label, I_ASSIGN, op1, op2, NULL, NULL);
                }
              }
              free(tmp_string2);
            }
            else if( tokenID.type == tt_full_id )
            {
              if( (item2 = htSearch(global_table, tokenID.string)) == NULL )
              {
                chyba = ERR_SEM1;   //nedeklarovana promenna
                fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
              }
              if( chyba == ERR_NONE )
              {
                op1 = item->data->p_uni.variable;  //L-operand
                op2 = item2->data->p_uni.variable;   //full.ID proměnná jejíž hodnota se přiřazuje
                
                if( tokenStatic.keyword == key_static )  //na úrovni třídy
                {
                  insertTab( instr_table, "CLASS", I_ASSIGN, op1, op2, NULL, NULL);
                  clearToken(&tokenStatic);
                }
                else     //na úrovni funkce
                {
                  char *akt_label = NULL;
                  int TypeFun;
                  topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
                  insertTab( instr_table, akt_label, I_ASSIGN, op1, op2, NULL, NULL);
                }
              }
            }
            clearToken(&tokenResult);
            free(tmp_string);
          }
          
          else if( tokenResult.type == tt_full_id )   //jedna se o L-operand full identifikator
          {
            if( (item = htSearch(global_table, tokenResult.string)) != NULL )
            {
              if( tokenID.type == tt_identifier )
              {
                tmp_string2 = add_full_ID( tokenID.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem ID:V
                if( (item2 = htSearch(TS_Array[position_in_arr - 1], tokenID.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
                {
                  if( (item2 = htSearch(global_table, tmp_string2)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                  {
                    chyba = ERR_SEM1;   //nedeklarovana promenna
                    fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                  }
                }
                if( chyba == ERR_NONE )
                {
                  op1 = item->data->p_uni.variable;   //L-operand
                  op2 = item2->data->p_uni.variable;    //ID proměnné jejíž hodnota se přiřazuje
                  
                  char *akt_label = NULL;
                  int TypeFun;
                  topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
                  insertTab( instr_table, akt_label, I_ASSIGN, op1, op2, NULL, NULL);
                }
                free(tmp_string2);
              }
              else if( tokenID.type == tt_full_id )
              {
                if( (item2 = htSearch(global_table, tokenID.string)) == NULL )
                {
                  chyba = ERR_SEM1;   //nedeklarovana promenna
                  fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                }
                if( chyba == ERR_NONE )
                {
                  op1 = item->data->p_uni.variable;        //L-operand
                  op2 = item2->data->p_uni.variable;      //ID proměnné jejíž hodnota se přiřazuje
                  
                  char *akt_label = NULL;
                  int TypeFun;
                  topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
                  insertTab( instr_table, akt_label, I_ASSIGN, op1, op2, NULL, NULL);
                }
              }
            }
            else
            {
              chyba = ERR_SEM1;   //nedeklarovana promenna
              fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
            }
            clearToken(&tokenResult);
          }
          OPERACE = false;
        }
        clearToken(&tokenID);
        clearToken(&tokenTmp);
        clearToken(&tokenReturn);
        //end semant
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekávána tečka nebo operace.\n", token.line);
      }
      inExp = 1;
    }
    else if(top == S_TYP)
    {
      if(token.keyword == key_int) // <typ>  ->  int    {int}
      {
        stackPop(&s);
        stackPush(&s, S_INT);
      }
      else if(token.keyword == key_double) // <typ>  -> double   {double}
      {
        stackPop(&s);
        stackPush(&s, S_DOUBLE);
      }
      else if(token.keyword == key_String) // <typ>  -> String   {String}
      {
        stackPop(&s);
        stackPush(&s, S_STRING);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván datový typ.\n", token.line);
      }
      inExp = 0;
    }
    else if(top == S_OPERATION)
    {
      if(token.type == tt_mul) // <operator>  ->  * {*}
      {
        stackPop(&s);
        stackPush(&s, S_MUL);
      }
      else if(token.type == tt_div) // <operator> -> / {/}
      {
        stackPop(&s);
        stackPush(&s, S_DIV);
      }
      else if(token.type == tt_plus) // <operator> -> + {+}
      {
        stackPop(&s);
        stackPush(&s, S_PLUS);
      }
      else if(token.type == tt_minus) // <operator> -> - {-}
      {
        stackPop(&s);
        stackPush(&s, S_MINUS);
      }
      else if(token.type == tt_less) // <operator> -> < {<}
      {
        stackPop(&s);
        stackPush(&s, S_LESS);
      }
      else if(token.type == tt_greater) // <operator> -> > {>}
      {
        stackPop(&s);
        stackPush(&s, S_GREATER);
      }
      else if(token.type == tt_lequal) // <operator> -> <= {<=}
      {
        stackPop(&s);
        stackPush(&s, S_LEQUAL);
      }
      else if(token.type == tt_gequal) // <operator> -> >= {>=}
      {
        stackPop(&s);
        stackPush(&s, S_GEQUAL);
      }
      else if(token.type == tt_same) // <operator> -> == {==}
      {
        stackPop(&s);
        stackPush(&s, S_SAME);
      }
      else if(token.type == tt_not_equal) // <operator> -> != {!=}
      {
        stackPop(&s);
        stackPush(&s, S_NOT_EQUAL);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekávána operace.\n", token.line);
      }
    } 

    if(Err != ERR_NONE) // Pokud nastala v průběhu pushování na zásobník chyba zde se detekuje a konec.
    {
      chyba = Err;
      break;
    }
    if(chyba != ERR_NONE) // Pokud nastala chyba v aplikování pravidel konec.
    {
      break;
    }
    
    // odeslání fronty tokenů syntaktické analýze výrazů
    if(inExp == 0 && tokenQueEmpty(&q) != 1)
    {
      chyba = parserExp(&q);
      if(chyba != ERR_NONE) // analýza výrazu proběhla neúspěšně
        break;
    }
//*************************************************************************************************************************      
    //------------------Následuje aplikování porovnavací operace---------------------------
    if(stackEmpty(&s) == 0)   
      stackTop(&s, &top); 
    else
      continue;

    if(top == S_CLASS)
    {
      if(token.keyword == key_class)
      {
        //semant
        if( tokenTmp.string != NULL )
          clearToken(&tokenTmp);
          
        copyToken(&tokenTmp, &token);  //zachycení aktuálního tokenu do pomocného tokenu
        //end semant
        
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván class.\n", token.line);
      }
    }
    else if(top == S_IF)
    {
      if(token.keyword == key_if)
      {        
       //semant
        if( ROUND == 2 )  //pouze v druhem pruchodu
        {
          IF_CLEAR++;   //zvyšování potřebné pro korektní vnořené podmínky
          if( tokenTmp.string != NULL )
            clearToken(&tokenTmp);
            
          copyToken(&tokenTmp, &token);    //zachycení aktuálního tokenu do pomocného tokenu
          
          if( tokenJump.string != NULL )
            clearToken(&tokenJump);
            
          copyToken(&tokenJump, &token);    //zachycení aktuálního tokenu do pomocného tokenu
        }
        //end semant
        
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván if.\n", token.line);
      }
    }
    else if(top == S_ELSE)
    {
      if(token.keyword == key_else)
      {
        //semant
        if( ROUND == 2 )
        {
          IF_CLEAR--;    //snižování potřebné pro korektní vnořené podmínky
          if( tokenJump.string != NULL )
            clearToken(&tokenTmp);
            
          copyToken(&tokenJump, &token);    //zachycení aktuálního tokenu do pomocného tokenu
        }
        //end semant
        
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván else.\n", token.line);
      }
    }
    else if(top == S_WHILE)
    {
      if(token.keyword == key_while)
      {
        //semant
        if( ROUND == 2 )  //pouze v druhem pruchodu
        {
          if( tokenTmp.string != NULL )
            clearToken(&tokenTmp);
            
          copyToken(&tokenTmp, &token);     //zachycení aktuálního tokenu do pomocného tokenu
          
          if( tokenWhile.string != NULL )
            clearToken(&tokenWhile);
            
          copyToken(&tokenWhile, &token);    //zachycení aktuálního tokenu do pomocného tokenu
        }
        //end semant
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván while.\n", token.line);
      }
    }
    else if(top == S_RETURN)
    {
      if(token.keyword == key_return)
      {
        //semant
        if( ROUND == 2 )  //pouze v druhem pruchodu
        {
          if( tokenTmp.string != NULL )
            clearToken(&tokenTmp);
            
          copyToken(&tokenTmp, &token);    //zachycení aktuálního tokenu do pomocného tokenu
          
          if( tokenReturn.string != NULL )
            clearToken(&tokenReturn);
            
          copyToken(&tokenReturn, &token);    //zachycení aktuálního tokenu do pomocného tokenu
        }
        //end semant
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván return.\n", token.line);
      }
    }
    else if(top == S_INT)
    {
      if(token.keyword == key_int)
      {
        //semant
        if( tokenTmp.string != NULL )
            clearToken(&tokenTmp);
            
        copyToken(&tokenTmp, &token);    //zachycení aktuálního tokenu do pomocného tokenu
        //end semant
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván int.\n", token.line);
      }
    }
    else if(top == S_DOUBLE)
    {
      if(token.keyword == key_double)
      {
        //semant
        if( tokenTmp.string != NULL )
          clearToken(&tokenTmp);
          
        copyToken(&tokenTmp, &token);    //zachycení aktuálního tokenu do pomocného tokenu
        //end semant
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván double.\n", token.line);
      }
    }
    else if(top == S_STRING)
    {
      if(token.keyword == key_String)
      {
        //semant
        if( tokenTmp.string != NULL )
          clearToken(&tokenTmp);
          
        copyToken(&tokenTmp, &token);  //zachycení aktuálního tokenu do pomocného tokenu
        //end semant
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván String.\n", token.line);
      }
    }
    else if(top == S_STATIC)
    {
      if(token.keyword == key_static)
      {
        //semant
        if( tokenStatic.string != NULL )
          clearToken(&tokenStatic);
          
        copyToken(&tokenStatic, &token);  //zachycení aktuálního tokenu do pomocného tokenu
        //end semant
        
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván static.\n", token.line);
      }
    }
    else if(top == S_VOID)
    {
      if(token.keyword == key_void)
      {
        //semant
        if( tokenTmp.string != NULL )
          clearToken(&tokenTmp);
          
        copyToken(&tokenTmp, &token);  //zachycení aktuálního tokenu do pomocného tokenu
        //end semant
        
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván void.\n", token.line);
      }
    }
    else if(top == S_TERM)
    {
      if(token.type == tt_identifier || token.type == tt_literal || token.type == tt_int || token.type == tt_double || token.type == tt_full_id)
      {        
        //semant
        if( tokenTerm.string != NULL )
          clearToken(&tokenTerm);
           
        copyToken(&tokenTerm, &token);  //zachycení aktuálního tokenu do pomocného tokenu
        //end semant
        
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván identifikátor, literál, číslo.\n", token.line);
      }
    }
    else if(top == S_ID)
    {
      if(token.type == tt_identifier)
      {
        //semant
        if(tokenID.string != NULL)
          clearToken(&tokenID);
         
        copyToken(&tokenID, &token);  //zkopirujeme si obsah do pomocneho tokenu
        //end semant      
        
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván identifikátor.\n", token.line);
      }
    }
    else if(top == S_ID_FULL_ID)
    {
      if(token.type == tt_full_id || token.type == tt_identifier)
      {
        //semant
        if(tokenID.string != NULL)
          clearToken(&tokenID);   //korektni free
         
        copyToken(&tokenID, &token);  //zkopirujeme si obsah do pomocneho tokenu
        //end semant 

        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván plný identifikátor nebo identifikátor.\n", token.line);
      }
    }
    else if(top == S_COMMA)
    {
      if(token.type == tt_comma)
      {
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván a ,.\n", token.line);
      }
    }
    else if(top == S_SEMICOLON)
    {
      if(token.type == tt_semicolon)
      {
        //semant
        if( ROUND == 1 )
        {
          if(tokenStatic.keyword == key_static)   //jedna se o deklaraci glob. promenne bez inicializace
          {
            char *tmp_string;
            tmp_string = add_full_ID( tokenID.string, AKT_TRIDA->class_name );   //promenna bude do TS vlozena pod klicem Class.ID
            
            tHTItem* item;
            if( (item = htSearch(global_table, tmp_string)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
            {
              Class *data_prom = get_VarData(&tokenTmp, tokenID.string);   //vytvorime zaznam o datovem typu a identifikatoru 
              htInsert ( global_table, tmp_string, data_prom );   //vlozime ji jako polozku do tabulky symbolu
            }
            else   //identifikator je nalezen v glob. tabulce symbolu ve stejne class
            {
              chyba = ERR_SEM1;   //pokus o redefinici staticke promenne
              fprintf(stderr, "Chyba. Řádek %d : Pokus o redefinici globální proměnné.\n", token.line);
            }
            free(tmp_string);
          }
          clearToken(&tokenStatic);
        }
        else if( ROUND == 2 )  //zde se bude jednat o definici nestaticke=lokalni promenne
        { 
          if( (tokenStatic.keyword != key_static) && (tokenTmp.keyword == key_int || tokenTmp.keyword == key_double || tokenTmp.keyword == key_String) )
          {
            tHTItem* item;
            if( (item = htSearch(TS_Array[position_in_arr - 1], tokenID.string) ) == NULL )  //promenna nenalezena v lok. tabulce symbolu
            {
              Class *data_prom = get_VarData(&tokenTmp, tokenID.string);   //vytvorime zaznam o datovem typu a identifikatoru 
              htInsert ( TS_Array[position_in_arr - 1], tokenID.string, data_prom );   //vlozime ji jako polozku do tabulky symbolu (key = ID)
            }
            else
            {
              chyba = ERR_SEM1;
              fprintf(stderr, "Chyba. Řádek %d : Pokus o redefinici lokální proměnné.\n", token.line);
            }
          }
        }
        clearToken(&tokenID);
        clearToken(&tokenTmp);
        //semant end
        
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván ;.\n", token.line);
      }
    } 
    else if(top == S_BRACKET_LEFT)
    {
      if(token.type == tt_bracket_left)
      {
        //semant
        if( ROUND == 1 )
        {
          if(tokenStatic.keyword == key_static)   //jedna se o deklaraci/definici funkce
          {
            char *tmp_string;
            tmp_string = add_full_ID( tokenID.string, AKT_TRIDA->class_name );  //jedna se u funkci-> klic Class.ID
            
            tHTItem *item;
            if( (item = htSearch(global_table, tmp_string) ) == NULL )  //funkce nenalezena v glob. tabulce symbolu
            {
              Class *data_fun = get_FunData(&tokenTmp, tokenID.string);   //vytvorime zaznam o datovem typu a identifikatoru
              data_fun->p_uni.function->position_arr = position_in_arr - 1;   //index pro lokalni tabulku symb teto funkce
              htInsert ( global_table, tmp_string, data_fun );   //vlozime ji jako polozku do tabulky symbolu
              copyToken(&tokenParam, &tokenID);
              
              //odkaz do tabulky symbolu tmp_table pro pripadnou return value
              Class *new = Create_Var_to_Table();           
              htInsert ( tmp_table, tmp_string, new );   //bude zde ulozena jako full.id funkce
            }
            else  //identifikator je nalezen v glob. tabulce symbolu a je ve stejne class
            {
              chyba = ERR_SEM1;   //pokus o redefinici funkce
              fprintf(stderr, "Chyba. Řádek %d : Pokus o redefinici funkce.\n", token.line);
            }
            free(tmp_string);
          }
        }
        
        if( ROUND == 2 )
        {
          if( tokenStatic.keyword == key_static )   //v místě kde se definuje funkce vytvoříme label
          {
            if( tokenID.string != NULL && OPERACE == false)
            {
              char *label;
              //jedná se o uživatelskou funkci Main.run, vytvoříme pro ni speciální label
              if( (strcmp(tokenID.string, "run") == 0) && (strcmp(AKT_TRIDA->class_name, "Main") == 0) && (tokenTmp.keyword == key_void) )
              {
                label = Create_MainRun_Label( label, "Main");
                pushStackLabel( &label_stack, label, 0);
              }      
              else  //jedná se o jinou uživatelskou funkci, vytvoříme pro ni label
              {
                label = add_full_ID( tokenID.string, AKT_TRIDA->class_name );  //label bude nazvan stejne jako full.id funkce
                
                if( tokenTmp.keyword == key_void )
                  pushStackLabel( &label_stack, label, 0);   //0 znamena void funkce
                else  
                  pushStackLabel( &label_stack, label, 1);   //1 znamena non-void funkci
              }
              
              copyToken( &tokenFun, &tokenID );   //zachytime si ID funkce do pomocneho tokenu
              free(label);              
            }
          }
          
          else if( inside_fun == true && (tokenTmp.type == tt_empty) )  //takto vyloucime definici funkci, budeme hledat pouze volani funkci
          {
            if( tokenID.string != NULL && OPERACE == false)
            {
              if( strcmp(tokenID.string, "ifj16.find") == 0 || strcmp(tokenID.string, "ifj16.print") == 0 
                || strcmp(tokenID.string, "ifj16.sort") == 0 || strcmp(tokenID.string, "ifj16.compare") == 0 
                || strcmp(tokenID.string, "ifj16.substr") == 0 || strcmp(tokenID.string, "ifj16.lenght") == 0 
                || strcmp(tokenID.string, "ifj16.readString") == 0 || strcmp(tokenID.string, "ifj16.readDouble") == 0 
                || strcmp(tokenID.string, "ifj16.readInt") == 0)
              {
                //Jedna se o vestavenou funkci
                copyToken(&tokenParam, &tokenID);
              }
              else  //jedna se o volani uzivatelske funkce
              {
                tHTItem *item;
                if( tokenID.type == tt_identifier )
                {
                  char *tmp_string;
                  tmp_string = add_full_ID( tokenID.string, AKT_TRIDA->class_name );  //jedna se o funkci-> klic Class.ID
                  if( (item = htSearch(global_table, tmp_string) ) == NULL )  //funkce nenalezena v glob. tabulce symbolu
                  {
                    chyba = ERR_SEM1;   //nedefinovana funkce
                    fprintf(stderr, "Chyba. Řádek %d : nedefinovaná funkce.\n", token.line);
                  }
                  else
                  {
                    copyToken(&tokenParam, &tokenID);  //uložení ID funkce pro práci s parametry a generování 3AC
                  }
                  free(tmp_string);
                }
                else if( tokenID.type == tt_full_id )
                {
                  if( (item = htSearch(global_table, tokenID.string) ) == NULL )  //funkce nenalezena v glob. tabulce symbolu
                  {
                    chyba = ERR_SEM1;   //nedefinovana funkce
                    fprintf(stderr, "Chyba. Řádek %d : nedefinovaná funkce.\n", token.line);
                  }
                  else
                  {
                    copyToken(&tokenParam, &tokenID);  //uložení ID funkce pro práci s parametry a generování 3AC
                  }
                }
              }
            }
          }
        }
        clearToken(&tokenTmp);
        clearToken(&tokenStatic);
        clearToken(&tokenID);
        //semant end
        
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekávána (.\n", token.line);
      }
    }  
    else if(top == S_BRACKET_RIGHT)
    {
      if(token.type == tt_bracket_right)
      {
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekávána ).\n", token.line);
      }
    } 
    else if(top == S_CURLY_LEFT)
    {
      if(token.type == tt_curly_left)
      {
        //semant
        if(tokenTmp.keyword == key_class)
        {             
          AKT_TRIDA = get_ClassName( tokenID.string );   //začátek nové třídy, uložíme si její jmeno do glob. proměnné
          AKT_TRIDA->type = t_class;   //jedna se o tridu          
          if( ROUND == 1 )             //definice pouze v prvnim pruchodu
          {
            tHTItem *item;
            char *tmp_string;
            tmp_string = add_char_ID( tokenID.string, 'C');   //klíč v TS bude ve formátu Class:C
            if( (item = htSearch(global_table, tmp_string)) != NULL )  //class id nalezeno v glob. tabulce symbolu
            {
              if( item->data->type == 2 )   //pro jistotu ověření t_class
              {
                chyba = ERR_SEM1;   //pokus o redefinici tridy
                fprintf(stderr, "Chyba. Řádek %d : Pokus o redefinici třídy.\n", token.line);
                
                if( AKT_TRIDA != NULL && ROUND == 1)  //korektni uvolnění a nastavení AKT_TRIDA na NULL
                {
                  if( AKT_TRIDA->class_name != NULL)
                  {
                    free(AKT_TRIDA->class_name);
                    AKT_TRIDA->class_name = NULL;
                  }
                  free(AKT_TRIDA);
                  AKT_TRIDA = NULL;
                }
              }
              else
              {
                if( ROUND == 1 )  
                  htInsert ( global_table, tmp_string, AKT_TRIDA );   //vlozime ji jako polozku do tabulky symbolu
              }
            }
            else if( strcmp( tokenID.string, "ifj16" ) == 0 )
            {
                chyba = ERR_SEM1;
                fprintf(stderr, "Chyba. Řádek %d : Nelze definovat třídu ifj16 jako uzivatelskou!.\n", token.line);
                if( ROUND == 1 )
                  htInsert ( global_table, tmp_string, AKT_TRIDA );   //vlozime ji jako polozku do tabulky symbolu, aby se mohla uvolnit
            }
            else
            {
              if( ROUND == 1 )
                htInsert ( global_table, tmp_string, AKT_TRIDA );   //vlozime ji jako polozku do tabulky symbolu
            }
            free(tmp_string);
          }
          clearToken(&tokenTmp);
          clearToken(&tokenID);
        }
        
        if( ROUND == 2 )  //vytvoření labelu pro else
        {
          if( (tokenJump.keyword == key_else && tokenWhile.keyword != key_while) || (IF_IN_WHILE > 0) )
          {
            char *label;
            int number;
            JumpStackTop(&jump_stack, &number);
            label = Get_Else_number( number );   //formát labelu "else + 'number'"
            
            pushStackLabel( &label_stack, label, -1);
            free(label);
            
            JumpStackPop(&jump_stack);   //odstraníme vrchol zásobníku pro If,Else
          }
        }
        //semant end
    
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekávána {.\n", token.line);
      }
    }  
    else if(top == S_CURLY_RIGHT)
    {
      if(token.type == tt_curly_right)
      {
        //semant
        if( inside_sekv > 0 )
        {
          inside_sekv--;   //postupne se nachazi '}' ke sve prislusne '{'
        }
        else if( (inside_sekv == 0) && (inside_fun == true) )
        {
          inside_fun = false;        //nyni jsme se dostali na konec definici funkce
        }
        else if( (inside_sekv == 0) && (inside_fun == false) && (inside_class == true))
        {
          inside_class = false;     //nyni jsme se dostali na konec definice tridy
          if( chyba == ERR_NONE )   //korektní uvolnění AKT_TRIDA a nastavení na NULL
          {
            if( AKT_TRIDA != NULL && ROUND == 2)
            {
              if( AKT_TRIDA->class_name != NULL)
              {
                free(AKT_TRIDA->class_name);
                AKT_TRIDA->class_name = NULL;
              }
              free(AKT_TRIDA);
              AKT_TRIDA = NULL;
            }
          }
        }
        if ( ROUND == 2 )
        {
          if( tokenJump.keyword == key_if && tokenWhile.keyword != key_while ) //if ktere neni v cyklu
          {
            popStackLabel(&label_stack);
            clearToken(&tokenJump);                       
          }
          else if( tokenJump.keyword == key_else && tokenWhile.keyword != key_while )  //else ktere neni v cyklu
          {           
            popStackLabel(&label_stack);
            clearToken(&tokenJump);                       
          }
          else if( IF_CLEAR > 0 && tokenWhile.keyword != key_while )  //if/else, ktere není v cyklu zjištěné pomocí pomocné proměnné
          {           
            popStackLabel(&label_stack);
            clearToken(&tokenJump);                       
          }
          else if( tokenWhile.keyword == key_while )
          {
            if( IF_IN_WHILE > 0 && ( tokenJump.keyword == key_if || tokenJump.keyword == key_else ) )
            {
              popStackLabel(&label_stack);
              clearToken(&tokenJump);
              
              IF_IN_WHILE--;   //situace kdy je IF ve while
            }
            else  //jedna se o while, kde neni žádná podmínka
            {
              char *akt_label = NULL;
              int TypeFun;
              topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
              
              //instrukce která umožní skočit opět na začátek cyklu
              insertTab( instr_table, akt_label, I_WJUMP, NULL, NULL, NULL, akt_label);
              
              popStackLabel(&label_stack);
              
              WHILE_CLEAR--;       //upráva proměnné při vnořených cyklech
              if( WHILE_CLEAR == 0 )
                clearToken(&tokenWhile);
            }
          }
          else if( tokenFun.string != NULL )   //konec funkce, odstraníme label
          {
            popStackLabel(&label_stack);
            clearToken(&tokenFun);
          }
        }
        //end semant  
    
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekávána }.\n", token.line);
      }
    } 
    else if(top == S_MUL)
    {
      if(token.type == tt_mul)
      {
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván operátor.\n", token.line);
      }
    } 
    else if(top == S_DIV)
    {
      if(token.type == tt_div)
      {
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván operátor.\n", token.line);
      }
    } 
    else if(top == S_PLUS)
    {
      if(token.type == tt_plus)
      {
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván operátor.\n", token.line);
      }
    } 
    else if(top == S_MINUS)
    {
      if(token.type == tt_minus)
      {
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván operátor.\n", token.line);
      }
    } 
    else if(top == S_LESS)
    {
      if(token.type == tt_less)
      {
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván operátor.\n", token.line);
      }
    } 
    else if(top == S_GREATER)
    {
      if(token.type == tt_greater)
      {
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván operátor.\n", token.line);
      }
    } 
    else if(top == S_EQUAL)
    {
      if(token.type == tt_equal)
      {
        //semant
        if( tokenResult.type != tt_empty )
        {
          clearToken(&tokenResult);   //pokud by zustalo něco v tokenu L-operandu z předchozího assign, bude uvolněn 
        }
        
        if ( ROUND == 1 )
        {
          if(tokenStatic.keyword == key_static)   //jedna se o deklaraci glob. promenne s inicializaci
          {
            char *tmp_string;
            tmp_string = add_full_ID( tokenID.string, AKT_TRIDA->class_name );   //promenna bude do gTS vlozena pod klicem Class.ID
            tHTItem* item;
            if( (item = htSearch(global_table, tmp_string)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
            {
              Class *data_prom = get_VarData(&tokenTmp, tokenID.string);   //vytvorime zaznam o datovem typu a identifikatoru
              htInsert ( global_table, tmp_string, data_prom );   //vlozime ji jako polozku do tabulky symbolu
            }
            else   //identifikator je nalezen v glob. tabulce symbolu ve stejne class
            {
              chyba = ERR_SEM1;   //pokus o redefinici staticke promenne
              fprintf(stderr, "Chyba. Řádek %d : Pokus o redefinici globální proměnné.\n", token.line);
            }
            free(tmp_string);
          }
          clearToken(&tokenStatic);
        }
        
        if( ROUND == 2 )
        {          //zde se bude jednat o definici nestaticke(lokalní) proměnné
          if(tokenStatic.keyword != key_static && (tokenTmp.keyword == key_int || tokenTmp.keyword == key_double || tokenTmp.keyword == key_String) )
          {
            if( htSearch(TS_Array[position_in_arr - 1], tokenID.string) == NULL )  //promenna nenalezena v lok. tabulce symbolu
            {
              Class *data_prom = get_VarData(&tokenTmp, tokenID.string);   //vytvorime zaznam o datovem typu a identifikatoru 
              htInsert ( TS_Array[position_in_arr - 1], tokenID.string, data_prom );   //vlozime ji jako polozku do tabulky symbolu
            }
            else
            {
              chyba = ERR_SEM1;
              fprintf(stderr, "Chyba. Řádek %d : Pokus o redefinici lokální proměnné.\n", token.line);
            }
          }
          
          else if( (tokenStatic.keyword != key_static) && (tokenTmp.type == tt_empty) )  //zkontrolujeme zda je proměnná už definovaná
          {
            tHTItem* item;
            if( tokenID.type == tt_identifier )  //jedna se o jednoduchy identifikator
            {
              char *tmp_string;
              tmp_string = add_full_ID( tokenID.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem ID:V
              if( (item = htSearch(TS_Array[position_in_arr - 1], tokenID.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
              {
                if( (item = htSearch(global_table, tmp_string)) == NULL )  //promenna nenalezena v glob. tabulce symbolu
                {
                  chyba = ERR_SEM1;   //nedeklarovana promenna
                  fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                }
              }
              free(tmp_string);
            }
            else if( tokenID.type == tt_full_id )  //jedna se o full identifikator
            {
              if( (item = htSearch(global_table, tokenID.string )) == NULL )
              {
                chyba = ERR_SEM1;   //nedeklarovana promenna
                fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
              }
            }
          }
          copyToken(&tokenResult, &tokenID);   //zkopirujeme si udaje o L-operandu při přiřazení
        }
        clearToken(&tokenID);
        clearToken(&tokenTmp);  
        //end semant
        
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván operátor.\n", token.line);
      }
    }
    else if(top == S_LEQUAL)
    {
      if(token.type == tt_lequal)
      {
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván operátor.\n", token.line);
      }
    }  
    else if(top == S_GEQUAL)
    {
      if(token.type == tt_gequal)
      {
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván operátor.\n", token.line);
      }
    } 
    else if(top == S_SAME)
    {
      if(token.type == tt_same)
      {
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván operátor.\n", token.line);
      }
    } 
    else if(top == S_NOT_EQUAL)
    {
      if(token.type == tt_not_equal)
      {
        stackPop(&s);
        clearToken(&token);
        processToken(&token);
      }
      else
      {
        chyba = ERR_SYN;
        fprintf(stderr, "Chyba. Řádek %d : očekáván operátor.\n", token.line);
      }
    } 
    if(Err != ERR_NONE) // Pokud nastala v průběhu získavaní nových tokenů chyba konec.
    {
      chyba = Err;
      break;
    }
    if(chyba != ERR_NONE) // Pokud nastala chyba v aplikování pravidel konec.
    {
      break;
    } 
  } // end while

  // korektní free
  while(tokenQueEmpty(&q) != 1)
    tokenQueRemove(&q);
  while(stackEmpty(&s) != 1)
    stackPop(&s);
  while(JumpStackEmpty(&jump_stack) != 1)
    JumpStackPop(&jump_stack);
    
  clearToken(&token);
  clearToken(&tokenTmp);
  clearToken(&tokenStatic);
  clearToken(&tokenID);
  clearToken(&tokenParam);
  clearToken(&tokenTerm);
  clearToken(&tokenFun);
  clearToken(&tokenJump);
  clearToken(&tokenWhile);
  clearToken(&tokenReturn);
  clearToken(&tokenResult);
  
  
  if( (ROUND == 2) && (chyba == ERR_NONE) )
  {
    chyba = interpret(instr_table);   //interpret bude zahájen pouze pokud byla veškerá analýza úspěšná
  }
  
  if( ROUND == 1 )
  {
    n_functions = position_in_arr;  //celkový počet funkcí
    position_in_arr = 0;           //vynulujeme index pro novy pruchod
    AKT_TRIDA = NULL;        //nastaveni na NULL pred druhym pruchodem
  }

  if( (chyba != ERR_NONE) || (ROUND == 2) )  //uvolnujeme pouze v pripade chyby nebo na konci druheho pruchodu
  {    
    htClearAll(global_table);  //uvolnime globalni tabulku
    free(global_table);
    
    int j;
    for( j=0; j<n_functions; j++)
    {
      htClearAll(TS_Array[j]);  //uvolnime pole tabulek symbolu
      free(TS_Array[j]);
    }
    
    if( tmp_table != NULL )
    {
      htClearAll(tmp_table);  //uvolnime pomocnou tabulku
      free(tmp_table);
    }
    
    if( ROUND == 2 )  //tyto akce pouze v druhém průchodu
    {
      while( emptyStackLabel(&label_stack) != 1)
      {
        popStackLabel(&label_stack);  //vyčistíme zásobník labelů
      }
      if( instr_table != NULL )
      {
        disposeTab(instr_table);  //uvolníme instrukční tabulku
        free(instr_table);
      }
    }
    
    closeFile();   //uzavreme soubor
  }
  
  if( chyba != ERR_NONE )  //korektní uvolnění AKT_TRIDA v případě chyby
  {
    if( AKT_TRIDA != NULL )
    {
      if( AKT_TRIDA->class_name != NULL)
      {
        free(AKT_TRIDA->class_name);
        AKT_TRIDA->class_name = NULL;
      }
      free(AKT_TRIDA);
      AKT_TRIDA = NULL;
    }
  }
  
  return chyba;
}

//***************************************************************************************************************************************
// Syntaktická analýza pro výrazy (zdola nahoru)

//funkce pro překlad token.type na precItem
precItem translate(Token token)
{
  if(token.type == tt_mul)
    return SV_MULTIPLY;
  else if(token.type == tt_div)
    return SV_DIVIDE;
  else if(token.type == tt_plus)
    return SV_PLUS;
  else if(token.type == tt_minus)
    return SV_MINUS;
  else if(token.type == tt_less)
    return SV_LESS;
  else if(token.type == tt_greater)
    return SV_GREATER;
  else if(token.type == tt_lequal)
    return SV_LEQUAL;
  else if(token.type == tt_gequal)
    return SV_GEQUAL;
  else if(token.type == tt_same)
    return SV_SAME;
  else if(token.type == tt_not_equal)
    return SV_NEQUAL;
  else if(token.type == tt_bracket_left)
    return SV_LBRACKET;
  else if(token.type == tt_bracket_right)
    return SV_RBRACKET;
  else if(token.type == tt_identifier || token.type == tt_full_id || token.type == tt_literal || token.type == tt_int || token.type == tt_double)
    return SV_TERM;
  else if(token.type == tt_semicolon)
    return SV_DOLLAR;
  else
    return SV_NOTHING;
}

const precOp precTab[14][14] = {
          // *  /  +  -  <  > <=  >=  == !=  (  )  term  $
/* * */     {V, V, V, V, V, V, V, V,  M,  V, M, V,  M,   V},
/* / */     {M, M, V, V, V, V, V, V,  M,  V, M, V,  M,   V},
/* + */     {M, M, V, V, V, V, V, V,  M,  V, M, V,  M,   V},
/* - */     {M, M, V, V, V, V, V, V,  M,  V, M, V,  M,   V},
/* < */     {M, M, M, M, V, V, V, V,  M,  V, M, V,  M,   V},
/* > */     {M, M, M, M, V, V, V, V,  M,  V, M, V,  M,   V},
/* <= */    {M, M, M, M, V, V, V, V,  M,  V, M, V,  M,   V},
/* >= */    {M, M, M, M, V, V, V, V,  M,  V, M, V,  M,   V},
/* == */    {M, M, M, M, V, V, V, V,  M,  V, M, V,  M,   V},
/* != */    {M, M, M, M, V, V, V, V,  M,  V, M, V,  M,   V},
/* ( */     {M, M, M, M, M, M, M, M,  M,  M, M, R,  M,   X},
/* ) */     {V, V, V, V, V, V, V, V,  V,  V, X, V,  X,   V},
/* term */  {V, V, V, V, V, V, V, V,  V,  V, X, V,  X,   V},
/* $ */     {M, M, M, M, M, M, M, M,  M,  M, M, X,  M,   X}
            };

//syntaktická analýza vyrázů
ErrorType parserExp(tTokenQue *q)
{
  ErrorType chyba = ERR_NONE;
  Token token;     //hlavní token z fronty
  Token tokenTmp;  //pomocný token při sémantických akcích

  tPrecStack s; // hlavní zásobník
  precStackInit(&s);
  precStackPush(&s, SV_DOLLAR); // vložení na zásobník $

  tPrecStack stmp; // pomocný zásobník
  precStackInit(&stmp);
  
  initToken(&token);
  tokenQueFront(q, &token);

  precOp op;
  precItem itm = translate(token); // uchování překladu aktuálního tokenů
  precItem itm2; // první terminál na zásobníku
  precItem itmTmp; // pomocná proměnná
  
  //semant
  tSemStack sem_stack;  //sémantický zásobník
  SemStackInit(&sem_stack);
  
  initToken(&tokenTmp);
  
  int start_redukce = false;   //nastaví se na true, až se nalezne první operátor při redukci
  int konec_redukce = false;   //nastaví se na true, až je nashiftován poslední token
  int bracket = false;         //nastaví se na true, v případě nalezení '('
  //end semant
  
  while(1)
  {
    if(itm != SV_DOLLAR) // v případě že je v itm DOLLAR již nepřekládej protože jsme na konci čtecí pásky
    {
      itm = translate(token);
    }

    if(itm == SV_NOTHING) // nelze přeložit token -> syntaktická chyba
    {
      chyba = ERR_SYN;
      break;
    }
    if(precStackEmpty(&s) == 1)
    {
      chyba = ERR_SYN;
      break;
    }
    precStackTop(&s, &itmTmp);
    while(itmTmp == SV_NETERM) // hledání nejvýše se nachazejicího terminálu na zásobníku
    {
      if(precStackEmpty(&s) == 1)
      {
         chyba = ERR_SYN;
         break;
      }
      precStackPush(&stmp, itmTmp);
      precStackPop(&s);  
      precStackTop(&s, &itmTmp);
    }
    if(chyba != ERR_NONE) // v případě hledání nejvýše se nachazejicího terminálu na zásobníku nastane chyba konec
      break;

    if(precStackEmpty(&s) == 1)
    {
      chyba = ERR_SYN;
      break;
    }
    precStackTop(&s, &itm2); // nejvýše se nachazejicí term na zasobniku

    while(precStackEmpty(&stmp) != 1) // vracení všech hodnot zpět na zásobník po hledání termu
    {
      precStackTop(&stmp, &itmTmp);
      precStackPush(&s, itmTmp);
      precStackPop(&stmp);
    }
    op = precTab[itm2][itm]; // určení co se má dělat na základě prec tabulky
//******************************************************************************************************
    if(op == X) // chyba
    {
      if(precStackEmpty(&s) == 1)
      {
        chyba = ERR_SYN;
        break;
      }
      precStackTop(&s, &itmTmp);
      if(itmTmp == SV_NETERM) // v případě že na zásobníku je už pouze $E a na čtecí pásce pouze $ to znamená úspešný konec
      {
        precStackPop(&s);
        if(precStackEmpty(&s) == 1)
        {
          chyba = ERR_SYN;
          break;
        }
        precStackTop(&s, &itmTmp);
        if(itmTmp == SV_DOLLAR)
        {
          chyba = ERR_NONE;
          //semant
          OPERACE = false;   //nastavení OPERACE zpět na počáteční 0
          break;
        }
      }
      chyba = ERR_SYN;
      break;
    }
//******************************************************************************************************
    else if(op == V) //redukuj
    {
      if(precStackEmpty(&s) == 1)
      {
        chyba = ERR_SYN;
        break;
      }
      precStackTop(&s, &itmTmp);
      while(itmTmp != SV_SHIFTUJ) // hledání shiftuj na zásobníku
      {
        if(precStackEmpty(&s) == 1)
        {
          chyba = ERR_SYN;
          break;
        }
        precStackPush(&stmp, itmTmp);
        precStackPop(&s);  
        precStackTop(&s, &itmTmp);
      }
      if(chyba != ERR_NONE) // v případě hledání shiftuj na zásobníku nastane chyba konec
        break;

      precStackPop(&s); // odstranění samotné shiftuj (<)

      if(precStackEmpty(&s) == 1)
      {
        chyba = ERR_SYN;
        break;
      }
      precStackTop(&stmp, &itmTmp);
      if(itmTmp == SV_TERM) // E -> term
      {
        //semant
        start_redukce = false;
        
        precStackPop(&stmp);
        precStackPush(&s, SV_NETERM);        
      }
      else if(itmTmp == SV_LBRACKET) // E -> (E)
      {
        //semant
        start_redukce = true;
        
        precStackPop(&stmp);
        if(precStackEmpty(&s) == 1)
        {
          chyba = ERR_SYN;
          break;
        }
        precStackTop(&stmp, &itmTmp);
        if(itmTmp == SV_NETERM)
        {
          precStackPop(&stmp);
          if(precStackEmpty(&s) == 1)
          {
            chyba = ERR_SYN;
            break;
          }
          precStackTop(&stmp, &itmTmp);
          if(itmTmp == SV_RBRACKET)
          {
            precStackPop(&stmp);
            precStackPush(&s, SV_NETERM);
          }
          else // neúspěšné aplikování pravidla
          {
            chyba = ERR_SYN;
            break;
          }
        }
        else // neúspšné aplikování pravidla
        {
          chyba = ERR_SYN;
          break;
        }
      }
      else if(itmTmp == SV_NETERM) // E -> E op E
      {
        precStackPop(&stmp);
        if(precStackEmpty(&s) == 1)
        {
          chyba = ERR_SYN;
          break;
        }
        precStackTop(&stmp, &itmTmp);
        if(itmTmp == SV_MULTIPLY || itmTmp == SV_DIVIDE || itmTmp == SV_PLUS || itmTmp == SV_MINUS || itmTmp == SV_LESS ||
         itmTmp == SV_GREATER || itmTmp == SV_LEQUAL || itmTmp == SV_GEQUAL || itmTmp == SV_SAME || itmTmp == SV_NEQUAL)
        {
          precStackPop(&stmp);
          if(precStackEmpty(&s) == 1)
          {
            chyba = ERR_SYN;
            break;
          }
          
          //semant
          start_redukce = true;
          
          precStackTop(&stmp, &itmTmp);
          if(itmTmp == SV_NETERM)
          {
            precStackPop(&stmp);
            precStackPush(&s, SV_NETERM);
          }
          else // neúspěšné aplikování pravidla
          {
            chyba = ERR_SYN;
            break;
          }
        }
        else // neúspěšné aplikování pravidla
        {
          chyba = ERR_SYN;
          break;
        }
      }
      else // v případě že nelze aplikovat žádné pravidlo
      {
        chyba = ERR_SYN;
        break;
      }
      
      //semant
      if( ROUND == 2 )  //sémantické vyhodnocování výrazů pouze v druhém průchodu
      {
        if( OPERACE == true )  //takto vyloučíme z výrazů obyčejné assign, to je řešeno v predikt. synt. analýze
        {
          if( start_redukce == true )  //zahájíme pouze pokud byla nalezena ( nebo operátor
          { 
            tHTItem* item;
            char *tmp_string;
            
            clearToken(&tokenTmp);
            SemStackTop( &sem_stack, &tokenTmp );  //vložíme vrchol zásobníku tokenů
            
            if( tokenTmp.type == tt_identifier )  //jedna se o jednoduchy identifikator
            {
              tmp_string = add_full_ID( tokenTmp.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem Class.ID
              if( (item = htSearch(TS_Array[position_in_arr - 1], tokenTmp.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
              {
                if( (item = htSearch(global_table, tmp_string)) == NULL )   //identifikator nenalezen ani v glob. tab. symbolu
                {
                  chyba = ERR_SEM1;   //nedeklarovana promenna
                  fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                }
              }
              if( chyba == ERR_NONE )
              {
                op2 = item->data->p_uni.variable;  //druhý operand bude ID
              }
              free(tmp_string);
            }
            else if( tokenTmp.type == tt_full_id )  //jedna se o full identifikator
            {
              if( (item = htSearch(global_table, tokenTmp.string)) != NULL )
              {
                op2 = item->data->p_uni.variable;  //druhý operand bude full.id
              }
              else
              {
                chyba = ERR_SEM1;   //nedeklarovana promenna
                fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
              }
            }
            else if( tokenTmp.type == tt_int || tokenTmp.type == tt_double || tokenTmp.type == tt_literal )
            {
              Class *new = Create_Const_to_Table(&tokenTmp);  //vytvoříme si novou položku do TS a získáme hodnotu termu
              
              Token tokenKey;
              initToken(&tokenKey);
              Create_Token_string( &tokenKey, "const" );
              
              if( Err == ERR_NONE )
              {
                htInsert ( tmp_table, tokenKey.string, new );
                item = htSearch( tmp_table, tokenKey.string );
                op2 = item->data->p_uni.variable;   //druhý operand bude term
              }
              clearToken(&tokenKey);
            }
            else if( tokenTmp.type == tt_bracket_right )
            {
              bracket = true;   //pokud je nalezena ), nastavíme si o tom info do proměnné bracket
            }
            else if( tokenTmp.type == tt_empty )
            {
              if( (item = htSearch(tmp_table, tokenTmp.string)) != NULL )
              {
                op2 = item->data->p_uni.variable;  //druhý operand bude již dříve vytvořený result
              }
              else
              {
                chyba = ERR_SEM1;   //nedeklarovana promenna
                fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
              }
            }
            
            SemStackPop(&sem_stack);   //pop operandu ze zasobniku
            clearToken(&tokenTmp);
            SemStackTop( &sem_stack, &tokenTmp);  //do tokenTmp vložíme operátor na vrcholu zásobníku
            
            Token tokenBracket;
            initToken(&tokenBracket);
            if( bracket == true )
            {
              copyToken(&tokenBracket, &tokenTmp);  //do pomocného tokenu vložíme zredukovaný result který je ( )
            }
            else  //získáme typ operátoru
            {
              if( tokenTmp.type == tt_plus )
              {
                operator = I_PLUS;
              }
              else if( tokenTmp.type == tt_minus )
              {
                operator = I_MINUS;
              }
              else if( tokenTmp.type == tt_mul )
              {
                operator = I_MUL;
              }
              else if( tokenTmp.type == tt_div )
              {
                operator = I_DIV;
              }
              else if( tokenTmp.type == tt_minus )
              {
                operator = I_MINUS;
              }
              else if( tokenTmp.type == tt_less )
              {
                operator = I_LESS;
              }
              else if( tokenTmp.type == tt_lequal )
              {
                operator = I_LEQUAL;
              }
              else if( tokenTmp.type == tt_greater )
              {
                operator = I_GREATER;
              }
              else if( tokenTmp.type == tt_gequal )
              {
                operator = I_GEQUAL;
              }
              else if( tokenTmp.type == tt_same )
              {
                operator = I_SAME;
              }
              else if( tokenTmp.type == tt_not_equal )
              {
                operator = I_NOT_EQUAL;
              }
              else
              {
                operator = I_NOP;
              }
            }
            
            SemStackPop(&sem_stack);   //pop operatoru ze zasobniku
            clearToken(&tokenTmp);        
            SemStackTop( &sem_stack, &tokenTmp);  //do tokenTmp vložíme operand na vrcholu zásobníku    

            if( tokenTmp.type == tt_identifier )  //jedna se o jednoduchy identifikator
            {
              tmp_string = add_full_ID( tokenTmp.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem Class.ID
              if( (item = htSearch(TS_Array[position_in_arr - 1], tokenTmp.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
              {
                if( (item = htSearch(global_table, tmp_string)) == NULL )   //identifikator nenalezen ani v glob. tab. symbolu
                {
                  chyba = ERR_SEM1;   //nedeklarovana promenna
                  fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                }
              }
              if( chyba == ERR_NONE )
              {
                op1 = item->data->p_uni.variable;  //první operand bude ID
              }
              free(tmp_string);
            }
            else if( tokenTmp.type == tt_full_id )  //jedna se o full identifikator
            {
              if( (item = htSearch(global_table, tokenTmp.string)) != NULL )
              {
                op1 = item->data->p_uni.variable;  //první operand bude Full.ID
              }
              else
              {
                chyba = ERR_SEM1;   //nedeklarovana promenna
                fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
              }
            }
            else if( tokenTmp.type == tt_int || tokenTmp.type == tt_double || tokenTmp.type == tt_literal )
            {
              Class *new = Create_Const_to_Table(&tokenTmp);  //získáme hodnotu literál
              
              Token tokenKey;
              initToken(&tokenKey);
              Create_Token_string( &tokenKey, "const" );
              
              if( Err == ERR_NONE )
              {
                htInsert ( tmp_table, tokenKey.string, new );  //vložíme do pomocné TS
                item = htSearch( tmp_table, tokenKey.string );
                op1 = item->data->p_uni.variable;     //první operand bude literál
              }
              clearToken(&tokenKey);
            }
            else if( tokenTmp.type == tt_bracket_left )
            {
              bracket = false;  //pokud je nalezena levá závorka, je upravena pomocná proměnná bracket
            }
            else if( tokenTmp.type == tt_empty )
            {
              if( (item = htSearch(tmp_table, tokenTmp.string)) != NULL )
              {
                op1 = item->data->p_uni.variable;  //první operand bude již dříve vytvořený result
              }
              else
              {
                chyba = ERR_SEM1;   //nedeklarovana promenna
                fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
              }
            }
            
            if( tokenBracket.string != NULL )
            {
              clearToken(&tokenTmp);
              SemStackPop(&sem_stack);      //pop ( ze zasobniku
              SemStackPush(&sem_stack, &tokenBracket); //push zredukovaný result uložený v pomocném tokenu
                            
              clearToken(&tokenBracket);
              operator = I_NOP;
            }
            else
            {
              Class *new = Create_Var_to_Table();  //vytvoření nové proměnné
              Token tokenKey;
              initToken(&tokenKey);
              Create_Token_string( &tokenKey, "result" );
                
              if( Err == ERR_NONE )
              {
                htInsert ( tmp_table, tokenKey.string, new );  //vložení nové proměnné do pomocné TS
                item = htSearch( tmp_table, tokenKey.string );
                result = item->data->p_uni.variable;    //pro result je nyní vyhrazena paměť a nastaven ukazatel
              }

              //generování instrukce
              if( operator != I_NOP )
              {
                if( tokenStatic.keyword == key_static )
                {
                  insertTab( instr_table, "CLASS", operator, op1, op2, result, NULL);  //na úrovni třídy
                }
                else     //na úrovni funkce
                {
                  char *akt_label = NULL;
                  int TypeFun;
                  topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
                  insertTab( instr_table, akt_label, operator, op1, op2, result, NULL);
                }
              }
              //řešení konstrukce if+else
              if( tokenJump.keyword == key_if && START_IF == true 
              && ( operator == I_LESS || operator == I_LEQUAL || operator == I_GREATER || operator == I_GEQUAL || operator == I_SAME || operator == I_NOT_EQUAL) )
              {
                char *akt_label = NULL;
                int TypeFun;
                topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
                
                if( tokenWhile.keyword == key_while )
                {
                  IF_IN_WHILE = IF_IN_WHILE + 2;  //zvýšíme si pomocnou proměnou pro if v cyklu
                }
                START_IF = false;   //nastavíme opět na false
                char *jump1;
                jump1 = Create_Label_string( jump1, "if" );  //vytvoříme label pro if
                
                char *jump2;    
                jump2 = Get_Else_number( IF );   //nastavíme příslušné else if0->else0
                JumpStackPush( &jump_stack, IF );  //nastavíme si hodnotu if na pomocný stack
                
                //generujeme příslušné instrukce
                insertTab( instr_table, akt_label, I_TJUMP, result, NULL, NULL, jump1);
                insertTab( instr_table, akt_label, I_FJUMP, result, NULL, NULL, jump2);
                
                pushStackLabel( &label_stack, jump1, -1);  //push label na zásobník labelů
                free(jump1);
                free(jump2);
                
                clearToken(&tokenTmp);
                SemStackPop(&sem_stack);   //pop operandu ze zasobniku
              }
              //řešení cyklu while
              else if( tokenWhile.keyword == key_while && START_WHILE == true 
              && ( operator == I_LESS || operator == I_LEQUAL || operator == I_GREATER || operator == I_GEQUAL || operator == I_SAME || operator == I_NOT_EQUAL) )
              {
                char *akt_label = NULL;
                int TypeFun;
                topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
                
                WHILE_CLEAR++;     //zvýšíme proměnnou potřebnou pro indikaci vnořených cyklů
                START_WHILE = false;  //nastavíme na původní false
                
                char *jump;
                jump = Create_Label_string( jump, "while" );
                
                //generování instrukce
                insertTab( instr_table, akt_label, I_WJUMP, NULL, NULL, NULL, jump);
                
                pushStackLabel( &label_stack, jump, -1);  //pushneme label pro while
                
                char *akt_label2 = NULL;
                topStackLabel( &label_stack, &akt_label2, &TypeFun, NULL, NULL );
                
                insertTab( instr_table, akt_label2, operator, op1, op2, result, NULL);  //vyhodnocení výrazu
                insertTab( instr_table, akt_label2, I_WFJUMP, result, NULL, NULL, jump);  //skok pro ukončení cyklu při neplatné podmínce
                
                free(jump);
                
                clearToken(&tokenTmp);
                SemStackPop(&sem_stack);   //pop operandu ze zasobniku
              } 
              else
              { 
                clearToken(&tokenTmp);
                SemStackPop(&sem_stack);   //pop operandu ze zasobniku
                SemStackPush(&sem_stack, &tokenKey);    //push result 
              }
              clearToken(&tokenKey);
            }  
            start_redukce = false;  //nastavíme start redukce na původní false
            //operator = I_NOP;
                        
          }  //endif( start_redukce == 1 )
        } //endif( OPERACE == 1 )
        
        if( sem_stack.count == 1 && konec_redukce == true )  //pouze pokud nám zůstal na pomocném sem. zásobníku jediný prvek, a redukce je úspěšně syntakticky ukončena
        {
          tHTItem* item;
          char *tmp_string;
          
          if( tokenReturn.keyword == key_return )
          {
            char *akt_label = NULL;
            int TypeFun;
            topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
            
            if( TypeFun == 0 )  //pokud se jedná o void funkci a za returnem je výraz
            {
              chyba = ERR_SEM2;
              fprintf(stderr, "Chyba. Řádek %d : Očekáván return;\n", token.line);
            }
            
            if( tokenFun.type == tt_identifier )  //jedna se o jednoduchy identifikator funkce
            {
              tmp_string = add_full_ID( tokenFun.string, AKT_TRIDA->class_name );
            }
            
            tHTItem *item2;
            
            if( Err == ERR_NONE )
            {
                if( (item2 = htSearch( tmp_table, tmp_string)) != NULL)
                  value_return = item2->data->p_uni.variable;
            }
            value_return = item2->data->p_uni.variable;  //najdeme si item v pomocné TS pro return vytvořený v prvním průchodu
            
            //generování instrukce
            if( operator != I_NOP )
            {
              insertTab( instr_table, akt_label, operator, op1, op2, value_return, NULL);   //získáme výsledek poslední operace
            }
            insertTab( instr_table, akt_label, I_RETURN, value_return, NULL, NULL, NULL);  //generujeme instrukci pro return
            
            SemStackPop(&sem_stack);
            if( !SemStackEmpty(&sem_stack) )
            {
              chyba = ERR_SEM3;
              fprintf(stderr, "Chyba. Řádek %d : Problém při sémantických akcích výrazů!\n", token.line);
            }
            
            clearToken(&tokenReturn);
            free(tmp_string);
          }
          
          else if( tokenResult.type != tt_empty )
          {
            if( tokenResult.type == tt_identifier )  //jedna se o jednoduchy identifikator
            {
              tmp_string = add_full_ID( tokenResult.string, AKT_TRIDA->class_name );   //promenna je v gTS vlozena pod klicem Class.ID
              if( (item = htSearch(TS_Array[position_in_arr - 1], tokenResult.string)) == NULL )  //promenna nenalezena v lok. tabulce symbolu
              {
                if( (item = htSearch(global_table, tmp_string)) == NULL )   //identifikator nenalezen ani v glob. tab. symbolu
                {
                  chyba = ERR_SEM1;   //nedeklarovana promenna
                  fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
                }
              }
              if( chyba == ERR_NONE )
              {
                op1 = item->data->p_uni.variable;  //op1 bude L-operand
                
                if( !SemStackEmpty(&sem_stack) )
                {
                  SemStackTop( &sem_stack, &tokenTmp);
                  
                  item = htSearch( tmp_table, tokenTmp.string );
                  op2 = item->data->p_uni.variable;   //op2 bude druhý operand, který bude přiřazen - výsledek po poslední redukci
                  clearToken(&tokenTmp);
                }

                if( tokenStatic.keyword == key_static )
                {
                  insertTab( instr_table, "CLASS", I_ASSIGN, op1, op2, NULL, NULL);  //na úrovni třídy
                  clearToken(&tokenStatic);
                }
                else  //na úrovni funkce
                {
                  char *akt_label = NULL;
                  int TypeFun;
                  topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
                  insertTab( instr_table, akt_label, I_ASSIGN, op1, op2, NULL, NULL);
                }       
              }
              free(tmp_string);
            }
            else if( tokenResult.type == tt_full_id )  //jedna se o full identifikator
            {
              if( (item = htSearch(global_table, tokenResult.string)) != NULL )
              {
                op1 = item->data->p_uni.variable;   //op1 bude L-operand
                
                if( !SemStackEmpty(&sem_stack) )
                {
                  SemStackTop( &sem_stack, &tokenTmp);
                  
                  item = htSearch( tmp_table, tokenTmp.string );
                  op2 = item->data->p_uni.variable;     //op2 bude druhý operand, který bude přiřazen - výsledek po poslední redukci
                  clearToken(&tokenTmp);
                }

                if( tokenStatic.keyword == key_static )
                {
                  insertTab( instr_table, "CLASS", I_ASSIGN, op1, op2, NULL, NULL);   //na úrovni třídy
                  clearToken(&tokenStatic);
                }
                else    //na úrovni funkce
                {
                  char *akt_label = NULL;
                  int TypeFun;
                  topStackLabel( &label_stack, &akt_label, &TypeFun, NULL, NULL );
                  insertTab( instr_table, akt_label, I_ASSIGN, op1, op2, NULL, NULL);
                } 
              }
              
              else
              {
                chyba = ERR_SEM1;   //nedeklarovana promenna
                fprintf(stderr, "Chyba. Řádek %d : Pokus o přístup k nedeklarované proměnné.\n", token.line);
              }
            }
            
            SemStackPop(&sem_stack);
            if( !SemStackEmpty(&sem_stack) )
            {
              chyba = ERR_SEM3;
              fprintf(stderr, "Chyba. Řádek %d : Problém při sémantických akcích výrazů!\n", token.line);
            }
            clearToken(&tokenResult);
          }
          else
          {            
            SemStackPop(&sem_stack);   //pop operandu ze zasobniku
            if( !SemStackEmpty(&sem_stack) )
            {
              chyba = ERR_SEM3;
              fprintf(stderr, "Chyba. Řádek %d : Problém při sémantických akcích výrazů!\n", token.line);
            }
          }
          
          if( chyba != ERR_NONE )
          { 
            break;
          }          
        }
        operator = I_NOP;
      } //endif( ROUND == 2 )
      
      //end semant
    }
//******************************************************************************************************
    else if(op == M) // shiftuj
    {
      if(precStackEmpty(&s) == 1)
      {
        chyba = ERR_SYN;
        break;
      }
      precStackTop(&s, &itmTmp);
      while(itmTmp == SV_NETERM) // pop prvky ze zasobniku dokud nenarazíš na terminál a popující prvky ukladej do pomocného zásobníku
      {
        precStackPush(&stmp, itmTmp);
        precStackPop(&s);
        if(precStackEmpty(&s) == 1)
        {
          chyba = ERR_SYN;
          break;
        }
        precStackTop(&s, &itmTmp);
      }
      if(chyba != ERR_NONE)
      {
        break;
      }
      
      //semant
      if( ROUND == 2 )
      {
        if( OPERACE == true )  //tímto vyloučíme jednoduché přiřazení, to je řešeno shora dolů
        {
          SemStackPush( &sem_stack, &token);  //pushujeme si tokeny na sémantický zásobník
        }
      }
      //end semant
      
      precStackPush(&s, SV_SHIFTUJ); // push znak SHIFTUJ
      while(precStackEmpty(&stmp) != 1) // vracej prvky které se poply 
      {
        precStackTop(&stmp, &itmTmp);
        precStackPush(&s, itmTmp);
        precStackPop(&stmp);
      }
      precStackPush(&s, itm); // push nový prvek

      tokenQueRemove(q);
      if(tokenQueEmpty(q) == 1) // získaní nového tokenu, ale pokude už žadný není syntax chyba!
      {
        if(itm == SV_DOLLAR)
        {
          chyba = ERR_SYN;
          break;
        }
        itm = SV_DOLLAR;
        
        //semant
        konec_redukce = true;
      }
      else
      {
        tokenQueFront(q, &token); // pošli další token
      }
    }
//******************************************************************************************************
    else if(op == R) // rovno -> pouze pushni prvek na zasobník
    {     
      precStackPush(&s, itm);

      if(Err != ERR_NONE) // v případě že v předchozích akcí (push) došlo k chybě konec
      {
        chyba = Err;
        break;
      }
      
      //semant
      if( ROUND == 2 )
      {
        if( OPERACE == true )
        {
          SemStackPush( &sem_stack, &token);
        }
      }
      //end semant
      
      tokenQueRemove(q);
      if(tokenQueEmpty(q) == 1) // získaní nového tokenu, ale pokude už žadný není syntax chyba!
      {
        if(itm == SV_DOLLAR)
        {
          chyba = ERR_SYN;
          break;
        }
        itm = SV_DOLLAR;
        
        //semant
        konec_redukce = true;
      }
      else
      {
        tokenQueFront(q, &token); // pošli další token
      }
      
    } // end if
    if(Err != ERR_NONE) // v případě že v předchozích akcí (push) došlo k chybě konec
    {
      chyba = Err;
      break;
    }
  }

  // free
  while(precStackEmpty(&s) != 1)
  {
     precStackPop(&s);
  }
  while(precStackEmpty(&stmp) != 1)
  {
     precStackPop(&stmp);
  }
  while(SemStackEmpty(&sem_stack) != 1)
  {
    SemStackPop(&sem_stack);
  }
  while(tokenQueEmpty(q) != 1)
  {
    tokenQueRemove(q);
  }
  
  return chyba;
}
