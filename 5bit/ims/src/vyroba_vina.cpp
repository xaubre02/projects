/**************************************
* -+-+-+-+-+-+-+-+-+-+-+-+-
* - Predmet: IMS
* - Projekt: Vyroba vina
* - Datum: 2017-12-01
* - Autor: Tomas Aubrecht
* - Login: xaubre02
* -+-+-+-+-+-+-+-+-+-+-+-+-
***************************************/

#include <time.h>
#include "vyroba_vina.hpp"

void Sberac::Behavior(void)
{
    time = Time; // zacatek prace
    
    while (true)
    {
        // pokud je k dispozici auto a je co prevest
        if (!Auto.Busy() && sklizenych_beden >= kapacita_auta)
            Prepravit();
            
        // jinak sbirat hrozny, pokud je co sbirat
        else if (zrale_hrozny >= 25)
            Sklidit();

        // jinak po sobe uklidit a jit pomoct zpracovavat
        else
        {
            PremistitSe();
            break;
        }
    }
    
    (new Pracovnik)->Activate(); // uz nesbira, ale zpracovava
}

void Sberac::PremistitSe(void)
{
    // nastaveni priznaku ukonceni sberu
    sber_ukoncen = true;
    doba_sberu(Time - time);

    // nastaveni priznaku ukonceni prepravy -> vse sklizeno a prevezeno
    if (!Auto.Busy() && sklizenych_beden == 0)
        transport_ukoncen = true;

    // presun ke zpracovani
    Wait(Uniform(15, 20));
}

void Sberac::Sklidit(void)
{
    zrale_hrozny -= 25;
    Wait(Uniform(10, 15));
    sklizenych_beden++;
}

void Sberac::Prepravit(void)
{
    Seize(Auto);
    
    // nakladani beden
    sklizenych_beden -= kapacita_auta;
    Wait(Uniform(3, 4));
    
    // cesta tam
    Wait(Uniform(15, 20));
    
    // vykladani beden
    Wait(Uniform(3, 4));
    bedny_ke_zpracovani += kapacita_auta;
    
    // cesta zpet
    Wait(Uniform(15, 20));
    
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
        else if (!Lis.Busy() && rmut >= kapacita_lisu)
            Lisovat();

        // jinak pokud je k dispozici odzrnovac a je co odzrnovat
        else if (!Odzrnovac.Busy() && bedny_ke_zpracovani >= 1)
            Odzrnit();
        
        // jinak pokud jsou k dispozici jedny kolecka a je dostatek odpadu pro odvezeni
        else if (!Kolecka.Full() && odpad >= 30)
            ZpracovatOdpad();
        
        else
        {
            if ( // pokud je vse hotove
                transport_ukoncen && 
                !Cerpadlo.Busy() && most < 30 && 
                !Lis.Busy() && rmut < kapacita_lisu && 
                !Odzrnovac.Busy() && bedny_ke_zpracovani == 0 && 
                Kolecka.Empty() && odpad < 30
               )
            {
                Stop(); // ukonceni simulace
                zpracovani_ukonceno = true;
                doba_zpracovani(Time - time);
                break;
            }
            
            // jinak je pracovnik na pauze
            DatSiPauzu();
        }
    }
}

void Pracovnik::DatSiPauzu(void)
{
    pauza = Time;
    WaitUntil   ( // ceka, dokud se neobjevi nejaka prace, kterou by mohl vykonavat, nebo nez se ukonci vyroba vina
                    zpracovani_ukonceno ||
                    (!Cerpadlo.Busy()  && most >= 30) ||
                    (!Lis.Busy()       && rmut >= kapacita_lisu) ||
                    (!Odzrnovac.Busy() && bedny_ke_zpracovani >= 1) ||
                    (!Kolecka.Full()   && odpad >= 30)
                );
    doba_pauzy(Time - pauza); // ulozeni celkove doby stravene na pauze
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
    rmut -= kapacita_lisu;
    
    Wait(Uniform(45, 60));

    odpad += uint(0.25 * kapacita_lisu);  // 25% je matolina
    most += uint(0.75 * kapacita_lisu);   // 75% je most
    Release(Lis);
}
    
void Pracovnik::Precerpat(void)
{
    Seize(Cerpadlo);
    most -= 30;
    Wait(3);            // precerpani trva 3 minuty
    Release(Cerpadlo);  // Sud je naplnen
}
    
void Pracovnik::ZpracovatOdpad(void)
{
    Enter(Kolecka, 1);
    odpad -= 30;
    Wait(Uniform(8, 10));
    Leave(Kolecka, 1);
}

void parseParams(int argc, char* argv[])
{
    if (argc != 1 && argc != 5)
    {
        fprintf(stderr, "Neplatne argumenty!\n");
        exit(1);
    }
    
    if (argc == 5)
    {
        // prvni argument je cislo experimentu
        output.clear();
        output.append("experiment_").append(argv[1]).append(".out");
        
        // druhy argument je pocet sberacu
        pocet_sberacu = atoi(argv[2]);
        
        // treti argument je kapacita auta
        kapacita_auta = atoi(argv[3]);
        
        // ctvrty argument je kapacita lisu
        kapacita_lisu = atoi(argv[4]);
    }
}

void printData(void)
{
    Print("+----------------------------------------------------------+\n");   
    Print("|            Zaznam o provedenem experimentu               |\n");
    Print("+----------------------------------------------------------+\n\n");
    Print("   Pocatecni mnozstvi hroznu: %dkg\n", 750);
    Print("   Pocatecni pocet sberacu:   %d\n", pocet_sberacu);
    Print("   Kapacita auta: %d beden\n",       kapacita_auta);
    Print("   Kapacita lisu: %d litru\n\n",     kapacita_lisu);
    Print("+----------------------------------------------------------+\n\n");
    Print(" - Celkovy cas vyroby:      %.2f hodin\n",   SIMLIB_statistics.EndTime   / 60);
    Print(" - Prumerny cas sberu:      %.2f hodin\n",   doba_sberu.MeanValue()      / 60);
    Print(" - Prumerny cas zpracovani: %.2f hodin\n",   doba_zpracovani.MeanValue() / 60);
    Print(" - Prumerny cas na pauze:   %.2f hodin\n\n", doba_pauzy.Number() != 0 ? (doba_pauzy.MeanValue() / 60) : 0); // pokud je nejaky zaznam o pauze (pripad, kdy je pouze jeden sberac)
    
    Auto.Output();
    Odzrnovac.Output();
    Lis.Output();
    Cerpadlo.Output();
}

int main(int argc, char* argv[])
{
    // zpracovani argumentu programu
    parseParams(argc, argv);
    
    // nastaveni seedu pro generatory nahodnych cisel
    RandomSeed(long(time(NULL)));
    
    SetOutput(output.c_str());

    Init(0, 3 DAYS);
    
    // Generovani sberacu
    for (uint C = 0; C < pocet_sberacu; C++)
        (new Sberac)->Activate();

    Run();
    
    // formatovani a vypis ziskanych udaju 
    printData();
}
