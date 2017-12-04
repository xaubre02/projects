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

// objekty pro zaznam statistik
Histogram delka_vyroby("Delka prace pracovnika", 0, 30 MIN, 20);
Stat doba_sberu("Doba sberu");
Stat doba_zpracovani("Doba zpracovani");
Stat doba_pauzy("Doba stravena na pauze");

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

uint sudy = 0;

bool sber_ukoncen = false;
bool transport_ukoncen = false;
bool zpracovani_ukonceno = false;

class Pracovnik : public Process 
{
    void Precerpat()
    {
        Enter(Hadice, 1);
        most -= 50;
        
        Wait(Uniform(10 MIN, 15 MIN));
        
        Leave(Hadice, 1);  // Sud je naplnen
        
        Wait(Exponential(4 MIN)); // Uklizeni sudu
        sudy++;
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
                if (transport_ukoncen && Hadice.Empty() && most < 50 && !Lis.Busy() && rmut < 120 && !Odzrnovac.Busy() && bedny_ke_zpracovani == 0 && Kolecka.Empty() && odpad < 30)
                {
                    zpracovani_ukonceno = true;
                    doba_zpracovani(Time - time);
                    pracovniku--;
                    Stop();
                    break;
                }
                
                double pauza = Time;
                WaitUntil   ( // ceka, dokud se neobjevi nejaka prace, kterou by mohl vykonavat
                                zpracovani_ukonceno ||
                                (!Hadice.Full() && most >= 50) ||
                                (!Lis.Busy() && rmut >= 120) ||
                                (!Odzrnovac.Busy() && bedny_ke_zpracovani >= 1) ||
                                (!Kolecka.Full() && odpad >= 30)
                            );
                doba_pauzy(Time - pauza);
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
        double time = Time; // zacatek sberu
        
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
                doba_sberu(Time - time);
                if (!Auto.Busy() && sklizenych_beden == 0 && sklizene_hrozny == 0)
                { 
                    transport_ukoncen = true; // nastaveni priznaku ukonceni prepravy -> vse prevezeno
                }
                Wait(Exponential(15 MIN)); 
                break;
            }
        }
        sberacu--;
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
    Hadice.Output();
    Kolecka.Output();
    
    delka_vyroby.Output();
    doba_sberu.Output();
    doba_zpracovani.Output();
    
    Print("Pocet sberacu na zacatku: %d\n", POCET_SBERACU);
    Print("Pocet sberacu na konci:   %d\n", sberacu);
    Print("Pocet pracovniku na zacatku: %d\n", POCET_PRACOVNIKU);
    Print("Pocet pracovniku na konci:   %d\n", pracovniku);
    Print("************************************************\n");
    Print("Pocet vyrobenych sudu s vinem: %d\n\n", sudy);
    
    Print("Pocet zbyvajicich zralych hroznu: %d\n", zrale_hrozny);
    Print("Pocet zbyvajicich sklizenych hroznu: %d\n", sklizene_hrozny);
    Print("Pocet zbyvajicich sklizenych beden: %d\n", sklizenych_beden);
    Print("Pocet zbyvajicich beden ke zpracovani: %d\n", bedny_ke_zpracovani);
    Print("Pocet zbyvajiciho odpadu: %d\n", odpad);
    Print("Pocet zbyvajiciho rmutu: %d\n", rmut);
    Print("Pocet zbyvajiciho mostu: %d\n", most);
    
    Print("Doba vinobrani: %d\n", SIMLIB_statistics.EndTime - SIMLIB_statistics.StartTime);
    
    SIMLIB_statistics.Output();
}
