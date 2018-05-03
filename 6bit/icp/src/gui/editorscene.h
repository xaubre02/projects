#pragma once

#include <QGraphicsScene>
#include <gui/editoritem.h>
/**
 * @file    editorscene.h
 * @author  Tomáš Aubrecht (xaubre02)
 * @date    květen 2018
 * @brief   Grafická část editoru pro práci s bloky.
 */

#include <gui/editornote.h>


/**
 * @brief třída reprezentující grafickou část pro práci s bloky
 */
class EditorScene : public QGraphicsScene {
    Q_OBJECT

public:
    /**
     * @brief seznam módů grafické scény
     */
    enum Mode { InsertItem, InsertLine, InsertNote, MoveItem };

    /**
     * @brief explicitní konstruktor
     * @param itemMenu kontextové menu pro objekty
     * @param parent odkaz na rodičovský grafický objekt
     */
    explicit EditorScene(QMenu *itemMenu, QObject *parent = 0);

    /**
     * @brief destruktor
     */
    ~EditorScene() {}

    /**
     * @brief vrací font grafické scény
     * @return font
     */
    QFont font() const { return curFont; }

    /**
     * @brief nastavuje font grafické scény
     * @param font nový font
     */
    void setFont(const QFont &font);

    /**
     * @brief načte uložený objekt
     * @param object název objektu
     * @param loader data objektu
     */
    void loadObject(QString object, SaveData loader);

public slots:
    /**
     * @brief nastavuje mód grafické scény
     * @param mode nový mód grafické scény
     */
    void setMode(Mode mode);

    /**
     * @brief nastavuje typ vkládaného objektu do scény
     * @param type typ objektu
     */
    void setItemType(EditorItem::EditorItemType type);

    /**
     * @brief zpracovává ztrátu zaměření na poznámku
     * @param note poznámka
     */
    void editorLostFocus(EditorNote *note);

signals:
    /**
     * @brief signál vysílaný při vložení bloku
     * @param item funkční blok
     */
    void itemInserted(EditorItem *item);

    /**
     * @brief signál vysílaný při vložení poznámky
     * @param note poznámka
     */
    void noteInserted(QGraphicsTextItem *note);

    /**
     * @brief signál při výběru poznámky
     * @param note poznámka
     */
    void noteSelected(QGraphicsItem *note);

protected:
    /**
     * @brief zpracovává událost při stisku tlačítka myši
     * @param mouseEvent událost
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

    /**
     * @brief zpracovává událost při pohybu myši
     * @param mouseEvent událost
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

    /**
     * @brief zpracovává událost při uvolnění tlačítka myši
     * @param mouseEvent událost
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

private:
    int whichPartOfItem(EditorItem *item, QPointF dropPoint);

    /**
     * @brief aktuální typ funkčního bloku
     */
    EditorItem::EditorItemType curItemType;

    /**
     * @brief kontextové menu
     */
    QMenu *myItemMenu;

    /**
     * @brief mód grafické scény
     */
    Mode curMode;

    /**
     * @brief linie spoje
     */
    QGraphicsLineItem *line;

    /**
     * @brief font grafické scény
     */
    QFont curFont;
};
