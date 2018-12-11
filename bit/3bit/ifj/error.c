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
 *  Error
 *
 * Autor: 
 *  Tomáš Aubrecht (xaubre02)
 *
 *****/


#include "error.h"

#include <stdio.h>

ErrorType Err = ERR_NONE;

int checkError(void)
{
    switch(Err)
    {
        case ERR_NONE:
        {
            //printf("Success!\n");
            return ERR_NONE;
        }
        case ERR_LEX:
        {
            fprintf(stderr, "Error! Scanner did not exit successfully.\n");
            return ERR_LEX;
        }
        case ERR_SYN:
        {
            fprintf(stderr, "Error! Syntactic analysis did not exit successfully.\n");
            return ERR_SYN;
        }
        case ERR_SEM1:
        {
            fprintf(stderr, "Error! Semantic analysis error (Nondefined function, variable, class or Predefined function, varibale, class).\n");
            return ERR_SEM1;
        }
        case ERR_SEM2:
        {
            fprintf(stderr, "Error! Semantic analysis error (Type compatibility of arithmetical, string, relational expression or wrong type, number of parameters in function).\n");
            return ERR_SEM2;
        }
        case ERR_SEM3:
        {
            fprintf(stderr, "Error! Semantic analysis error (Other).\n");
            return ERR_SEM3;
        }
        case ERR_RUN1:
        {
            fprintf(stderr, "Error! Running error (Loading number from input).\n");
            return ERR_RUN1;
        }
        case ERR_RUN2:
        {
            fprintf(stderr, "Error! Running error (Working with uninitialized variable).\n");
            return ERR_RUN2;
        }
        case ERR_RUN3:
        {
            fprintf(stderr, "Error! Running error (Division by zero).\n");
            return ERR_RUN3;
        }
        case ERR_RUN4:
        {
            fprintf(stderr, "Error! Running error (Other).\n");
            return ERR_RUN4;
        }
        case ERR_OTHER:
        {
            fprintf(stderr, "Error! Internal error of interpret (Alloc error, file error, ...).\n");
            return ERR_OTHER;
        }
    }
}
