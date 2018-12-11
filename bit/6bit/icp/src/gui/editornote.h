/**
 * @file    editornote.h
 * @author  Tomáš Aubrecht (xaubre02)
 * @date    květen 2018
 * @brief   Textový objekt reprezentující poznámku.
 */

#pragma once

#include "gui/savedata.h"
#include <QGraphicsTextItem>
#include <QPen>


/**
 * @brief třída reprezentující textovou poznámku v editoru
 */
class EditorNote : public QGraphicsTextItem {
    Q_OBJECT

public:
    /**
     * @brief ID grafického objektu
     */
    enum { Type = UserType + 100 };

    /**
     * @brief konstruktor třídy pro načítání ze souboru
     * @param contextMenu kontextové menu
     * @param data data z uloženého souboru
     * @param parent odkaz na rodičovský grafický objekt
     */
    EditorNote(QMenu *contextMenu, SaveData data, QGraphicsItem *parent = 0);

    /**
     * @brief základní konstruktor třídy
     * @param contextMenu kontextové menu
     * @param parent odkaz na rodičovský grafický objekt
     */
    EditorNote(QMenu *contextMenu, QGraphicsItem *parent = 0);

    /**
     * @brief destruktor třídy
     */
    ~EditorNote() {}

    /**
     * @brief vrací typ objektu
     * @return typ objektu
     */
    int type() const override { return Type; }

    /**
     * @brief seskupí svoje důležité atributy a vrátí je v řetězci
     * @return data k uložení
     */
    QString save();

signals:
    /**
     * @brief signál při ztrátě zaměření na poznámku
     * @param note poznámka
     */
    void lostFocus(EditorNote *note);

    /**
     * @brief signál při změně výběru grafického objektu
     * @param item grafický objekt
     */
    void selectedChange(QGraphicsItem *item);

protected:

    /**
     * @brief zpracovává stav změny objektu
     * @param change daná změna
     * @param value hodnota změny
     * @return vrací hodnotu změny
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    /**
     * @brief zpracovává událost ztráty zaměření
     * @param event událost
     */
    void focusOutEvent(QFocusEvent *event) override;

    /**
     * @brief zpracovává událost dvojkliku myši
     * @param event událost
     */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    /**
     * @brief zpracovává interakci s kontextovým menu
     * @param event událost
     */
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
    /**
     * @brief kontextové menu
     */
    QMenu *curContextMenu;
};
