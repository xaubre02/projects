/**
 * @file    editoritem.h
 * @author  Tomáš Aubrecht (xaubre02)
 * @date    květen 2018
 * @brief   Grafický objekt reprezentující funkční blok.
 */

#pragma once

#include "savedata.h"
#include "connector.h"
#include "core/myblocks.h"
#include <QGraphicsPixmapItem>
#include <QList>
// deklarace třídy
class Connector;


/**
 * @brief třída reprezentující grafické zobrazení funkčního bloku
 */
class EditorItem : public QGraphicsPixmapItem {
public:
    /**
     * @brief ID grafického objektu
     */
    enum { Type = UserType + 101 };

    /**
     * @brief výčet typů funkčních bloků
     */
    enum EditorItemType { Not, Sine, Cosine, Adder, Subtractor, Multiplier, Divider, LogAND, LogOR, LogXOR, LogNOT };

    /**
     * @brief konstuktor pro vytvoření objektu ze souboru
     * @param contextMenu kontextové menu objektu
     * @param data uložená data
     * @param parent odkaz na rodičovský grafický objekt
     */
    EditorItem(QMenu *contextMenu, SaveData data, QGraphicsItem *parent = 0);

    /**
     * @brief základní konstuktor
     * @param itemType typ funkčního bloku
     * @param contextMenu kontextové menu objektu
     * @param parent odkaz na rodičovský grafický objekt
     */
    EditorItem(EditorItemType itemType, QMenu *contextMenu, QGraphicsItem *parent = 0);

    /**
     * @brief destruktor
     */
    ~EditorItem();

    /**
     * @brief inicializuje objekt
     */
    void init();

    /**
     * @brief načte zbylá data objektu jako je zvýraznění, dílčí výsledky apod.
     * @param data uložená data
     */
    void loadRest(SaveData data);

    /**
     * @brief přidá nový konektor
     * @param connector konektor
     * @param portID ID portu, na který se konektor připojuje
     */
    void addConnector(Connector *connector, int portID = -1);

    /**
     * @brief odstraní daný konektor funkčního bloku
     * @param connector konektor
     */
    void removeConnector(Connector *connector);

    /**
     * @brief odstraní všechny konektory funkčního bloku
     */
    void removeConnectors();

    /**
     * @brief vrací typ bloku
     * @return typ bloku
     */
    EditorItemType editorItemType() const { return curItemType; }

    /**
     * @brief vrací typ objektu
     * @return typ objektu
     */
    int type() const override { return Type; }

    /**
     * @brief vrací ikonu bloku
     * @return ikona bloku
     */
    QPixmap icon() const { return curPixmap; }

    /**
     * @brief vrací vstupní bod bloku, na který se napojují konektory
     * @param ID ID vstupního bodu
     * @return vstupní bod
     */
    QPointF inputPoint(int ID) const { return inPoints[ID] + QPointF(73, 73); }

    /**
     * @brief vrací výstupní bod bloku, na který se napojují konektory
     * @return výstupní bod
     */
    QPointF outputPoint() const { return outPoint + QPointF(73, 73); }

    /**
     * @brief zobrazí výsledek kalkulace, pokud není nic připojeno na výstup
     */
    void displayResult();

    /**
     * @brief schová výsledek kalkulace
     */
    void hideResult();

    /**
     * @brief zobrazí nebo schová výsledek dílčího kroku na základě vstupního flagu
     * @param show flag
     */
    void showStepResult(bool show);

    /**
     * @brief zobrazí vstupní hodnotu portu ve štítku
     * @param index index vstupního štítku
     * @param data data vstupního štítku
     */
    void addInputLabel(int index, QString data);

    /**
     * @brief aktualizuje pozice všech svých štítků
     */
    void updateLabels();

    /**
     * @brief nastaví Z-hodnoty všech svých štítků
     * @param value Z-hodnota
     */
    void setLabelsZValue(qreal value);

    /**
     * @brief vrací seznam připojených bloků na vstupní porty
     * @return seznam bloků
     */
    QList<EditorItem *> getInputConnections();

    /**
     * @brief vrací seznam připojených bloků na výstupní port
     * @return seznam bloků
     */
    QList<EditorItem *> getOutputConnections();

    /**
     * @brief zvýrazní objekt danou barvou nebo zvýraznění odstraní
     * @param show flag zvýraznění
     * @param color barva zvýraznění
     */
    void showHighlight(bool show, QColor color = Qt::red);

    /**
     * @brief zapozdření funkce funkčního bloku
     */
    core::Port *getOutputPort()                      { return coreBlock->getOutputPort(); }

    /**
     * @brief zapozdření funkce funkčního bloku
     */
    std::vector<core::Port *> getInputPorts()        { return coreBlock->getInputPorts(); }

    /**
     * @brief zapozdření funkce funkčního bloku
     */
    std::string getName()                            { return coreBlock->getName(); }

    /**
     * @brief zapozdření funkce funkčního bloku
     */
    double getState()                                { return coreBlock->getState(); }

    /**
     * @brief zapozdření funkce funkčního bloku
     */
    void update()                                    { coreBlock->update(); }

    /**
     * @brief zapozdření funkce funkčního bloku
     */
    void startCalculation()                          { coreBlock->startCalculation(); }

    /**
     * @brief zapozdření funkce funkčního bloku
     */
    bool isGoingToCycle()                            { return coreBlock->isGoingToCycle(); }

    /**
     * @brief zapozdření funkce funkčního bloku
     */
    bool calculate()                                 { return coreBlock->calculate(); }

    /**
     * @brief zapozdření funkce funkčního bloku
     */
    bool isInputBlock()                              { return coreBlock->isInputBlock(); }

    /**
     * @brief zapozdření funkce funkčního bloku
     */
    void releaseInputPort(int ID)                    { coreBlock->releaseInputPort(ID); }

    /**
     * @brief zapozdření funkce funkčního bloku
     */
    void releaseOutputBlock(EditorItem *item)        { coreBlock->releaseOutputBlock(item->getCoreBlock()); }

    /**
     * @brief zapozdření funkce funkčního bloku
     */
    bool connectTo(EditorItem *item, int id)         { return coreBlock->connectTo(item->getCoreBlock(), id); }

    /**
     * @brief zapozdření funkce funkčního bloku
     */
    bool outputPortConnected()                       { return coreBlock->outputPortConnected(); }

    /**
     * @brief zapozdření funkce funkčního bloku
     */
    bool inputPortsConnected()                       { return coreBlock->inputPortsConnected(); }

    /**
     * @brief vrací svůj vlastní konkrétní funkční blok
     * @return funkční blok
     */
    core::Block *getCoreBlock()                      { return coreBlock; }

    /**
     * @brief seskupí svoje důležité atributy a vrátí je v řetězci
     * @return data k uložení
     */
    QString save();

protected:
    /**
     * @brief zpracovává interakci s kontextovým menu
     * @param event událost
     */
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

    /**
     * @brief zpracovává stav změny objektu
     * @param change daná změna
     * @param value hodnota změny
     * @return vrací hodnotu změny
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    /**
     * @brief vytvoří grafickou reprezentaci objektu
     * @param painter kreslící nástroj
     * @param inputs počet vstupů bloku
     * @param fillColor barva výplně
     */
    void drawBlockShape(QPainter *painter, int inputs, QColor fillColor);

    /**
     * @brief seznam všech připojených konektorů
     */
    QList<Connector *> connectors;

    /**
     * @brief typ funkčního bloku
     */
    EditorItemType curItemType;

    /**
     * @brief grafická reprezentace objektu
     */
    QPixmap curPixmap;

    /**
     * @brief kontextové menu
     */
    QMenu *curContextMenu;

    /**
     * @brief seznam vstupních míst, kde se napojují konektory
     */
    QList<QPointF> inPoints;

    /**
     * @brief výstupní místo, kde se napojuje konektor
     */
    QPointF outPoint;

    /**
     * @brief konkrétní funkční blok (backend)
     */
    core::Block *coreBlock;

    /**
     * @brief výsledek výpočtu bloku
     */
    QGraphicsTextItem *result;

    /**
     * @brief výsledek dílčího kroku
     */
    QGraphicsTextItem *stepResult;

    /**
     * @brief seznam štítků hodnot vstupních portů
     */
    QList<QGraphicsTextItem *> inputLabels;

    /**
     * @brief flag zobrazení výsledku
     */
    bool resDisplayed;

    /**
     * @brief flag zobrazení dílčího kroku
     */
    bool stepResDisplayed;

    /**
     * @brief flag zvýraznění
     */
    bool highlighted;

    /**
     * @brief barva zvýraznění objektu
     */
    QColor highlightCol;

    /**
     * @brief zvýraznění objektu
     */
    QGraphicsRectItem *highlight;
};
