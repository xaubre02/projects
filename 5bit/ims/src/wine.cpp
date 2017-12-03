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
#include "constants.h"

// Stores
Store Sberaci("Sberaci", 8);     // Nezajima nas, kdo sbira ze ktereho kere

// Facilities
Facility Transport("Transport");
Facility Odzrnovac("Odzrnovac");
Facility Lis("Lis");

//Fronty
Queue kap_auta("Kapacita auta");
Queue kap_lisu("Kapacita lisu")


// Statistics
Histogram celkovy_cas("Detailni histogram casu hroznu v systemu", 1 HOURS, 1 HOUR, 15);


// Pocitadla
uint hrozny = 0;
uint vino   = 0;
uint odpad  = 0;
uint ucpani = 0;

// Hroznove vino
class Hrozny : public Process 
{
    void Behavior()
    {
        //celkovy pocet sklizenych hroznu
        hrozny++;
        double time = Time;
            
        if (Random() < 0.005)  // 0.5% je nepouzitelnych (zhnile, plesnive, apod.)
        {
            odpad++;
            return;
        }
        
        // pokud se jedna o posledni hrozen pro nalpneni kapacity
        if ( kap_auta.Size() == (KAPACITA_AUTA - 1) )
        {
            Seize(Transport);
            Wait(Exponential(10));
            Release(Transport);
        }    
        
        else
            Into(kap_auta);
        
        Seize(Odzrnovac);
        
        if (Random() < 0.015)  // 1.5% zpusobi ucpani
        {
            ucpani++;
            Wait(Exponential(1.5));
        }
        else
        {
            Wait(Exponential(0.5));
        }
        
        Release(Odzrnovac);
        
        if (Random() < 0.04) // 4% skonci jako odpad
        {
            odpad++;
            return;
        }

        Enter(Lis, 1);
        
        // pockat, az se lis naplni
        WaitUntil(Lis.Used() == KAPACITA_LISU);
        Wait(60);           // Lisovani trva hodinu
        Leave(Lis, 1);
        
        if (Random() < 0.2) // 20% skonci jako odpad
        {
            odpad++;
            return;
        }
        
        vino++;
        celkovy_cas(Time - time);
    }
};

// kere vinne revy
bool less_grapes = true;
class VinnaReva : public Process
{
    void Behavior()
    {
        while (true)
        {
            Enter(Sberaci, 1); // zabira sberace
            
            Wait(Exponential(1));
            
            // Jeden ker prumerne vyprodukuje 1.5 kg hroznu
            // -> Ker vygeneruje 1kg hroznu a kazdy druhy ker vygeneruje 2kg
            if (less_grapes)
            {
                (new Hrozny)->Activate(); // vytvori novy hrozen
            }
            else
            {
                (new Hrozny)->Activate();
                (new Hrozny)->Activate();
            }
            
            less_grapes = !less_grapes;
            
            Leave(Sberaci, 1);
            
            Wait(1 YEAR); // ceka jeden rok do dalsi urody
        }
    }
};

class DovozHroznu : public Event
{
    void Behavior()
    {
        for (int C = 0; C < KAPACITA_AUTA; C++)
            (new Hrozny)->Activate();
            
        Activate(Time + Exponential(PRIJEZD_HROZNU));
    }
};

int main(void)
{ 
    // SetOutput(OUTPUT);
    Init(0, RUNTIME);
    /*
    // generovani vinne revy do systemu
    for (int C = 0; C < POCET_KERU; C++)
        (new VinnaReva)->Activate();*/
    
    // Generovani hroznu pro zpracovani
    (new DovozHroznu)->Activate();
    
    Run();
    
    Sberaci.Output();
    Odzrnovac.Output();
    Lis.Output();
    celkovy_cas.Output();
    Print("Pocet kg hroznu %d\n", hrozny);
    Print("Pocet kg odpadu %d\n", odpad);
    Print("Pocet litru vina %d\n", vino);
    Print("Pocet ucpani %d\n", ucpani);
    SIMLIB_statistics.Output();
}
