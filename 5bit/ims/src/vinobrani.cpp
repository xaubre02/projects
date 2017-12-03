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

Histogram delka_vyroby("Delka prace pracovnika", 0, 30 MIN, 20);

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

uint sberacu = 0;
uint pracovniku = 0;

bool sber_ukoncen = false;
bool transport_ukoncen = false;

class Pracovnik : public Process 
{
    void Precerpat()
    {
        Enter(Hadice, 1);
        most -= 50;
        
        Wait(Uniform(10 MIN, 15 MIN));
        
        Leave(Hadice, 1);  // Sud je naplnen
        
        Wait(Exponential(4 MIN)); // Uklizeni sudu
    }
    
    void Lisovat()
    {
        Seize(Lis);
        rmut -= 120;
        
        Wait(Uniform(45 MIN, 60 MIN));  // Lisovani trva 45 az 60 minut
        
        odpad += 30;
        most += 90;
        Release(Lis);
    }
    
    void Odzrnit()
    {
        Seize(Odzrnovac);
        bedny_ke_zpracovani--;
        
        if(Random() < 0.8)
            Wait(Exponential(8 MIN));
        
        else
            Wait(Exponential(12 MIN));

        odpad++;
        rmut += 24;
        Release(Odzrnovac);
    }
    
    void ZpracovatOdpad()
    {
        Enter(Kolecka, 1);
        odpad -= 30;
        Wait(Exponential(8 MIN));
        Leave(Kolecka, 1);
    }
    
    void Behavior()
    {
        pracovniku++;
        double time = Time; // zacatek prace

        while (true)
        {
            if (!Hadice.Full() && most >= 50)
                Precerpat();

            else if (!Lis.Busy() && rmut >= 120)
                Lisovat();

            else if (!Odzrnovac.Busy() && bedny_ke_zpracovani >= 1)
                Odzrnit();
            
            else if (!Kolecka.Full() && odpad >= 30)
                ZpracovatOdpad();
            
            else
            {
                if (!sber_ukoncen)
                {   
                    WaitUntil(odpad >= 30 || bedny_ke_zpracovani >= 1 || rmut >= 120 || most >= 50);
                }
                else
                {
                    delka_vyroby(Time - time);
                    break;
                }
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
        sberacu++;
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
    /*
    // Generovani pracovniku
    for (uint C = 0; C < POCET_PRACOVNIKU; C++)
        (new Pracovnik)->Activate();*/
    
    Run();
    
    // Vypis statistik
    Auto.Output();
    Odzrnovac.Output();
    Lis.Output();
    
    delka_vyroby.Output();
    
    Print("Celkovy pocet sberacu: %d\n", sberacu);
    Print("Celkovy pocet pracovniku: %d\n", pracovniku);
    Print("************************************************\n");
    Print("Pocet zbyvajicich zralych hroznu: %d\n", zrale_hrozny);
    Print("Pocet zbyvajicich sklizenych hroznu: %d\n", sklizene_hrozny);
    Print("Pocet zbyvajicich sklizenych beden: %d\n", sklizenych_beden);
    Print("Pocet zbyvajicich beden ke zpracovani: %d\n", bedny_ke_zpracovani);
    Print("Pocet zbyvajiciho odpadu: %d\n", odpad);
    Print("Pocet zbyvajiciho rmutu: %d\n", rmut);
    Print("Pocet zbyvajiciho mostu: %d\n", most);
    

    SIMLIB_statistics.Output();
}
