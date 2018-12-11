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
 *  Andrej Čulen    (xculen02)
 *  
 * Odkaz na repozitář:
 *  https://github.com/xbedna62/IFJ16
 *
 * Modul:
 *  tsymbol.h (funkce pro zjednodušení práce s tabulkou symbolů)
 *
 * Autor: 
 *  Vít Ambrož (xambro15)
 *
 *****/

#ifndef _TSYMBOLS_H_
#define _TSYMBOLS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "scanner.h"

extern int IF;

//**********************************************************************************************************
//struktury pro udaje o promenne
typedef enum {
    variable_boolean,
    variable_string,
    variable_integer,
    variable_double,
    variable_void     //void muze byt pouze u funkce
} type_variable;      //mozne datove typy promennych
    
typedef struct sVariable_Data {
    type_variable var_type;     //typ promenne
    int string_init;        //pomocna informace o tom, zda bude promenna obsahovat string, pro korektni free
    union{
        int value_boolean;
        char* value_string;
        int value_integer;
        double value_double;
    } uni_value;             //mozne hodnoty promennych
    int inicialized;        //je promenna inicializovana(1 ini/ 0 neini)
    char *name;             //jmeno promenne
} Variable_Data;



//**********************************************************************************************************
//struktury pro udaje o funkci
typedef struct s_function_params{
    type_variable param_type;
    char *name;
} function_params;      //udaje pro nazev parametru a jejich typy

typedef struct sFunction_Data {
    int n_params;               //pocet parametru funkce
    int cap;                    //informace o tom, pro kolik argumentu je aktualne alokovane misto
    function_params *Params;    //udaje o parametrech
    type_variable returnType;   //typ navratove hodnoty
    int position_arr;           //index, na kterem bude vytvořena lok. tab. symb pro danou funkci
    char *name;                 //jmeno funkce
} Function_Data;

//konstanta pro realokaci kapacity pro argumenty funkce
#define CAP_CONST 4

//**********************************************************************************************************
//struktury pro zjednodušení prvku hashovaci tabulky

typedef enum {
    t_function,
    t_variable,
    t_class
} type_item;   //umozni ulozit info o tom, zda se jedna o funkci nebo promenou, nasledne je vyuzito unionu

typedef struct s_Class{
    type_item type;           //určuje, zda se jedná o proměnou nebo o funkci, nebo o třídu
    union {
        struct sVariable_Data *variable;
        struct sFunction_Data *function;
    } p_uni;
    char *class_name;
} Class;   //used to be AllData


//vrací typ podle tokenu
type_variable get_Type ( KeywordType key_type );
//funkce pro získání jména class, ve které se aktuálně nacházíme
Class* get_ClassName ( char *name );


//**********Funkce pro uvolňování dat z tab. symbolů a z Class******************
void free_ClassName( Class* data );
//funkce pro zjednoduseni korektniho uvolneni dat z tab. symbolu
void free_Data( Class* data );


//**************Funkce pro práci s Variable Data****************
//získání dat při definici nové proměnné
Class* get_VarData ( Token* tok , char *name );
//pomocná funkce pro zkopírování dat proměnné
void copy_VarData ( Variable_Data *dst, Variable_Data *src );
//pomocná funkce pro vytvoření konstanty, která se bude vkládat do tabulky symbolů
Class* Create_Const_to_Table( Token *src);
//pomocná funkce pro vytvoření dat, které se budou vkládat do tabulky symbolů
Class* Create_Var_to_Table();
//pomocná funkce pro získání hodnoty proměnné
void get_VarValue_Term( Variable_Data *var, Token *tok );


//*******************Funkce pro práci s Function Data**********************
//získání dat při definici nové funkce
Class* get_FunData ( Token* tok, char *fun_name );
//funkce pro zjednodušení přidávání parametrů funkce
void add_Params ( Function_Data *fun, KeywordType key_type, char *name );
//funkce zjištuje, zda sedí typ argumentu volané funkce s typem parametru při jeho definici
ErrorType check_argument_type( Class *data, Token tok, int index );


//************Funkce pro práci s řetězci, pro upravení stringu před vložením do tab. symbolů******************
#define MAX_STRING_SIZE 5

//pomocná funkce pro vytvoření nového tokenu, kde bude uložen speciálně vytvořený string
void Create_Token_string( Token *new, char *key );
//pomocná funkce pro získání příslušného else pro skokovou instrukci
char* Get_Else_number( int number );
//pomocná funkce pro vytvoření nového řetězce, kde bude uložen speciálně vytvořený string
char* Create_Label_string( char *new, char *key );
//pomocná funkce pro vytvoření labelu funkce Main.run, na vstup se očekává Main, připojí se ".run"
char* Create_MainRun_Label( char *new, char *key );
//funkce pro zmenu klice na full_ID pred vlozenim do glob. tab. symbolů 
char* add_full_ID( char *id, char *class );
//funkce pro zmenu klice Class pred vlozenim do glob. tab. symbolů 
char* add_char_ID( char *id, char znak );


#endif

