/**
 * @file    block.h
 * @author  Tomáš Aubrecht (xaubre02)
 * @date    květen 2018
 * @brief   Základ pro tvorbu funkčních bloků.
 */

#pragma once

#include <string>
#include <vector>

/**
 * @brief Prostor jmen pro základní bloky bez GUI.
 */
namespace core {
    /**
     * @brief Globální proměnná reprezentující vstupní port.
     */
    const int PORT_IN  = 0;
    /**
     * @brief Globální proměnná reprezentující výstupní port.
     */
    const int PORT_OUT = 1;

    /**
     * @brief Třída reprezentující port funkčního bloku.
     */
    class Port {
    private:
        /**
         * @brief ID portu (pouze u vstupního)
         */
        int id;
        /**
         * @brief typ portu (IN/OUT)
         */
        int type;
        /**
         * @brief jméno typu spojeného s portem
         */
        std::string name;
        /**
         * @brief číselná hodnota portu
         */
        double value;
        /**
         * @brief odkaz na port, který je k tomuto připojen
         */
        Port *pairPort;
        /**
         * @brief udává, zdali se dá portu nastavit hodnota (není připojen)
         */
        bool customizable;
        /**
         * @brief udává, zdali má port nastavenou hodnotu
         */
        bool valSet;

    public:
        /**
         * @brief konstruktor třídy
         * @param type typ portu (IN/OUT)
         * @param name jméno typu spojeného s portem
         */
        Port(int type, std::string name);
        /**
         * @brief destruktor třídy
         */
        ~Port() {}

        /**
         * @brief nastaví index portu
         * @param id index portu
         */
        void setIndex(int id)       { this->id = id; }
        /**
         * @brief vrací index portu
         * @return index portu
         */
        int index()                 { return id; }
        /**
         * @brief vrací typ portu
         * @return typ portu
         */
        int portType()              { return type; }
        /**
         * @brief vrací jméno typu spojeného s portem
         * @return jméno typu spojeného s portem
         */
        std::string portName()      { return name; }
        /**
         * @brief vrací hodnotu portu
         * @return hodnota portu
         */
        double portValue()          { return value; }
        /**
         * @brief vrací odkaz na port, ke kterému je tento port připojen
         * @return odkaz na port
         */
        Port *connectedPort()       { return pairPort; }
        /**
         * @brief vrací flag, zdali se dá portu nastavit hodnota (není připojen)
         * @return flag
         */
        bool hasCustomizableValue() { return customizable; }
        /**
         * @brief vrací flag, zdali má port nastavenou hodnotu
         * @return flag
         */
        bool hasValueSet()          { return valSet; }
        /**
         * @brief nastaví hodnotu portu
         * @param value hodnota k nastavení
         */
        void setValue(double value);
        /**
         * @brief vrací flag, zdali je port k dispozici
         * @return flag
         */
        bool isAvailable();
        /**
         * @brief připojí se k danému portu
         * @param port
         */
        void setPairPort(Port *port);
        /**
         * @brief odpojí se od portu
         */
        void release();
    };


    /**
     * @brief Abstraktní třída reprezentující obecný funkční blok.
     */
    class Block {
    private:
        /**
         * @brief název funkčního bloku
         */
        std::string name;
        /**
         * @brief vektor odkazů na vstupní porty tohoto bloku
         */
        std::vector<Port *> inPorts;
        /**
         * @brief vektor odkazů na funkční bloky, které jsou na tento blok připojeny
         */
        std::vector<Block *> outBlocks;
        /**
         * @brief odkaz na výstupní port tohoto bloku
         */
        Port *outPort;

    protected:
        /**
         * @brief vrací odkaz na vstupní port s daným ID
         * @param id ID portu
         */
        Port *inputPort(size_t id);
        /**
         * @brief nastaví jméno bloku
         * @param blockName jméno bloku
         */
        void setBlockName(std::string blockName) { name = blockName; }
        /**
         * @brief overload funkce
         * @param blockName jméno bloku
         */
        void setBlockName(const char *blockName) { name = blockName; }
        /**
         * @brief přiřadí bloku výstupní port
         * @param port výstupní port
         */
        void setOutputPort(Port *port)           { outPort = port; }
        /**
         * @brief přidá nový vstupní port, kterému nastaví ID podle aktuálního počtu vstupních portů
         * @param port vstupní port
         */
        void addInputPort(Port *port);

    public:
        /**
         * @brief konstruktor třídy
         */
        Block() {}
        /**
         * @brief virtuální destruktor třídy
         */
        virtual ~Block();

        /**
         * @brief vrací název bloku
         * @return název bloku
         */
        std::string getName()               { return name; }
        /**
         * @brief vrací odkaz na výstupní port
         * @return odkaz na výstupní port
         */
        Port *getOutputPort()               { return outPort; }
        /**
         * @brief vrací aktuální stav bloku
         * @return hodnota výstupního portu
         */
        std::vector<Port *> getInputPorts() { return inPorts; }
        /**
         * @brief vrací aktuální stav bloku
         * @return hodnota výstupního portu
         */
        double getState()                   { return outPort->portValue(); }
        /**
         * @brief vrací hodnotu, zdali se úspěšně podařilo připojit k danému bloku na daný vstup
         * @param block blok k připojení
         * @param id ID portu
         * @return flag
         */
        bool connectTo(Block *block, int id);
        /**
         * @brief odpojí vstupní port na základě poskytnutého ID
         * @param id ID vstupního portu
         */
        void releaseInputPort(int ID);
        /**
         * @brief odpojí se od daného bloku
         * @param block blok k odpojení
         */
        void releaseOutputBlock(Block *block);
        /**
         * @brief vrací hodnotu, zdali jsou všeechny vstupní porty připojeny
         * @return flag
         */
        bool inputPortsConnected();
        /**
         * @brief vrací hodnotu, zdali je výstupní port připojen
         * @return flag
         */
        bool outputPortConnected();
        /**
         * @brief aktualizace hodnot vstupních portů
         */
        void update();
        /**
         * @brief spustí výpočet tohoto bloku a všech následujících bloků, ke kterým je tento připojen
         */
        void startCalculation();
        /**
         * @brief zkontroluje, zdali jsou k tomuto bloku připojeny jiné bloky, a pokud ne, jedná se o vstupní blok schématu
         * @return flag, zdali je vstupní blok či ne
         */
        bool isInputBlock();
        /**
         * @brief  zkontroluje, zdali se při spuštění výpočtu, který pokračuje na další bloky, dostane tento blok do cyklu
         * @return flag, zdali se dostane do cyklu
         */
        bool isGoingToCycle();
        /**
         * @brief hledá cykly na základě počtu již projítých bloků, kde každý blok nesmí být obsažen ve vektoru vícekrát, než je počet cest, kterými se k němu dá dostat
         * @param blocks IN/OUT parametr s vektorem již projítých bloků
         * @param found IN/OUT flag značící zacyklení
         */
        void findCycle(std::vector<Block *> &blocks, bool &found);
        /**
         * @brief čistě virtuální metoda definující rozhraní, které provádí výpočet nad blokem
         */
        virtual bool calculate() = 0;
    };
}
