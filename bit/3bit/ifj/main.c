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
 *  main.c
 *****/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"
#include "parser.h"
#include "error.h"

int main(int argc, char *argv[])
{
    if(argc != 2){
        fprintf(stderr, "Bad Params...\n");
        return 99;
    }

    openFile(argv[1]);
    if(Err != ERR_NONE) // v případě že se něco pokazilo při otevírání souboru
        return checkError();

    Err = parser_twoRounds();
    
    return checkError();
}
