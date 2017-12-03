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

Facility Auto("Pick-up pro prepravu hroznu");
Facility Odzrnovac("Odzrnovac");
Facility Lis("Vodni lis");

Store Hadice("Hadice", 3);
Store Kolecka("Kolecka", 2);

uint bedny_ke_zpracovani = 0;
uint sklizenych_beden = 0;

uint sklizene_hrozny = 0;
uint zrale_hrozny = 1500;

uint odpad = 0;
uint rmut = 0;
uint most = 0;

bool sber_ukoncen = false;
bool transport_ukoncen = false;

class Pracovnik : public Process 
{
    void Behavior()
    {
        while (true)
        {
            if (!Hadice.Empty() && most >= 50)
                Precerpat();

            else if (!Lis.Busy() && rmut >= 120)
                Lisovat();

            else if (!Odzrnovac.Busy() && bedny_ke_zpracovani >= 1)
                Odzrnit();
            
            else if (!Kolecka.Empty() && odpad >= 30)
                Zpracovat_odpad();
            
            else
            {
                
            }
            
        }
    };
};

class Sberac : public Process 
{
    void Prepravit()
    {
        Seize(Auto);
        
        for (uint C = 0; C < KAPACITA_AUTA; C++) // nakladani beden
        {
            sklizenych_beden--;
            Wait(Exponential(15));
        }
        
        Wait(Exponential(10 MIN));  // cesta tam
        
        
        for (uint C = 0; C < KAPACITA_AUTA; C++) // vykladani beden
        {
            Wait(Exponential(15));
            bedny_ke_zpracovani++;
        }
        
        Wait(Exponential(10 MIN));  // cesta zpet
        
        Release(Auto);
    }
    
    void Sklidit()
    {
        zrale_hrozny--;
        Wait(Exponential(10));
        sklizene_hrozny++;
        
        if (sklizene_hrozny >= KAPACITA_BEDNY) // pokud je naplnena bedna hroznama
        {
            sklizene_hrozny -= KAPACITA_BEDNY;
            sklizenych_beden++;
        }
    }

    void Behavior()
    {
        while (true)
        {
            if (!Auto.Busy() && sklizenych_beden >= KAPACITA_AUTA) // pokud je k dispozici auto a je co prevest
                Prepravit();
                
            else if (zrale_hrozny > 0)  // jinak sbirat hrozny, pokud je co sbirat
                Sklidit();
                
            else // jinak po sobe uklidit a jit pomoct zpracovavat
            {
                sber_ukoncen = true; // nastaveni priznaku ukonceni sberu
                if (!Auto.Busy() && sklizenych_beden == 0 && sklizene_hrozny == 0)
                    transport_ukoncen = true; // nastaveni priznaku ukonceni prepravy -> vse prevezeno
                    
                Wait(Exponential(15 MIN)); 
                break;
            }
        }
        
        (new Pracovnik)->Activate();
    };
};

int main(void)
{ 
    // SetOutput(OUTPUT);
    Init(0, RUNTIME);
    
    // Generovani sberacu
    for (uint C = 0; C < POCET_SBERACU; C++)
        (new Sberac)->Activate();
    
    // Generovani pracovniku
    for (uint C = 0; C < POCET_PRACOVNIKU; C++)
        (new Pracovnik)->Activate();
    
    Run();
    
    // Vypis statistik
    Print("Pocet zbyvajicich zralych hroznu %d\n", zrale_hrozny);
    
    SIMLIB_statistics.Output();
}
