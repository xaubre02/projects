#pragma once

#include <QWidget>


/**
 * @brief třída zpracovávající ukládání a načítání blokových schémat
 */
class SaveData {
    /**
     * @brief obsah k uložení
     */
    QString data;

    /**
     * @brief seznam atributů
     */
    QStringList atts;

    /**
     * @brief seznam hodnot atributů
     */
    QStringList value;

public:
    /**
     * @brief konstruktor pro ukládání
     */
    SaveData() {}

    /**
     * @brief konstruktor pro načítání
     * @param data obsah jednoho řádku uloženého souboru
     */
    SaveData(QString data) { atts = data.split(";"); }

    /**
     * @brief destruktor
     */
    ~SaveData() {}

    /**
     * @brief vrací svůj obsah v podobě QT stringu
     * @return svůj obsah
     */
    QString toString() const { return data; }

    /**
     * @brief vyčistí svůj obsah
     */
    void clear();

    // ------------------- Ukládání -------------------
    /**
     * @brief přidá si další pozici
     * @param point bod pozice
     */
    void appendPos(QPointF point);
    
    /**
     * @brief uloží si jméno objektu
     * @param objectName jméno objektu
     */
    void setObjectName(QString objectName);

    /**
     * @brief uloží si pozici
     * @param point bod pozice
     */
    void addPos(QPointF point);

    /**
     * @brief uloží si font
     * @param font font
     */
    void addFont(QFont font);

    /**
     * @brief uloží si textová data
     * @param text textová data
     */
    void addTextData(QString text);

    /**
     * @brief uloží si typ objektu
     * @param type typ objektu
     */
    void addItemType(int type);

    /**
     * @brief uloží si výstupní port
     * @param portVal hodnota portu
     * @param label flag, zdali má port štítek
     */
    void addOutputPort(double portVal, bool label);

    /**
     * @brief uloží si vstupní port
     * @param ID ID portu
     * @param portVal hodnota portu
     * @param label flag, zdali má port štítek
     */
    void addInputPort(int ID, double portVal, bool label);

    /**
     * @brief uloží si flag zobrazení zvýraznění a jeho barvu
     * @param show flag zobrazení
     * @param color barva zvýraznění
     */
    void addHighlight(bool show, QColor color);

    /**
     * @brief  uloží si flag zobrazení dílčího výpočtu
     * @param show flag zobrazení
     */
    void addShowStepResult(bool show);

    /**
     * @brief  uloží si pozici bloku
     * @param point pozice bloku
     */
    void addFromItem(QPointF point);

    /**
     * @brief uloží si pozici bloku a ID jeho vstupního portu
     * @param point pozice bloku
     * @param portID ID portu
     */
    void addToItem(QPointF point, int portID);

    /**
     * @brief uloží si stav počítadla
     * @param counter stav počítadla
     */
    void addCounter(int counter);
    
    /**
     * @brief uloží si index pozadí
     * @param id index pozadí
     */
    void addBackgroundID(int id);

    // ------------------- Načítání -------------------
    /**
     * @brief vrací název objektu
     * @return název objektu
     */
    QString getObjectName();

    /**
     * @brief vrací pozici
     * @return pozice
     */
    QPointF getPos();

    /**
     * @brief vrací font
     * @return font
     */
    QFont getFont();

    /**
     * @brief vrací textová data
     * @return textová data
     */
    QString getTextData();

    /**
     * @brief vrací typ objektu
     * @return typ objektu
     */
    int getItemType();

    /**
     * @brief v parametrech vrací hodnoty výstupního portu
     * @param portVal hodnota portu
     * @param label flag, zdali má port štítek
     */
    void getOutputPort(double &portVal, bool &label);

    /**
     * @brief v parametrech vrací hodnoty vstupního portu
     * @param ID ID portu
     * @param portVal hodnota portu
     * @param label flag, zdali má port štítek
     */
    void getInputPort(int ID, double &portVal, bool &label);

    /**
     * @brief v parametrech vrací flag, zdali se má zobrazit zvýraznění, a jeho barvu
     * @param show flag zvýraznění
     * @param color barva zvýraznění
     */
    void getHighlight(bool &show, QColor &color);

    /**
     * @brief vrací flag, zdali se má zobrazit výpočet dílčího kroku
     * @return flag
     */
    bool getShowStepResult();

    /**
     * @brief vrací pozici bloku
     * @return pozice bloku
     */
    QPointF getFromItem();

    /**
     * @brief vrací pozici bloku a ID jeho portu v parametru
     * @param portID ID portu
     * @return pozice bloku
     */
    QPointF getToItem(int &portID);
    
    /**
     * @brief vrací hodnotu počítadla
     * @return hodnota počítadla
     */
    int getCounter();
    
    /**
     * @brief vrací seznam pozic
     * @return seznam bodů
     */
    QList<QPointF> getPosList();

    /**
     * @brief vrací ID pozadí
     * @return ID pozadí
     */
    int getBackgroundID();
};
