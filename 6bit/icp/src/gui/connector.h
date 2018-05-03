/**
 * @file    connector.h
 * @author  Tomáš Aubrecht (xaubre02)
 * @date    květen 2018
 * @brief   Reprezentace propoje mezi dvěma bloky.
 */

#pragma once

#include "editoritem.h"
#include <QGraphicsLineItem>
/**
 * @brief deklarace třídy
 */
class EditorItem;


/**
 * @brief třída reprezentující grafické spojení mezi dvěma bloky
 */
class Connector : public QGraphicsLineItem {
public:
    /**
     * @brief ID grafického objektu
     */
    enum { Type = UserType + 102 };

    /**
     * @brief konstruktor třídy
     * @param startItem odkaz na blok, ze kterého vychází
     * @param endItem odkaz na blok, ke kterému se připojuje
     * @param portID ID vstupního portu, ke kterému je připojen
     * @param contextMenu kontextové menu objektu
     * @param parent odkaz na rodičovský grafický objekt
     */
    Connector(EditorItem *startItem, EditorItem *endItem, int portID, QMenu *contextMenu, QGraphicsItem *parent = 0);

    /**
     * @brief destruktor třídy
     */
    ~Connector() {}

    /**
     * @brief vrací typ objektu
     * @return typ objektu
     */
    int type() const override { return Type; }

    /**
     * @brief aktualizuje svou pozici ve scéně
     */
    void updatePosition();

    /**
     * @brief vrací odkaz na blok, ze kterého vychází
     * @return odkaz na blok
     */
    EditorItem *startItem() const { return fromItem; }

    /**
     * @brief vrací odkaz na blok, ke kterému se připojuje
     * @return odkaz na blok
     */
    EditorItem *endItem() const { return toItem; }

    /**
     * @brief vrací ID vstupního portu, na který je připojen
     * @return ID portu
     */
    int portID() const { return inPortID; }

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
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

    /**
     * @brief zpracovává interakci vstupu myši nad objekt
     * @param event událost
     */
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    /**
     * @brief zpracovává interakci myši při pohybu nad objektem
     * @param event událost
     */
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

    /**
     * @brief zpracovává interakci opuštění objektu myší
     * @param event událost
     */
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *) override;

private:
    /**
     * @brief odkaz na blok, ze kterého vychází
     */
    EditorItem *fromItem;

    /**
     * @brief odkaz na blok, ke kterému se připojuje
     */
    EditorItem *toItem;

    /**
     * @brief kontextové menu
     */
    QMenu *curContextMenu;

    /**
     * @brief ID vstupního portu, na který je připojen
     */
    int inPortID;

    /**
     * @brief grafický objekt pro zobrazení aktuálního stavu na spoji
     */
    QGraphicsTextItem *hint;
};
