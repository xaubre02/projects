#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>

////////////////////TISK POPISU//////////////////////////
int printDes(int argc, char* argv[])
{
    if(argc>1 && argv[1]!=NULL)         //pokud je zadan neprazdny argument
    {
        printf("//////////////////////////////////////////////////\n"
               "*** Autor:   Tomas Aubrecht (xaubre02)\n"
               "*** Datum:   24.10.2015\n"
               "*** Projekt c. 1: Prace s textem\n\n"
               "Jedna se o program na analyzu vstupnich dat.\n"
               "//////////////////////////////////////////////////\n");
    exit(EXIT_SUCCESS);
    }
    return 0;
}
/////////////////////////////////////////////////////////


////////////////////DETEKCE DATA/////////////////////////
 int isDate(char input[101])
{
    if(isdigit(input[0]) && isdigit(input[1]) && isdigit(input[2]) &&           //detekce formatu DDDD-DD-DD, kde D je cislice
       isdigit(input[3]) && input[4]=='-' && isdigit(input[5]) &&
       isdigit(input[6]) && input[7]=='-' && isdigit(input[8]) && isdigit(input[9]) && input[10]=='\0')
        return 1;
    else
        return 0;
}
/////////////////////////////////////////////////////////


////////////////////ZPRACOVANI DATA//////////////////////
int dateTest(char input[101])
{
    char mesic[3];              //preneseni ze vstupu
        mesic[0]=input[5];
        mesic[1]=input[6];

    char den[3];
        den[0]=input[8];
        den[1]=input[9];

    int year  = atoi(input);    //prevod charu na int
    int month = atoi(mesic);
    int day   = atoi(den);


    struct tm time_str;
    char daybuf[20];

    time_str.tm_year =year-1900;
    time_str.tm_mon = month-1;
    time_str.tm_mday = day;
    time_str.tm_hour = 0;
    time_str.tm_min = 0;
    time_str.tm_sec = 1;
    time_str.tm_isdst = -1;

    if (mktime(&time_str) == -1 || day>31 || month>12 || day==0 || month==0)        //podminka pro platne datum
        printf("word: %s\n",input);
    else
    {
        strftime(daybuf, sizeof(daybuf), "%a", &time_str);
        printf("date: %s %s\n",daybuf, input);
    }
  return 0;

}
/////////////////////////////////////////////////////////


////////////////////DETEKCE CISLA////////////////////////
int isNumber (char input[101])
{
    bool number=false;

    if(input[0]=='+' && input[1]=='\0')           //pokud je na vstupu pouze "+"
        number=false;

    else if(input[0]=='+' || isdigit(input[0]))   //pokud je na vstupu kladne cislo (+3)
    {
        number=true;
        for(int i =1; input[i]!='\0'; i++)        //projizdi cely retezec, zdali jsou vsechny znaky cislice
        {
            if (isdigit(input[i]))
            {
                number=true;
            }
            else
            {
                number=false;
                break;
            }
        }
    }
    if(number)
        return 1;
    else
        return 0;
}
/////////////////////////////////////////////////////////


////////////////////DETEKCE PRVOCISLA////////////////////
int primeTest(char input [101])
{
    bool Prime;
    char* chyba;
    long i = strtol(input, &chyba, 10);
    if(i<=INT_MAX)
    {
         if(i==2)                //pokud je vstupni cislo i rovno 2, tak je to prvocislo
            Prime=true;
         if(i<=1)     //pokud je vstupni cislo i mensi nebo rovno 1 a zaroven vetsi nez INT_MAX,
            Prime=false;        //tak to neni prvocislo
        for(int x=2; x<i; x++)
        {
            if(i%x==0)          //zjisteni, zdali je vstupni cislo i delitelne cislem x <2, i-1> beze zbytku
            {                   //pokud je, tak to neni prvocislo
                Prime=false;    // pokud neni, tak to je prvocislo
                break;
            }
            else
                Prime=true;
        }
        if(Prime)                                   //tisk vysledku na vystup
            printf("number: %ld (Prime)\n", i);
        else
            printf("number: %ld \n",i);
    }
    else
        printf("number: %s\n", input);
    return 0;
}
/////////////////////////////////////////////////////////


////////////////////DETEKCE PALINDROMU///////////////////
int palindromeTest(char input[101])
{
    bool Palindrome;
    int velikost =0;
                                        //zjistime si velikost slova, abychom znali pozici posledniho znaku
    for(int y=0;input[y]!='\0';y++)     //a mohli bychom ho porovnat s prvnim
    {
        velikost++;
    }

    int front = 0;
    int back = velikost - 1;

    while(front <= back)                //v cyklu porovnavame prvni a posledni znak
    {
        char frontChar = input[front];
        char backChar = input[back];

        if(frontChar != backChar)       //pokud se nerovnaji, nejedna se o palindrom
        {                               //pokud se rovnaji, pokracuje se dal
            Palindrome=false;           //a porovnava se posledni a predposledni znak
            break;
        }
        else
        {
            Palindrome=true;
        }
        front++;
        back--;
    }

    if(Palindrome)                      //tisk vysledku na vystup
        printf("word: %s (Palindrome)\n", input);
    else
        printf("word: %s\n", input);

    return 0;
}
/////////////////////////////////////////////////////////


////////////////////HLAVNI PROGRAM///////////////////////

int main(int argc, char* argv[])
{
    printDes(argc, argv);                   //overi, zdali jsou na vstupu argumenty

    char input [101];
    int code = scanf("%100s", input);
    while (code==1)                         //skenuje, dokud na vstupu neco je
    {
        if(isNumber(input))                 //pokud se jedna o cislo, otestuje ho, jestli je prvocislo
            primeTest(input);

        else if(isDate(input))              //pokud se jedna o datum, otestuje, zdali je to platne datum a provede vypocet dne v tydnu
            dateTest(input);

        else                                //vsechno ostatni jsou slova, u kterych je proveden test, zdali je to palindrom
             palindromeTest(input);

        code = scanf("%100s", input);
    }
return 0;
}
/////////////////////////////////////////////////////////
