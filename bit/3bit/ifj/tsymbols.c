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
 *  Andrej Čulen  (xculen02)
 *  
 * Odkaz na repozitář:
 *  https://github.com/xbedna62/IFJ16
 *
 * Modul:
 *  tsymbol.c (funkce pro zjednodušení práce s tabulkou symbolů)
 *
 * Autor: 
 *  Vít Ambrož (xambro15)
 *
 *****/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h" 
#include "tsymbols.h"
#include "scanner.h"


int INDEX = 0;  //proměnná je využita při vytváření nových dat o proměnných

int IF = 0;  //proměnné jsou využity při vytváření nových LABELU pro skoky
int WHILE = 0;

//vrací typ podle tokenu
type_variable get_Type ( KeywordType key_type )
{
  type_variable pom;
  
  switch(key_type)
  {
    case key_int:
      pom = variable_integer;
      break;

    case key_String:
      pom = variable_string;
      break;

    case key_double:
      pom = variable_double;
      break;

    case key_boolean:
      pom = variable_boolean;
      break;
      
    case key_void:
      pom = variable_void;
      break;

    default:
      Err = ERR_OTHER;
  }
  
  return pom;
}

//funkce pro získání jména class, ve které se aktuálně nacházíme
Class* get_ClassName ( char *name )
{
  Class* trida;
  if( (trida = (Class*) malloc( sizeof(Class) ))  == NULL )
  {
    Err = ERR_OTHER;   //nepovedna alokace
    return trida;
  }
  
  if( (trida->class_name = (char*) malloc((sizeof(char)) * strlen(name)+1)) == NULL)
  {
    Err = ERR_OTHER;  //nepovedna alokace
    return trida;
  }
  
  strcpy( trida->class_name, name );
  return trida;
}


//**********Funkce pro uvolňování dat z tab. symbolů a z Class******************

//funkce pro správné uvolnění paměti
void free_ClassName( Class* data )
{
  if( data != NULL)
  {
    if( data->class_name != NULL)
    {
      free(data->class_name);
      data->class_name = NULL;
    }
    free(data);
    data = NULL;
  }
}

//funkce pro zjednoduseni korektniho uvolneni dat z tab. symbolu
void free_Data( Class* data )
{
  if( data->type == 0 ) //t_function
  {
    if( data->p_uni.function->name != NULL )
    {
      free(data->p_uni.function->name);
      data->p_uni.function->name = NULL;
    }
    if( data->p_uni.function->Params != NULL )
    {
      int i;
      for( i=0; i < data->p_uni.function->n_params; i++)
      {
        free(data->p_uni.function->Params[i].name);
        data->p_uni.function->Params[i].name = NULL;
      }
      free(data->p_uni.function->Params);
      data->p_uni.function->Params = NULL;
    }
    if( data->p_uni.function != NULL )
    {
      free(data->p_uni.function);
      data->p_uni.function = NULL;
    }
    if( data != NULL )
    {
      free(data);
      data = NULL;
    }
  }
  else if( data->type == 1 ) //t_variable
  {
    if( data != NULL )
    {
            if( data->p_uni.variable != NULL )
            { 
                if( data->p_uni.variable->name != NULL )
                {
                    free(data->p_uni.variable->name);
                    data->p_uni.variable->name = NULL;
                }
                if( data->p_uni.variable->string_init != 0  )
                { 
                    free(data->p_uni.variable->uni_value.value_string);
                    data->p_uni.variable->uni_value.value_string = NULL;
                }
                free(data->p_uni.variable);
                data->p_uni.variable = NULL;
            }
            free(data);
            data = NULL;
        }
  }
  else if( data->type == 2 )  //t_class
  {
    if( data != NULL)
    {
      if( data->class_name != NULL)
      {
        free(data->class_name);
        data->class_name = NULL;
      }
      free(data);
      data = NULL;
    }
  }
}


//**************Funkce pro práci s Variable Data****************

//získání dat při definici nové proměnné
Class* get_VarData ( Token* tok, char *var_name )
{
  Class* var;
  
  if( (var = malloc( sizeof(Class) ) ) == NULL )
  {
    Err = ERR_OTHER;
    return var;
  }
  var->class_name = NULL;
  
  if( (var->p_uni.variable = malloc( sizeof(Variable_Data) ) ) == NULL )
  {
    Err = ERR_OTHER;
    return var;
  }
  
  if( (var->p_uni.variable->name = (char*) malloc((sizeof(char))*(strlen(var_name)+1) )) == NULL)
  {
    Err = ERR_OTHER;  //nepovedna alokace
    return var;
  }
  strcpy(var->p_uni.variable->name, var_name);
  
  var->type = t_variable;
  var->p_uni.variable->uni_value.value_string = NULL;  //?
  var->p_uni.variable->inicialized = 0;
  var->p_uni.variable->string_init = 0;
  var->p_uni.variable->var_type = get_Type( tok->keyword );

  return var;
}

//pomocná funkce pro zkopírování dat proměnné
void copy_VarData ( Variable_Data *dst, Variable_Data *src )
{
    if( (dst = (Variable_Data*) malloc( sizeof(Variable_Data)) ) == NULL )
    {
        Err = ERR_OTHER;
        return;
    }
    
    dst->string_init = src->string_init;
    dst->inicialized = src->inicialized;
    dst->var_type = src->var_type;
    
    if( (dst->name = (char*) malloc( (strlen(src->name) + 1) * sizeof(char)) ) == NULL )
    {
        Err = ERR_OTHER;
        return;
    }
    strcpy(dst->name, src->name); 
    
    if( dst->var_type == variable_integer )
        dst->uni_value.value_integer = src->uni_value.value_integer;
    if( dst->var_type == variable_double )
        dst->uni_value.value_double = src->uni_value.value_double;
    if( dst->string_init == 1 )
    {
        if( (dst->uni_value.value_string = (char*) malloc( (strlen(src->uni_value.value_string) + 1) * (sizeof(char) ) ) ) == NULL )
        {
            Err = ERR_OTHER;
        return;
        } 
        strcpy(dst->uni_value.value_string, src->uni_value.value_string);
    }
}

//pomocná funkce pro vytvoření konstanty, která se bude vkládat do tabulky symbolů
Class* Create_Const_to_Table( Token *src)
{
    Class *new;
    if( (new = (Class*) malloc( sizeof(Class) ))  == NULL )
    {
        Err = ERR_OTHER;
        return new;
    }
    new->type = t_variable;
    new->class_name = NULL;
    
    if( (new->p_uni.variable = (Variable_Data*) malloc( sizeof(Variable_Data) ))  == NULL )
    {
        Err = ERR_OTHER;
        return new;
    }
    new->p_uni.variable->name = NULL;
    new->p_uni.variable->string_init = 0;
    new->p_uni.variable->uni_value.value_string = NULL; 
    
    get_VarValue_Term( new->p_uni.variable, src );
    
    return new;
}

//pomocná funkce pro vytvoření dat, které se budou vkládat do tabulky symbolů
Class* Create_Var_to_Table()
{
    Class *new;
    if( (new = (Class*) malloc( sizeof(Class) ))  == NULL )
    {
        Err = ERR_OTHER;
        return new;
    }
    new->type = t_variable;
    new->class_name = NULL;
    
    if( (new->p_uni.variable = (Variable_Data*) malloc( sizeof(Variable_Data) ))  == NULL )
    {
        Err = ERR_OTHER;
        return new;
    }
    new->p_uni.variable->name = NULL;
    new->p_uni.variable->string_init = 0;
    new->p_uni.variable->inicialized = 0;
    
    new->p_uni.variable->uni_value.value_string = NULL;
    
    return new;
}


//pomocná funkce pro získání hodnoty proměnné
void get_VarValue_Term( Variable_Data *var, Token *tok )
{
  switch( tok->type )
  {
    case tt_literal:
      if( (var->uni_value.value_string = (char*) malloc( (tok->cap) * (sizeof(char) ) ) ) == NULL )
      {
        Err = ERR_OTHER;
        return;
      } 
      strcpy(var->uni_value.value_string, tok->string);
      var->inicialized = 1;
      var->string_init = 1;
      var->var_type = variable_string;
      return;
    
    case tt_int:
      var->uni_value.value_integer = atoi(tok->string);
      var->inicialized = 1;
      var->string_init = 0;
      var->var_type = variable_integer;
      return;

    case tt_double:
      var->uni_value.value_double = atof(tok->string);
      var->inicialized = 1;
      var->string_init = 0;
      var->var_type = variable_double;
      return;
      
    default:
      Err = ERR_SEM2;
      return;
  }
}


//*******************Funkce pro práci s Function Data**********************

//získání dat při definici nové funkce
Class* get_FunData ( Token* tok, char *fun_name )
{
  Class* fun;
  
  if( (fun = malloc( sizeof(Class) ) ) == NULL )
  {
    Err = ERR_OTHER;
    return fun;
  }
  
  fun->class_name = NULL;
  
  if( (fun->p_uni.function = malloc( sizeof(Function_Data) ) ) == NULL )
  {
    Err = ERR_OTHER;
    return fun;
  }
  
  if( (fun->p_uni.function->name = (char*) malloc((sizeof(char))*(strlen(fun_name)+1) )) == NULL)
  {
    Err = ERR_OTHER;  //nepovedna alokace
    return fun;
  }
  
  strcpy(fun->p_uni.function->name, fun_name);
  fun->type = t_function;
  fun->p_uni.function->n_params = 0;
  fun->p_uni.function->cap = 0;
  fun->p_uni.function->Params = NULL;
  fun->p_uni.function->returnType = get_Type( tok->keyword );

  return fun;
}

//funkce pro zjednodušení přidávání parametrů funkce
void add_Params ( Function_Data *fun, KeywordType key_type, char *param_name )
{
  if( fun->n_params == 0 && fun->cap == 0 )
  {
    if( (fun->Params = (function_params*) malloc( CAP_CONST * sizeof(function_params) ) ) == NULL )
    {
      Err = ERR_OTHER;    //alokace pameti pro 4 argumenty
      return;
    }
    fun->cap = CAP_CONST;
  }
  
  (fun->n_params)++;
  
  if( fun->cap <= fun->n_params )   //realokujeme vice pameti pro dalsi 4 argumenty
  {
    fun->cap += CAP_CONST;
    fun->Params = realloc( fun->Params, (fun->cap * sizeof(function_params) ) ); 
  }
  
  fun->Params[fun->n_params - 1].param_type = get_Type(key_type);  //ziskani dat. typu parametru
  
  if( ( fun->Params[fun->n_params - 1].name = (char*) malloc( (strlen(param_name)+1) * sizeof(char) ) ) == NULL )
  {
    Err = ERR_OTHER;
    return;
  }
  
  strcpy( fun->Params[fun->n_params - 1].name, param_name );   //ziskani jmena identifikatoru parametru
}

//funkce zjištuje, zda sedí typ argumentu volané funkce s typem parametru při jeho definici
ErrorType check_argument_type( Class *data, Token tok, int index )
{
  ErrorType chyba = ERR_NONE;
  
  if( tok.type == tt_int )
  {
    if( data->p_uni.function->Params[index].param_type != variable_integer )
    {
      chyba = ERR_SEM2;  //tento typ nesedi podle definice
    }
  }
  if( tok.type == tt_double )
  {
    if( data->p_uni.function->Params[index].param_type != variable_double )
    {
      chyba = ERR_SEM2;  //tento typ nesedi podle definice
    }
  }
  if( tok.type == tt_literal )
  {
    if( data->p_uni.function->Params[index].param_type != variable_string )
    {
      chyba = ERR_SEM2;  //tento typ nesedi podle definice
    }
  }
  
  return chyba;
}



//************Funkce pro práci s řetězci, pro upravení stringu před vložením do tab. symbolů******************

//pomocná funkce pro vytvoření nového tokenu, kde bude uložen speciálně vytvořený string
void Create_Token_string( Token *new, char *key )
{   
    new->cap = strlen(key) + MAX_STRING_SIZE;
    if( (new->string = (char*) malloc( sizeof(char) * new->cap) ) == NULL )
    {
        Err = ERR_OTHER;
        return;
    }
    
    char *buffer = malloc(sizeof(char) * (MAX_STRING_SIZE-1) );    //zde se vejde 0-9999

    sprintf(buffer, "%d", INDEX);
    
    strcpy( new->string, buffer);
    strcat( new->string, key );

    free(buffer);
    INDEX++;
}

//pomocná funkce pro získání příslušného else pro skokovou instrukci
char* Get_Else_number( int number )
{
  char *new;
  if( (new = (char*) malloc( sizeof(char) * (strlen("else") + MAX_STRING_SIZE) ) ) == NULL )
  {
    Err = ERR_OTHER;
    return new;
  }
  char *buffer = malloc(sizeof(char) * (MAX_STRING_SIZE-1) );    //zde se vejde 0-9999
  sprintf(buffer, "%d", number);
  
  strcpy( new, buffer);
  strcat( new, "else" );
  free(buffer);
  
  return new;
}

//pomocná funkce pro vytvoření nového jména pro label, kde bude uložen speciálně vytvořený string
char* Create_Label_string( char *new, char *key )
{
  if( (new = (char*) malloc( sizeof(char) * (strlen(key) + MAX_STRING_SIZE) ) ) == NULL )
  {
    Err = ERR_OTHER;
    return new;
  }  
  
  char *buffer = malloc(sizeof(char) * (MAX_STRING_SIZE-1) );    //zde se vejde 0-9999
  
  if( strcmp( key, "if" ) == 0 )
  {
    IF++;
    sprintf(buffer, "%d", IF);
  }
  else if( strcmp( key, "while" ) == 0 )
  {
    sprintf(buffer, "%d", WHILE);
    WHILE++;
  } 
  else
  {
    sprintf(buffer, "%d", INDEX);
    INDEX++;
  }
  
  strcpy( new, buffer);
  strcat( new, key );
  free(buffer);
      
  return new;
}

//pomocná funkce pro vytvoření labelu funkce Main.run, na vstup se očekává Main, připojí se ".run"
char* Create_MainRun_Label( char *new, char *key )
{
  int lenght = strlen(key) + 5;  //.run\0  je celkem 5 znaků
  
  if( (new = (char*) malloc( sizeof(char) * lenght ) ) == NULL )
  {
    Err = ERR_OTHER;
    return new;
  }
  
  memset(new, 0, lenght);
    
  int i = 0;
  while( key[i] != '\0' )
  {
    new[i] = key[i];
    i++;
  }
  new[i] = '.'; 
  new[i+1] = 'r';
  new[i+2] = 'u';
  new[i+3] = 'n';
  new[i+4] = '\0';
  
  return new;   
}

//funkce pro zmenu klice na full_ID pred vlozenim do glob. tab. symbolů 
char* add_full_ID( char *id, char *class )
{
  char *key;
  int lenght = strlen(id) + strlen(class) + 2; //2 navic pro '.' a '\0'
  if( (key = (char*) malloc( (sizeof(char)) * (lenght) )) == NULL)  
  {
    Err = ERR_OTHER;  //nepovedna alokace, navratova hodnota
    return key;
  }
  memset(key, 0, lenght);
  
  int i = 0;
  while( class[i] != '\0' )
  {
    key[i] = class[i];
    i++;
  }
  key[i] = '.';
  i++;
  
  int j = 0;
  while( id[j] != '\0' )
  {
    key[i] = id[j];
    j++;
    i++;
  }
  key[i] = '\0';

  return key;        //vysledny klic bdue ve formatu "full.id"
}

//funkce pro zmenu klice Class pred vlozenim do glob. tab. symbolů 
char* add_char_ID( char *id, char znak )
{
  char *key;
  int lenght = strlen(id) + 3; //3 navic pro ':' 'znak' '\0'
  if( (key = (char*) malloc( (sizeof(char)) * (lenght) )) == NULL)  //dva pridavane znaky + koncovy znak
  {
    Err = ERR_OTHER;  //nepovedna alokace, navratova hodnota
    return key;
  }
  memset(key, 0, lenght);
  
  int i = 0;
  while( id[i] != '\0' )
  {
    key[i] = id[i];
    i++;
  }
  key[i] = ':';
  key[i+1] = znak;
  key[i+2] = '\0';

  return key;        //vysledny klic bdue ve formatu "Class:'znak'"
}

