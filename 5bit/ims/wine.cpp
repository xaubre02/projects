/**************************************
* -+-+-+-+-+-+-+-+-+-+-+-+-
* - Predmet: IMS
* - Projekt: Vyroba vina
* - Datum: 2017-12-01
* - Autor: Tomas Aubrecht
* - Login: xaubre02
* -+-+-+-+-+-+-+-+-+-+-+-+-
***************************************/

#include "simlib.h"

#define OUTPUT "wine.out"

// Casove konstanty - zakladni casova jednotka je minuta
// -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
#define HOUR  * 60         // hodina -> 60 minut
#define DAY   HOURS * 24   // den    -> 24 hodin
#define MONTH DAYS  * 30   // mesic  -> 30 dni
#define YEAR  DAYS  * 365  // rok    -> 365 dni

#define HOURS  HOUR
#define DAYS   DAY
#define MONTHS MONTH
#define YEARS  YEAR
// -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

#define RUNTIME 5 YEARS
#define PICKERS 8
#define GRAPEVINE_COUNT 800

// Stores
Store Pickers("Sberaci", 8);        // nezajima nas, kdo sbira ze ktereho kere
Store Separator("Odzrnovac", 20);   // Do odzrnovace se v jednu chvili vleze maximalne 20 kg

// Facilities

unsigned int trapina = 0;
// Hroznove vino
class Grape : public Process 
{
    void Behavior()
    {
        Enter(Separator, 1);
        
        Wait(Exponential(0.5));
        
        Leave(Separator, 1);
        
        if (Random() < 0.04) // 4% skonci jako odpad
        {
            trapina++;
            return;
        }
    }
};

bool less_grapes = true;
// vinna reva
class Grapevine : public Process
{
    void Behavior()
    {
        while (true)
        {
            Enter(Pickers, 1); // zabira sberace
            
            Wait(Exponential(1));
            
            if (less_grapes)
            {
                (new Grape)->Activate(); // vytvori novy hrozen
            }
            else
            {
                (new Grape)->Activate();
                (new Grape)->Activate();
            }
            
            less_grapes = !less_grapes;
            
            Leave(Pickers, 1);
            
            Wait(1 YEAR); // ceka jeden rok do dalsi urody
        }
    }
};

int main(void)
{
    SetOutput(OUTPUT); 
    Init(0, RUNTIME);
    
    // generovani vinne revy do systemu
    for (int C = 0; C < GRAPEVINE_COUNT; C++)
        (new Grapevine)->Activate();
       
    
    Run();
}
