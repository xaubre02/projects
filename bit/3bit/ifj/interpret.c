#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ial.h"
#include "interpret.h"

ErrorType interpret(struct tTabLabel *t)
{
  ErrorType chyba = ERR_NONE;
  struct tStackLabel s; // zásobník labelů (top mi říká z jakého labelu se mají vykonavat instrukce)
  char *top = NULL;
  initStackLabel(&s);
  pushStackLabel(&s, "Main.run", 0); // začíná se ve funkci Main.run, void funkce
 
  struct tListItem ins, tmp; // proměná pro uchování aktualní istrukce která se vykonává  
  initListItem(&ins);
  initListItem(&tmp);
  
  if(getItemTab(t, "CLASS", &tmp) != -1)
  {
	  backInListTab(t, "CLASS");
	  pushStackLabel(&s, "CLASS", 0); // začíná se s instrukcemi pro static proměnné, pouze pokud v labelu CLASS neco je	
  }
  clearListItem(&tmp);
  
  int end; // proměná pro uchování navratu funkce getItemTab

  int typeFun; // pomocná proměná pro zjištění v jakém typu jsem funkce -1 nic, 0 void, 1 norm. fce 

  int count; // proměná pro uchování informace kolik už bylo provedeno instrukcí na daném labelu

  // zjištění jestli jsou vůbec nějaké instrukce v Main.run
  if(getItemTab(t, "Main.run", &ins) == -1) // žádné instrukce v Main.run uspěšný konec
  {
    popStackLabel(&s);
    clearListItem(&ins);
    return ERR_NONE;
  }
  else // je co interpretovat
  {
    backInListTab(t, "Main.run"); // zpět na první instrukce v Main.run
    clearListItem(&ins);
  }

  while(1)
  {
    topStackLabel(&s, &top, &typeFun, &end, &count); // zjístí v kterém labelu jsme
    clearListItem(&ins);
    end = getItemTab(t, top, &ins);
    count++;
    actualizeStackLabel(&s, end, count); // aktualizace proměné end pro label na vrcholu zasobníku

    switch(ins.ins)
    {
      case I_MUL://----------------------------------------------------------------------------------------------------------------
            if(ins.op1->inicialized == 0 || ins.op2->inicialized == 0) // ošetření zda nepracujeme s neinicializovanýma proměnýma
            {
              chyba = ERR_RUN2;
              break;
            }
            // datové typy
            if(ins.op1->var_type == variable_integer && ins.op2->var_type == variable_integer) // int * int
            {
              ins.result->uni_value.value_integer = ins.op1->uni_value.value_integer * ins.op2->uni_value.value_integer;
              ins.result->var_type = variable_integer;
            }            
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_double) // double * double
            {
              ins.result->uni_value.value_double = ins.op1->uni_value.value_double * ins.op2->uni_value.value_double;
              ins.result->var_type = variable_double;
            }
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_integer) // double * int
            {
              ins.result->uni_value.value_double = ins.op1->uni_value.value_double * ins.op2->uni_value.value_integer;
              ins.result->var_type = variable_double;
            }
            else if(ins.op1->var_type == variable_integer && ins.op2->var_type == variable_double) // int * double
            {
              ins.result->uni_value.value_double = ins.op1->uni_value.value_integer * ins.op2->uni_value.value_double;
              ins.result->var_type = variable_double;
            }
            else // semantická chyba špatné typy operandů
            {
              chyba = ERR_SEM2;
              break;
            }  
            ins.result->inicialized = 1;          
            break;
      case I_DIV://----------------------------------------------------------------------------------------------------------------
            if(ins.op1->inicialized == 0 || ins.op2->inicialized == 0) // ošetření zda nepracujeme s neinicializovanýma proměnýma
            {
              chyba = ERR_RUN2;
              break;
            }
             // ošetření dělení nulou
            if(ins.op2->var_type == variable_integer)
            {
              if(ins.op2->uni_value.value_integer == 0)
              {
                chyba = ERR_RUN3;
                break;
              }
            }
            else if(ins.op2->var_type == variable_double)
            {
              if(ins.op2->uni_value.value_double == 0.0)
              {
                chyba = ERR_RUN3;
                break;
              }
            }
            // datové typy
            if(ins.op1->var_type == variable_integer && ins.op2->var_type == variable_integer) // int / int
            {
              ins.result->uni_value.value_integer = ins.op1->uni_value.value_integer / ins.op2->uni_value.value_integer;
              ins.result->var_type = variable_integer;
            }            
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_double) // double / double
            {
              ins.result->uni_value.value_double = ins.op1->uni_value.value_double / ins.op2->uni_value.value_double;
              ins.result->var_type = variable_double;
            }
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_integer) // double / int
            {
              ins.result->uni_value.value_double = ins.op1->uni_value.value_double / ins.op2->uni_value.value_integer;
              ins.result->var_type = variable_double;
            }
            else if(ins.op1->var_type == variable_integer && ins.op2->var_type == variable_double) // int / double
            {
              ins.result->uni_value.value_double = ins.op1->uni_value.value_integer / ins.op2->uni_value.value_double;
              ins.result->var_type = variable_double;
            }
            else // semantická chyba špatné typy operandů
            {
              chyba = ERR_SEM2;
              break;
            }  
            ins.result->inicialized = 1;
            break;
      case I_PLUS://----------------------------------------------------------------------------------------------------------------
            if(ins.op1->inicialized == 0 || ins.op2->inicialized == 0) // ošetření zda nepracujeme s neinicializovanýma proměnýma
            {
              chyba = ERR_RUN2;
              break;
            }
            // datové typy
            if(ins.op1->var_type == variable_integer && ins.op2->var_type == variable_integer) // int + int
            {
              ins.result->uni_value.value_integer = ins.op1->uni_value.value_integer + ins.op2->uni_value.value_integer;
              ins.result->var_type = variable_integer;
            }            
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_double) // double + double
            {
              ins.result->uni_value.value_double = ins.op1->uni_value.value_double + ins.op2->uni_value.value_double;
              ins.result->var_type = variable_double;
            }
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_integer) // double + int
            {
              ins.result->uni_value.value_double = ins.op1->uni_value.value_double + ins.op2->uni_value.value_integer;
              ins.result->var_type = variable_double;
            }
            else if(ins.op1->var_type == variable_integer && ins.op2->var_type == variable_double) // int + double
            {
              ins.result->uni_value.value_double = ins.op1->uni_value.value_integer + ins.op2->uni_value.value_double;
              ins.result->var_type = variable_double;
            }
            else if(ins.op1->var_type == variable_string && ins.op2->var_type == variable_string) // string + string (konkatenace)
            {
              if( ins.op1->string_init == 0 || ins.op2->string_init == 0 )
              {
				  chyba = ERR_RUN2;
				  break;
			  }
              char *tmpString = malloc(sizeof(char)*(strlen(ins.op1->uni_value.value_string)+strlen(ins.op2->uni_value.value_string)+1));
              if(tmpString == NULL)
              {
                chyba = ERR_OTHER;
                break;
              }
              tmpString[0] = '\0';
              strcat(tmpString, ins.op1->uni_value.value_string);
              strcat(tmpString, ins.op2->uni_value.value_string);
              if(ins.result->string_init != 0 && ins.result->var_type == variable_string) // pokud už v minulosti byla proměná result použita a nahodou pro string tak uvolním string
              {
                free(ins.result->uni_value.value_string);
              }
              ins.result->string_init = 1;
              ins.result->var_type = variable_string;
              ins.result->uni_value.value_string = tmpString;
            }
            else // semantická chyba špatné typy operandů
            {
              chyba = ERR_SEM2;
              break;
            }  
            ins.result->inicialized = 1;
            break;
      case I_MINUS://----------------------------------------------------------------------------------------------------------------
            if(ins.op1->inicialized == 0 || ins.op2->inicialized == 0) // ošetření zda nepracujeme s neinicializovanýma proměnýma
            {
              chyba = ERR_RUN2;
              break;
            }
            // datové typy
            if(ins.op1->var_type == variable_integer && ins.op2->var_type == variable_integer) // int - int
            {
              ins.result->uni_value.value_integer = ins.op1->uni_value.value_integer - ins.op2->uni_value.value_integer;
              ins.result->var_type = variable_integer;
            }            
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_double) // double - double
            {
              ins.result->uni_value.value_double = ins.op1->uni_value.value_double - ins.op2->uni_value.value_double;
              ins.result->var_type = variable_double;
            }
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_integer) // double - int
            {
              ins.result->uni_value.value_double = ins.op1->uni_value.value_double - ins.op2->uni_value.value_integer;
              ins.result->var_type = variable_double;
            }
            else if(ins.op1->var_type == variable_integer && ins.op2->var_type == variable_double) // int - double
            {
              ins.result->uni_value.value_double = ins.op1->uni_value.value_integer - ins.op2->uni_value.value_double;
              ins.result->var_type = variable_double;
            }
            else // semantická chyba špatné typy operandů
            {
              chyba = ERR_SEM2;
              break;
            }  
            ins.result->inicialized = 1;
            break;
      case I_ASSIGN://----------------------------------------------------------------------------------------------------------------  
            if(ins.op2->inicialized == 0) // ošetření zda nepracujeme s neinicializovanýma proměnýma
			{
              chyba = ERR_RUN2;
              break;
            }
            if(ins.op1->var_type == variable_integer && ins.op2->var_type == variable_integer) // int = int
            {
              ins.op1->uni_value.value_integer = ins.op2->uni_value.value_integer;
            }
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_integer) // double = int
            {
              ins.op1->uni_value.value_double = ins.op2->uni_value.value_integer;
            }
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_double) // double = double
            {
              ins.op1->uni_value.value_double = ins.op2->uni_value.value_double;
            }
            else if(ins.op1->var_type == variable_string && ins.op2->var_type == variable_string) // string = string
            {
              char *tmpString = malloc(sizeof(char)*(strlen(ins.op2->uni_value.value_string)+1));
              if(tmpString == NULL)
              {
                chyba = ERR_OTHER;
                break;
              }
              strcpy(tmpString, ins.op2->uni_value.value_string);
              if( ins.op1->string_init != 0 || ins.op1->uni_value.value_string != NULL) // pokud je nějaký string v operandu do kterého přiřazujem uvolníme ho
              {
                free(ins.op1->uni_value.value_string);
              }
              ins.op1->uni_value.value_string = tmpString;
              ins.op1->string_init = 1;
            }
            else
            {
              chyba = ERR_SEM2;
              break;
            }  
            ins.op1->inicialized = 1;
            break;
      case I_LESS://------------------------------------------------------------------------------------------------------------------
            if(ins.op1->inicialized == 0 || ins.op2->inicialized == 0) // ošetření zda nepracujeme s neinicializovanýma proměnýma
            {
              chyba = ERR_RUN2;
              break;
            }
            // datové typy
            if(ins.op1->var_type == variable_integer && ins.op2->var_type == variable_integer) // int < int
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_integer < ins.op2->uni_value.value_integer;
              ins.result->var_type = variable_boolean;
            }            
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_double) // double < double
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_double < ins.op2->uni_value.value_double;
              ins.result->var_type = variable_boolean;
            }
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_integer) // double < int
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_double < ins.op2->uni_value.value_integer;
              ins.result->var_type = variable_boolean;
            }
            else if(ins.op1->var_type == variable_integer && ins.op2->var_type == variable_double) // int < double
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_integer < ins.op2->uni_value.value_double;
              ins.result->var_type = variable_boolean;
            }
            else // semantická chyba špatné typy operandů
            {
              chyba = ERR_SEM2;
              break;
            }  
            ins.result->inicialized = 1; 
            break;
      case I_GREATER://---------------------------------------------------------------------------------------------------------------
            if(ins.op1->inicialized == 0 || ins.op2->inicialized == 0) // ošetření zda nepracujeme s neinicializovanýma proměnýma
            {
              chyba = ERR_RUN2;
              break;
            }
            // datové typy
            if(ins.op1->var_type == variable_integer && ins.op2->var_type == variable_integer) // int > int
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_integer > ins.op2->uni_value.value_integer;
              ins.result->var_type = variable_boolean;
            }            
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_double) // double > double
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_double > ins.op2->uni_value.value_double;
              ins.result->var_type = variable_boolean;
            }
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_integer) // double > int
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_double > ins.op2->uni_value.value_integer;
              ins.result->var_type = variable_boolean;
            }
            else if(ins.op1->var_type == variable_integer && ins.op2->var_type == variable_double) // int > double
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_integer > ins.op2->uni_value.value_double;
              ins.result->var_type = variable_boolean;
            }
            else // semantická chyba špatné typy operandů
            {
              chyba = ERR_SEM2;
              break;
            }  
            ins.result->inicialized = 1; 
            break;
      case I_LEQUAL://----------------------------------------------------------------------------------------------------------------
            if(ins.op1->inicialized == 0 || ins.op2->inicialized == 0) // ošetření zda nepracujeme s neinicializovanýma proměnýma
            {
              chyba = ERR_RUN2;
              break;
            }
            // datové typy
            if(ins.op1->var_type == variable_integer && ins.op2->var_type == variable_integer) // int <= int
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_integer <= ins.op2->uni_value.value_integer;
              ins.result->var_type = variable_boolean;
            }            
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_double) // double <= double
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_double <= ins.op2->uni_value.value_double;
              ins.result->var_type = variable_boolean;
            }
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_integer) // double <= int
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_double <= ins.op2->uni_value.value_integer;
              ins.result->var_type = variable_boolean;
            }
            else if(ins.op1->var_type == variable_integer && ins.op2->var_type == variable_double) // int <= double
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_integer <= ins.op2->uni_value.value_double;
              ins.result->var_type = variable_boolean;
            }
            else // semantická chyba špatné typy operandů
            {
              chyba = ERR_SEM2;
              break;
            }  
            ins.result->inicialized = 1; 
            break;      
      case I_GEQUAL://----------------------------------------------------------------------------------------------------------------
            if(ins.op1->inicialized == 0 || ins.op2->inicialized == 0) // ošetření zda nepracujeme s neinicializovanýma proměnýma
            {
              chyba = ERR_RUN2;
              break;
            }
            // datové typy
            if(ins.op1->var_type == variable_integer && ins.op2->var_type == variable_integer) // int >= int
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_integer >= ins.op2->uni_value.value_integer;
              ins.result->var_type = variable_boolean;
            }            
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_double) // double >= double
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_double >= ins.op2->uni_value.value_double;
              ins.result->var_type = variable_boolean;
            }
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_integer) // double >= int
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_double >= ins.op2->uni_value.value_integer;
              ins.result->var_type = variable_boolean;
            }
            else if(ins.op1->var_type == variable_integer && ins.op2->var_type == variable_double) // int >= double
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_integer >= ins.op2->uni_value.value_double;
              ins.result->var_type = variable_boolean;
            }
            else // semantická chyba špatné typy operandů
            {
              chyba = ERR_SEM2;
              break;
            }  
            ins.result->inicialized = 1; 
            break;      
      case I_SAME://------------------------------------------------------------------------------------------------------------------
            if(ins.op1->inicialized == 0 || ins.op2->inicialized == 0) // ošetření zda nepracujeme s neinicializovanýma proměnýma
            {
              chyba = ERR_RUN2;
              break;
            }
            // datové typy
            if(ins.op1->var_type == variable_integer && ins.op2->var_type == variable_integer) // int == int
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_integer == ins.op2->uni_value.value_integer;
              ins.result->var_type = variable_boolean;
            }            
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_double) // double == double
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_double == ins.op2->uni_value.value_double;
              ins.result->var_type = variable_boolean;
            }
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_integer) // double == int
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_double == ins.op2->uni_value.value_integer;
              ins.result->var_type = variable_boolean;
            }
            else if(ins.op1->var_type == variable_integer && ins.op2->var_type == variable_double) // int == double
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_integer == ins.op2->uni_value.value_double;
              ins.result->var_type = variable_boolean;
            }
            else // semantická chyba špatné typy operandů
            {
              chyba = ERR_SEM2;
              break;
            }  
            ins.result->inicialized = 1; 
            break;
      case I_NOT_EQUAL://-------------------------------------------------------------------------------------------------------------
            if(ins.op1->inicialized == 0 || ins.op2->inicialized == 0) // ošetření zda nepracujeme s neinicializovanýma proměnýma
            {
              chyba = ERR_RUN2;
              break;
            }
            // datové typy
            if(ins.op1->var_type == variable_integer && ins.op2->var_type == variable_integer) // int != int
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_integer != ins.op2->uni_value.value_integer;
              ins.result->var_type = variable_boolean;
            }            
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_double) // double != double
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_double != ins.op2->uni_value.value_double;
              ins.result->var_type = variable_boolean;
            }
            else if(ins.op1->var_type == variable_double && ins.op2->var_type == variable_integer) // double != int
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_double != ins.op2->uni_value.value_integer;
              ins.result->var_type = variable_boolean;
            }
            else if(ins.op1->var_type == variable_integer && ins.op2->var_type == variable_double) // int != double
            {
              ins.result->uni_value.value_boolean = ins.op1->uni_value.value_integer != ins.op2->uni_value.value_double;
              ins.result->var_type = variable_boolean;
            }
            else // semantická chyba špatné typy operandů
            {
              chyba = ERR_SEM2;
              break;
            }  
            ins.result->inicialized = 1; 
            break;
      case I_READ_INT:
            if(scanf("%d", &(ins.result->uni_value.value_integer)) != 1)
            {
              chyba = ERR_RUN1;
              break;
            }
            while(getchar() != '\n');
            ins.result->var_type = variable_integer;
            ins.result->inicialized = 1;
            break;
      case I_READ_DOUBLE:
            if(scanf("%lf", &(ins.result->uni_value.value_double)) != 1)
            {
              chyba = ERR_RUN1;
              break;
            }
            while(getchar() != '\n');
            ins.result->var_type = variable_double;
            ins.result->inicialized = 1;
            break;
      case I_READ_STRING:
            if(ins.result->uni_value.value_string != NULL && ins.result->var_type == variable_string) // pokud je nějaký string v operandu do kterého načítámw nejdřív ho uvolníme
            {
              free(ins.result->uni_value.value_string);
            }
            ins.result->uni_value.value_string = malloc(sizeof(char)*5);
            if(ins.result->uni_value.value_string == NULL)
            {
              chyba = ERR_OTHER;
              break;
            }
            int cap = 5;
            int len = 0;
            char znak;
            while((znak = getchar()) != '\n')
            {
              if(len == cap)
              {
                ins.result->uni_value.value_string = realloc(ins.result->uni_value.value_string, sizeof(char)*(cap+5));
                if(ins.result->uni_value.value_string == NULL)
                {
                  chyba = ERR_OTHER;
                  break;
                }
                cap += 5;
              }
              ins.result->uni_value.value_string[len] = znak;
              len++;
            }
            if(len == cap) // není místo na ukončovací znak řetězce
            {
              ins.result->uni_value.value_string = realloc(ins.result->uni_value.value_string, sizeof(char)*(cap+1));
              if(ins.result->uni_value.value_string == NULL)
              {
                chyba = ERR_OTHER;
                break;
              }
            }
            ins.result->uni_value.value_string[len] = '\0';
            ins.result->var_type = variable_string;
            ins.result->inicialized = 1;
            ins.result->string_init = 1;
            break;
      case I_PRINT:
            if(ins.op1->inicialized == 0) // test inicializace
            {
              chyba = ERR_RUN2;
              break;
            }
            if(ins.op1->var_type == variable_integer) // print(int)
            {
              printf("%d", ins.op1->uni_value.value_integer);
            }
            else if(ins.op1->var_type == variable_double) // print(double)
            {
              printf("%g", ins.op1->uni_value.value_double);
            }
            else if(ins.op1->var_type == variable_string) // print(string)
            {
              printf("%s", ins.op1->uni_value.value_string);
            }
            break;
      case I_LENGTH:
            if(ins.op1->inicialized == 0) // test inicializace
            {
              chyba = ERR_RUN2;
              break;
            }
            if(ins.op1->var_type == variable_string) // může být provedeno jen pro string
            {
              ins.result->uni_value.value_integer = strlen(ins.op1->uni_value.value_string);
              ins.result->var_type = variable_integer;
              ins.result->inicialized = 1;
            }
            else // chyba datových typů
            {
              chyba = ERR_SEM2;
              break;
            }
            break;
      case I_SUBSTR1:
            if(ins.op1->inicialized == 0 || ins.op2->inicialized == 0) // test inicializace
            {
              chyba = ERR_RUN2;
              break;
            }
            if(ins.op1->var_type == variable_string && ins.op2->var_type == variable_integer) // substr(string, int, int) zatím dělám jen první dva parametry
            {
              if( (int)strlen(ins.op1->uni_value.value_string) < ins.op2->uni_value.value_integer) // délka řetezce je menší než index od ktereho se má kopírovat
              {
                chyba = ERR_RUN4;
                break;
              }
              char *tmpString = malloc(sizeof(char)*(strlen(ins.op1->uni_value.value_string)+1-ins.op2->uni_value.value_integer));
              if(tmpString == NULL)
              {
                chyba = ERR_OTHER;
                break;
              }
              strcpy(tmpString, ins.op1->uni_value.value_string + ins.op2->uni_value.value_integer);
              if(ins.result->uni_value.value_string != NULL && ins.result->var_type == variable_string)
              {
                free(ins.result->uni_value.value_string);
              }
              ins.result->uni_value.value_string = tmpString;
              ins.result->var_type = variable_string;
              ins.result->inicialized = 1;
              ins.result->string_init = 1;
            }
            else 
            {
              chyba = ERR_SEM2;
              break;
            }
            break;
      case I_SUBSTR2:
            if(ins.op1->inicialized == 0 || ins.op2->inicialized == 0) // test inicializace
            {
              chyba = ERR_RUN2;
              break;
            }
            if(ins.op1->var_type == variable_string && ins.op2->var_type == variable_integer) // substr(string, int, int) zatím dělám jen první dva parametry
            {
              if( (int)strlen(ins.op1->uni_value.value_string) < ins.op2->uni_value.value_integer) // délka řetezce je menší než počet znaků co se má zkopírovat
              {
                chyba = ERR_RUN4;
                break;
              }
              char *tmpString = malloc(sizeof(char)*((ins.op2->uni_value.value_integer+1)));
              if(tmpString == NULL)
              {
                chyba = ERR_OTHER;
                break;
              }
              strncpy(tmpString, ins.op1->uni_value.value_string, ins.op2->uni_value.value_integer);
              tmpString[ins.op2->uni_value.value_integer] = '\0';
              if(ins.result->uni_value.value_string != NULL && ins.result->var_type == variable_string)
              {
                free(ins.result->uni_value.value_string);
              }
              ins.result->uni_value.value_string = tmpString;
              ins.result->var_type = variable_string;
              ins.result->inicialized = 1;
              ins.result->string_init = 1;
            }
            else 
            {
              chyba = ERR_SEM2;
              break;
            }
            break;
      case I_COMPARE:
            if(ins.op1->inicialized == 0 || ins.op2->inicialized == 0) // test inicializace
            {
              chyba = ERR_RUN2;
              break;
            }
            if(ins.op1->var_type == variable_string && ins.op2->var_type == variable_string) // pouze string compare string
            {
              int rescmp = strcmp(ins.op1->uni_value.value_string, ins.op2->uni_value.value_string);
              if(rescmp == 0)
              {
                ins.result->uni_value.value_integer = 0;
              }
              else if(rescmp < 0)
              {
                ins.result->uni_value.value_integer = -1;
              }
              else if(rescmp > 0)
              {
                ins.result->uni_value.value_integer = 1;
              }
              ins.result->var_type = variable_integer;
              ins.result->inicialized = 1;
            }
            else
            {
              chyba = ERR_SEM2;
              break;
            }
            break;
      case I_FIND:
            if(ins.op1->inicialized == 0 || ins.op2->inicialized == 0) // test inicializace
            {
              chyba = ERR_RUN2;
              break;
            }
            if(ins.op1->var_type == variable_string && ins.op2->var_type == variable_string) // pouze string find string
            {
              if(strlen(ins.op2->uni_value.value_string) == 0) // hledá se v řetezci prázdný řetezec
              {
                ins.result->uni_value.value_integer = 0;
              }
              else
              {
                ins.result->uni_value.value_integer = KMPMatch(ins.op1->uni_value.value_string, strlen(ins.op1->uni_value.value_string), ins.op2->uni_value.value_string, strlen(ins.op2->uni_value.value_string));
              }
              ins.result->var_type = variable_integer;
              ins.result->inicialized = 1;
            }
            else
            {
              chyba = ERR_SEM2;
              break;
            }
            break;
      case I_SORT:
            if(ins.op1->inicialized == 0) // test inicializace
            {
              chyba = ERR_RUN2;
              break;
            }
            if(ins.op1->var_type == variable_string) // pouze sort(string)
            {
              char *tmpString = malloc(sizeof(char)*(strlen(ins.op1->uni_value.value_string)+1));
              if(tmpString == NULL)
              {
                chyba = ERR_OTHER;
                break;
              }
              strcpy(tmpString, ins.op1->uni_value.value_string);
              ShellSort(tmpString);
              if(ins.result->uni_value.value_string != NULL && ins.result->var_type == variable_string) // pokud je nějaký string v operandu do kterého vložíme string nejdřív ho uvolníme
              {
                free(ins.result->uni_value.value_string);
              }
              ins.result->uni_value.value_string = tmpString;
              ins.result->var_type = variable_string;
              ins.result->inicialized = 1;
              ins.result->string_init = 1;
            }
            else
            {
              chyba = ERR_SEM2;
              break;
            }
            break;
      case I_TJUMP:
            if(ins.op1->var_type == variable_boolean) // podmínka musí byt boolean
            {
              if(ins.op1->uni_value.value_boolean == 1) // jen když je cond true něco dělej
              {               
                if(getItemTab(t, ins.label, &tmp) == -1) // prázdný if -> if{}else
                {
                  clearListItem(&ins);
                  end = getItemTab(t, top, &ins); // přeskočení instrukce FJUMP vždy nasleduje při IF ELSE
                  actualizeStackLabel(&s, end, count+1);
                }
                else // něco je v ifu tudiž vykonej co tam je
                {
                  backInListTab(t, ins.label); // zpět na první instrukce v ifu
                  clearListItem(&tmp);
                  actualizeStackLabel(&s, getItemTab(t, top, &tmp), count+1); // přeskočení instrukce FJUMP vždy nasleduje při IF ELSE
                  pushStackLabel(&s, ins.label, -1);
                  firstInListTab(t, ins.label); // začínám na první instrukci větve if
                }
                clearListItem(&tmp);
              }
            }            
            break;
      case I_FJUMP:
            if(ins.op1->var_type == variable_boolean) // podmínka musí byt boolean
            {
              if(ins.op1->uni_value.value_boolean == 0) // jen když je cond true něco dělej
              {
                if(getItemTab(t, ins.label, &tmp) != -1) // nějaké instrukce v else jsou -> vykonej je
                {
                  backInListTab(t, ins.label); // zpět na první instrukce v else
                  pushStackLabel(&s, ins.label, -1);
                  topStackLabel(&s, &top, &typeFun, &end, &count);
                  firstInListTab(t, top); // začinám na první instrukci větve else 
                }
                clearListItem(&tmp);
              }
            }           
            break;
      case I_WJUMP:
          end = 0;
          if(strcmp(ins.label, top) != 0) // v případě že se skáče z while zpět na začatek stejného while nic se neděje
          {
            pushStackLabel(&s, ins.label, -1);
            topStackLabel(&s, &top, &typeFun, NULL, &count);
            firstInListTab(t, top); // začínám na první insrtukci while
          }
          break;
      case I_WFJUMP: // podmínka skoku pryč pro while
          end = 0;
          if(ins.op1->var_type == variable_boolean) // podmínka musí byt boolean
          {
            if(ins.op1->uni_value.value_boolean == 0) // když je podmínka ve while false konec while cyklu
            {
              popStackLabel(&s);
              topStackLabel(&s, &top, &typeFun, &end, &count);
              firstInListTab(t, top); // zpět na první instrukci
              for(int i = 0; i < count; i++) // přeskočení tolik instrukcí kolk již bylo vykonáno
              {
                getItemTab(t, top, &tmp);
                clearListItem(&tmp);
              }
            }
          }    
          break;
      case I_FCEJUMP:
          if(getItemTab(t, ins.label, &tmp) != -1) // test zda jsou nějaké instrukce ve funkci
          {
            backInListTab(t, ins.label); // zpět na první instrukce ve funkci
            pushStackLabel(&s, ins.label, 1);
            topStackLabel(&s, &top, &typeFun, &end, &count);
            firstInListTab(t, top); // zpět na první instrukci
          }
          else // žádné instrukce v norm. funkci chyba nebyl vykonán return
          {
            chyba = ERR_RUN2;
          }
          clearListItem(&tmp);
          break;
      case I_FCEVJUMP:
          if(getItemTab(t, ins.label, &tmp) != -1) // test zda jsou nějaké instrukce ve funkci
          {
            backInListTab(t, ins.label); // zpět na první instrukce ve funkci
            pushStackLabel(&s, ins.label, 0);
            topStackLabel(&s, &top, &typeFun, &end, &count);
            firstInListTab(t, top); // zpět na první instrukci
          }
          clearListItem(&tmp);
          break;
      case I_RETURN:
          while(typeFun == -1) // když je return v if nebo while tak se vynářím z labelů dokud není label funkce
          {
            firstInListTab(t, top); // zpět na začátek instrukcí daného labelu if nebo while

            popStackLabel(&s);
            topStackLabel(&s, &top, &typeFun, &end, &count);
          }
          if(typeFun == 0) // void funkce končí
          {
            if(ins.op1->inicialized == 1) // pokud je navratova hodnota inicializovana chybu u void funkce
            {
              chyba = ERR_RUN2;
              break;
            }
          }
          else if(typeFun == 1) // normalní funkce
          {
            if(ins.op1->inicialized == 0) // pokud je navratova hodnota neinicializovana chyba u norm. funkce
            {
              chyba = ERR_RUN2;
              break;
            }            
          }
          firstInListTab(t, top); // zpět na začátek instrukcí dané funkce připraveno pro další volání této funkce
  
          popStackLabel(&s);
          if(emptyStackLabel(&s) == 1) // když je return v Main zásobník je prázdný -> vrátím Main.run zpět a do end dám 1 -> úspěšný konec
          {
            pushStackLabel(&s, "Main.run", 0);
            topStackLabel(&s, &top, &typeFun, NULL, NULL);
            end = 1;
          }
          else
          {
            topStackLabel(&s, &top, &typeFun, &end, &count);
            firstInListTab(t, top); // zpět na první instrukci
            for(int i = 0; i < count; i++) // přeskočení tolik instrukcé kolik již bylo vykonáno na daném labelu
            {
              getItemTab(t, top, &tmp);
              clearListItem(&tmp);
            }
          }
          break;
    }
  
    if(chyba != ERR_NONE)
    {
      break;
    }    
    if(end == 1) // byla provedena poslední instrukce na daném labelu
    {
      if(strcmp(top, "Main.run") == 0) // konec programu
      {
        chyba = ERR_NONE;
        break;
      }
      else
      {
        while(end == 1) // popuji dokud nenajdu label s end = 0 (jsou zde ještě instrukce)
        {
          if(typeFun == 1) // pro norm. funkci nebyl vykonán return chyba!
          {
            chyba = ERR_RUN2;
            break;
          }

          if(strcmp(top, "Main.run") == 0) // dopopovali jsem se až k Main.run a už není žádná další instrukce v ní -> konec
          {
            break;
          }

          popStackLabel(&s);
          topStackLabel(&s, &top, &typeFun, &end, &count);
          firstInListTab(t, top); // zpět na první instrukci
          for(int i = 0; i < count; i++) // přeskočení tolik instrukcé kolik již bylo vykonáno na daném labelu
          {
            getItemTab(t, top, &tmp);
            clearListItem(&tmp);
          }
        }
        if(strcmp(top, "Main.run") == 0 && end == 1)
        {
          chyba = ERR_NONE;
          break;
        }
        if(chyba != ERR_NONE)
        {
          break;
        }
      }
    }   
  }
  //free
  clearListItem(&ins);
  while(emptyStackLabel(&s) != 1)
  {
    popStackLabel(&s);
  }
  return chyba;
}
