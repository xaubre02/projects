/**************************************
* -+-+-+-+-+-+-+-+-+-+-+-+-
* - Predmet: IMS
* - Projekt: Vyroba vina
* - Datum: 2017-12-01
* - Autor: Tomas Aubrecht
* - Login: xaubre02
* -+-+-+-+-+-+-+-+-+-+-+-+-
***************************************/

#include "vinobrani.hpp"

void Sberac::Behavior(void)
{
    time = Time; // zacatek prace
    
    while (true)
    {
        // pokud je k dispozici auto a je co prevest
        if (!Auto.Busy() && sklizenych_beden >= KAPACITA_AUTA)
            Prepravit();
            
        // jinak sbirat hrozny, pokud je co sbirat
        else if (zrale_hrozny >= KAPACITA_BEDNY)
            Sklidit();

        // jinak po sobe uklidit a jit pomoct zpracovavat
        else
        {
            sber_ukoncen = true; // nastaveni priznaku ukonceni sberu
            doba_sberu(Time - time);
            
            // nastaveni priznaku ukonceni prepravy -> vse sklizeno a prevezeno
            if (!Auto.Busy() && sklizenych_beden == 0)
                transport_ukoncen = true;

            Wait(Uniform(15, 20));  // presun ke zpracovani
            break;
        }
    }
    
    (new Pracovnik)->Activate(); // uz nesbira, ale zpracovava
}

void Sberac::Sklidit(void)
{
    zrale_hrozny -= KAPACITA_BEDNY;
    Wait(Uniform(8, 12));   // sber trva 8 - 12 minut
    sklizenych_beden++;
}

void Sberac::Prepravit(void)
{
    Seize(Auto);
    
    for (uint C = 0; C < KAPACITA_AUTA; C++) // nakladani beden
    {
        sklizenych_beden--;
        Wait(15 SEC);
    }
    
    Wait(Uniform(15, 20));  // cesta tam
    
    
    for (uint C = 0; C < KAPACITA_AUTA; C++) // vykladani beden
    {
        Wait(15 SEC);
        bedny_ke_zpracovani++;
    }
    
    Wait(Uniform(15, 20));  // cesta zpet
    
    Release(Auto);
}

void Pracovnik::Behavior(void)
{
    time = Time; // zacatek prace

    while (true)
    {
        // pokud je k dispozici cerpadlo a je co precerpat
        if (!Cerpadlo.Busy() && most >= 30)
            Precerpat();

        // jinak pokud je k dispozici lis a je co lisovat
        else if (!Lis.Busy() && rmut >= KAPACITA_LISU)
            Lisovat();

        // jinak pokud je k dispozici odzrnovac a je co odzrnovat
        else if (!Odzrnovac.Busy() && bedny_ke_zpracovani >= 1)
            Odzrnit();
        
        // jinak pokud jsou k dispozici jedny kolecka a je dostatek odpadu pro odvezeni
        else if (!Kolecka.Full() && odpad >= 30)
            ZpracovatOdpad();
        
        else
        {
            // pokud je vse hotove
            if (transport_ukoncen && !Cerpadlo.Busy() && most < 30 && !Lis.Busy() && rmut < KAPACITA_LISU && !Odzrnovac.Busy() && bedny_ke_zpracovani == 0 && Kolecka.Empty() && odpad < 30)
            {
                Stop(); // ukonceni simulace
                zpracovani_ukonceno = true;
                doba_zpracovani(Time - time);
                break;
            }
            
            // jinak je pracovnik na pauze
            double pauza = Time;
            WaitUntil   ( // ceka, dokud se neobjevi nejaka prace, kterou by mohl vykonavat, nebo nez se ukonci vyroba vina
                            zpracovani_ukonceno ||
                            (!Cerpadlo.Busy() && most >= 30) ||
                            (!Lis.Busy() && rmut >= KAPACITA_LISU) ||
                            (!Odzrnovac.Busy() && bedny_ke_zpracovani >= 1) ||
                            (!Kolecka.Full() && odpad >= 30)
                        );
            doba_pauzy(Time - pauza);
        }
    }
}

void Pracovnik::Odzrnit(void)
{
    Seize(Odzrnovac);
    bedny_ke_zpracovani--;
    
    // kazda pata bedna zpusobi ucpani
    if(Random() < 0.8)
        Wait(Uniform(3, 4));
    
    else
        Wait(Uniform(3, 4) * 2); // odzrneni trva dvakrat dele

    odpad++;
    rmut += 24;
    Release(Odzrnovac);
}    

void Pracovnik::Lisovat(void)
{
    Seize(Lis);
    rmut -= KAPACITA_LISU;
    
    Wait(Uniform(40, 50));  // Lisovani trva 40 az 50 minut
    
    odpad += uint(0.25 * KAPACITA_LISU);
    most += uint(0.75 * KAPACITA_LISU);
    Release(Lis);
}
    
void Pracovnik::Precerpat(void)
{
    Seize(Cerpadlo);
    most -= 30;
    
    Wait(3); // precerpani trva 3 minuty
    
    sudy++;
    Release(Cerpadlo);  // Sud je naplnen
}
    
void Pracovnik::ZpracovatOdpad(void)
{
    Enter(Kolecka, 1);
    odpad -= 30;
    Wait(Uniform(8, 10));
    Leave(Kolecka, 1);
}

int main(void)
{ 
    // SetOutput(OUTPUT);
    Init(0, RUNTIME);
    
    // Generovani sberacu
    for (int C = 0; C < POCET_SBERACU; C++)
        (new Sberac)->Activate();

    // Generovani pracovniku
    for (int C = 0; C < POCET_PRACOVNIKU; C++)
        (new Pracovnik)->Activate();

    Run();
    
    // Vypis statistik
    Auto.Output();
    Odzrnovac.Output();
    Lis.Output();
    Cerpadlo.Output();
    Kolecka.Output();
    
    doba_pauzy.Output();
    doba_sberu.Output();
    doba_zpracovani.Output();
    
    Print("Pocet sberacu na zacatku: %d\n", POCET_SBERACU);
    Print("Pocet pracovniku na zacatku: %d\n", POCET_PRACOVNIKU);
    Print("************************************************\n");
    Print("Pocet vyrobenych sudu s vinem: %d\n\n", sudy);
    
    Print("Pocet zbyvajicich zralych hroznu: %d\n", zrale_hrozny);
    Print("Pocet zbyvajicich sklizenych beden: %d\n", sklizenych_beden);
    Print("Pocet zbyvajicich beden ke zpracovani: %d\n", bedny_ke_zpracovani);
    Print("Pocet zbyvajiciho odpadu: %d\n", odpad);
    Print("Pocet zbyvajiciho rmutu: %d\n", rmut);
    Print("Pocet zbyvajiciho mostu: %d\n", most);
    
    SIMLIB_statistics.Output();
}
