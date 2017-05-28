/* **********************************************************
 *  Datum: 12.11.2015                                       *
 *  Autor: Tomas Aubrecht (xaubre02)                        *
 *  Projekt #2 - Iteracni vypocty                           *
 *  Popis: Program provede vypocet prirozeneho logaritmu a  *
 *         poctu iteraci k zajisteni pozadovane presnosti.  *
 ********************************************************** */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <math.h>

#define TEST_INPUT(x,y) argc == x && strcmp(argv[1], y) != 0        ///zadefinovani kvuli prehlednosti

double taylor_log(double x, unsigned int n);        ///prototypy funkci
double cfrac_log(double x, unsigned int n);

/// ********************* Chybova hlaseni  ********************* /// OK
enum terrors                                                     /// tak udelal jsem si vycet chyb
{
    ERR_OK,
    ERR_INPUT,
    ERR_X,
    ERR_ITER,
    ERR_EPS,
    ERR_LIMIT
};
const char *EMSG[] =                                             /// tady zase vycet chybovych hlasek
{
    "OK",                                                                       ///OK
    "Neplatny vstup!",                                                          ///OK
    "Neplatna hodnota x/MIN/MAX!",                                              ///OK
    "Neplatny pocet iteraci!",                                                  ///OK
    "Neplatna presnost!",                                                       ///OK
    "Prekrocena mez datoveho typu!"                                             ///OK
};
/// ************************************************************ ///


/// ******************** Absolutni hodnota  ******************** /// OK
double myAbs(double x){                                          ///Vlastni funkce pro absolutni hodnotu z cisla
    return x>=0 ? x : -x;                                        ///Pouze menim znamenko, je-li to zaporne
}
/// ************************************************************ ///


/// ********************** Kontrola vstupu ********************* /// OK
int inputCheck(int argc, char* argv[],
               double x, double MIN, double MAX, double EPS, unsigned long n,
            char *chyba1, char *chyba2, char *chyba3)                                ///tady si vsemnozne kontroluji vstup
{
    if(argc>5 || argc<4 || (TEST_INPUT(4,"--log")) || (TEST_INPUT(5 ,"--iter")))     ///je-li neplatny vstup
        return ERR_INPUT;

    if((argc==4 && *chyba1 != '\0') || (argc==5 && (*chyba1 != '\0' || *chyba2 != '\0')))
        return ERR_X;                                                               ///je-li neplatna hodnota x/MIN/MAX

    if(argc==4 && (argv[3][0]=='-' || *chyba2 != '\0'))                             ///je-li neplatny pocet iteraci
        return ERR_ITER;

    if(argc==5 && (EPS<=0 || *chyba3 != '\0'))                                      ///je-li neplatna presnost
        return ERR_EPS;

    if((argc==5 &&((MIN > DBL_MAX && MIN != INFINITY) || (MAX > DBL_MAX && MAX != INFINITY) || EPS > DBL_MAX)) ||  (argc==4 && (n > UINT_MAX || (x>DBL_MAX && x != INFINITY))))
        return ERR_LIMIT;                                                           ///pretece-li datovy typ

    else
        return 0;                                                                   ///jinak vse v pohode
}
/// ************************************************************ ///


/// ********************* Zretezene zlomky ********************* /// OK
double cfrac_log(double x, unsigned int n)
{
    if(x==0)                            ///pokud x=0, tak to vraci -nekonecno
        return -INFINITY;
    else if(x<0)
        return NAN;
    else if(x==INFINITY)
        return INFINITY;
    else{
        double sum = 0;
        double z = (x-1)/(x+1);         ///vypocet 'z' ze vzorce z wiki
        double sq = z*z;                ///deklarace a inicializace druhe mocniny
        double l = 2 * n -1;            ///deklarace a inicializace licheho cisla

        while(n>1){
            n--;                        ///snizeni n - dale je potreba pretypovat uint na double, aby nepretekl
            sum=((double)n*(double)n*sq)/(l-sum);       ///druha mocnina n * z delene lichym cislem + predchozim vysledkem
            l-=2;                       ///inicializace predchoziho licheho cisla
        }
        sum = 2*z/(1-sum);              ///posledni krok - dokonceni vzorce
        return sum;
    }
}
/// ************************************************************ ///


/// ********************* Tayloruv polynom ********************* /// OK
double taylor_log(double x, unsigned int n)
{
    if(x==0)                        ///pokud x=0, tak to vraci -nekonecno
        return -INFINITY;
    else if(x<0)
        return NAN;
        else if(x==INFINITY)
        return INFINITY;
    else{
        double sum = 0;
        unsigned int i = 1;

        if(x >= 1.0){
            double j = (x-1)/x;     ///deklarace a inicializace citatele
            double k=j;             ///inicializace citatele prvniho clenu
            while(i <= n){
                sum = sum + k/i;    ///ulozeni dalsiho clenu k vysledku
                k = k * j;          ///zvyseni mocniny o 1
                i++;                ///zvyseni jmenovatele o 1
            }
            return sum;
        }
        else{
            double z = 1 - x;       ///vypocet 'z' ze Taylorova vzorce
            double sq = z;        ///deklarace a inicializace druhe mocniny
            double fr;              ///zlomek
            sum = 0;                 ///ulozeni prvniho clenu posloupnosti
            i = 1;
            while(i<=n){
                fr = sq/i;
                sq  = sq * z;       ///zvyseni mocniny o 1
                i++;                ///zvyseni jmenovatele o 1
                sum = sum - fr;     ///ulozeni castecneho vysledku
            }
            return sum;
        }
    }
}
/// ************************************************************ ///


///Bisekce neboli puleni intervalu.
///Funguje to tak, ze mam interval 0-1000 nebo u taylorova polynomu, ktery potrebuje vice iterace 0-10000.
///Vypocitam si median intervalu pomoci prvniho clenu first a posledniho clenu last -> (first+last)/2
///Pokud median odpovida poctu iteraci pro danou presnost, tak vypocet konci.
///Pokud je median vetsi nez potrebny pocet iteraci, nastavi se posledni clen intervalu /last/ na hodnotu medianu a vypocita se novy median.
///Pokud je median mensi nez potrebny pocet iteraci, nastavi se prvni clen intervalu /first/ na hodnotu medianu a vypocita se novy median.
///Takhle to pokracuje do doby, nez se nalezne spravny pocet iteraci.
///Pokud interval 0-1000 pripadne 0-10000 nestaci, zdvojnasobi se a pocita se dal. Dvojnasobit se muze vicekrat, dokud se nenalezne odpovidajici hodnota.


/// ******************** Bisekce pro zlomky ******************** /// OK
void bisekce_cfrac(double MIN, double MAX, double EPS)
{
    unsigned int first=0;
    unsigned int last=1000;
    unsigned int median1;
    unsigned int median2;

    if(MAX<MIN){
        double tmp = MAX;
        MAX=MIN;
        MIN=tmp;
    }

        while(1){
            median1=(first+last)/2;                                 ///nalezeni medianu intervalu

            if(myAbs(log(MIN) - cfrac_log(MIN, median1)) > EPS && first==last){  ///je-li pro dany median presnost nedostacujici a zaroven
                last*=2;                                                         ///se interval zmenil na dve stejna cisla, tak se zdvojnasobi
            }
            if(myAbs(log(MIN) - cfrac_log(MIN, median1)) == EPS || (first==last && myAbs(log(MIN) - cfrac_log(MIN, median1)) < EPS))
                break;

            else if(myAbs(log(MIN) - cfrac_log(MIN, median1)) < EPS){            ///je-li pro dany median presnost dostacujici, tak se
                last = median1;                                                  ///posune interval na firs-median1
            }

            else if (myAbs(log(MIN) - cfrac_log(MIN, median1)) > EPS){           ///je-li pro dany median presnost nedostacujici, tak se
                first = median1 + 1;                                             ///posune interval na median1-last
            }

            else                                                                 ///jinak se ukonci cyklus
                break;
        }

    first=0;                ///prenastaveni vychozi hodnoty
    last=1000;              ///prenastaveni vychozi hodnoty

        while(1){
            median2=(first+last)/2;

            if(myAbs(log(MAX) - cfrac_log(MAX, median2)) > EPS && first==last){
                last*=2;
            }
            if(myAbs(log(MAX) - cfrac_log(MAX, median2)) == EPS || (first==last && myAbs(log(MAX) - cfrac_log(MAX, median2)) < EPS))
                break;

            else if(myAbs(log(MAX) - cfrac_log(MAX, median2)) < EPS){
                last = median2;
            }

            else if (myAbs(log(MAX) - cfrac_log(MAX, median2)) > EPS){
                first = median2 + 1;
            }

            else
                break;
        }

    if(median1<median2)
        median1 = median2;

    if(MIN<0 && MAX<0){
        printf("    cf_log(%g) = %.12g\n", MIN, NAN);
        printf("    cf_log(%g) = %.12g\n", MAX, NAN);
    }
    else if(MIN==0 && MAX==0){
        printf("    cf_log(%g) = %.12g\n", MIN, -INFINITY);
        printf("    cf_log(%g) = %.12g\n", MAX, -INFINITY);
    }
    else if(MIN==INFINITY && MAX==INFINITY){
        printf("    cf_log(%g) = %.12g\n", MIN, INFINITY);
        printf("    cf_log(%g) = %.12g\n", MAX, INFINITY);
    }
    else if(MIN==1 && MAX==1){
        printf("    cf_log(%g) = 0\n", MIN);
        printf("    cf_log(%g) = 0\n", MAX);
    }

    else{
        printf("continued fraction iterations = %u\n", median1);
        printf("    cf_log(%g) = %.12g\n", MIN, MIN!=0? cfrac_log(MIN, median1) : -INFINITY);
        printf("    cf_log(%g) = %.12g\n", MAX, MAX!=0? cfrac_log(MAX, median1) : -INFINITY);
    }
}
/// ************************************************************ ///


/// ******************* Bisekce pro Taylora ******************** ///
void bisekce_taylor(double MIN, double MAX, double EPS)
{
    unsigned int first=0;                                        ///stejny princip jako u bisekci pro zretezeny zlomky
    unsigned int last=10000;
    unsigned int median1;
    unsigned int median2;

    if(MAX<MIN){
        double tmp = MAX;
        MAX=MIN;
        MIN=tmp;
    }
    while(1){
        median1=(first+last)/2;

        if(myAbs(log(MIN) - taylor_log(MIN, median1)) > EPS && first==last){
            last*=2;
        }
        if(myAbs(log(MIN) - taylor_log(MIN, median1)) == EPS || (first==last && myAbs(log(MIN) - taylor_log(MIN, median1)) < EPS))
            break;

        else if(myAbs(log(MIN) - taylor_log(MIN, median1)) < EPS){
            last = median1;
        }

        else if (myAbs(log(MIN) - taylor_log(MIN, median1)) > EPS){
            first = median1 + 1;
        }

        else
            break;
    }

    first=0;
    last=10000;

    while(1){
        median2=(first+last)/2;

        if(myAbs(log(MAX) - taylor_log(MAX, median2)) > EPS && first==last){
            last*=2;
        }
        if(myAbs(log(MAX) - taylor_log(MAX, median2)) == EPS || (first==last && myAbs(log(MAX) - taylor_log(MAX, median2)) < EPS))
            break;

        else if(myAbs(log(MAX) - taylor_log(MAX, median2)) < EPS){
            last = median2;
        }

        else if (myAbs(log(MAX) - taylor_log(MAX, median2)) > EPS){
            first = median2 + 1;
        }

        else
            break;
    }

    if(median1<median2)
        median1 = median2;

    if(MIN<0 && MAX<0){
        printf("taylor_log(%g) = %.12g\n", MIN, NAN);
        printf("taylor_log(%g) = %.12g\n", MAX, NAN);
    }
    else if(MIN==0 && MAX==0){
        printf("taylor_log(%g) = %.12g\n", MIN, -INFINITY);
        printf("taylor_log(%g) = %.12g\n", MAX, -INFINITY);
    }
    else if(MIN==INFINITY && MAX==INFINITY){
        printf("taylor_log(%g) = %.12g\n", MIN, INFINITY);
        printf("taylor_log(%g) = %.12g\n", MAX, INFINITY);
    }
    else if(MIN==1 && MAX==1){
        printf("taylor_log(%g) = 0\n", MIN);
        printf("taylor_log(%g) = 0\n", MAX);
    }

    else{
        printf("taylor polynomial iterations = %u\n", median1);
        printf("taylor_log(%g) = %.12g\n", MIN, MIN!=0? taylor_log(MIN, median1): -INFINITY);
        printf("taylor_log(%g) = %.12g\n", MAX, MAX!=0? taylor_log(MAX, median1): -INFINITY);
    }
}
/// ************************************************************ ///


/// ********************** Hlavni program ********************** ///
int main(int argc, char *argv[])
{
    char *chyba1, *chyba2, *chyba3;
    double x;
    double MIN;
    double MAX;
    double EPS;
    unsigned long n;

    if(argc==4){
        x = strtod(argv[2], &chyba1);
        n = strtoul(argv[3], &chyba2, 10);
    }
    else if(argc==5){
        MIN = strtod(argv[2], &chyba1);
        MAX = strtod(argv[3], &chyba2);
        EPS = strtod(argv[4], &chyba3);
    }
    int err=inputCheck(argc, argv, x, MIN, MAX, EPS, n, chyba1, chyba2, chyba3);

    if(err==ERR_OK){

        switch (argc){
        case 4:
            printf("       log(%g) = %.12g\n", x, log(x));
            printf("    cf_log(%g) = %.12g\n", x, cfrac_log(x, n));
            printf("taylor_log(%g) = %.12g\n", x, taylor_log(x, n));
            break;

        case 5:
            printf("       log(%g) = %.12g\n",MAX<MIN? MAX : MIN, log(MAX<MIN? MAX : MIN));
            printf("       log(%g) = %.12g\n",MAX>MIN? MAX : MIN, log(MAX>MIN? MAX : MIN));
            bisekce_cfrac(MIN, MAX, EPS);
            bisekce_taylor(MIN, MAX, EPS);
            break;
        }
    }
    else{
        fprintf(stderr, "%s\n", EMSG[err]);
        return 1;
    }
    return 0;
}
/// ************************************************************ ///
