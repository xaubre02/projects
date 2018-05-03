/**
 * @file    connector.cpp
 * @author  Tomáš Aubrecht (xaubre02)
 * @date    květen 2018
 * @brief   Reprezentace propoje mezi dvěma bloky.
 */

#include "connector.h"

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPen>
#include <QPainter>


Connector::Connector(EditorItem *startItem, EditorItem *endItem, int portID, QMenu *contextMenu, QGraphicsItem *parent) : QGraphicsLineItem(parent) {
    fromItem = startItem;
    toItem = endItem;
    curContextMenu = contextMenu;
    inPortID = portID;
    // inicializace objektu
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setAcceptHoverEvents(true);
    setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}


void Connector::updatePosition() {
    // aktualizace pozice konektoru, pokud se hybe s blokem, kte kteremu je pripojen
    QPointF p1(fromItem->pos() + fromItem->outputPoint());
    QPointF p2(toItem->pos() + toItem->inputPoint(inPortID));
    QLineF line(p1, p2);
    setLine(line);
}


void Connector::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    // zobrazeni menu objektu
    scene()->clearSelection();
    setSelected(true);
    curContextMenu->exec(event->screenPos());
}


void Connector::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    // zobrazeni hodnoty konektoru
    hint = new QGraphicsTextItem();
    QString text = QString::number(fromItem->getState());
    QFont font;
    font.setPixelSize(30);
    hint->setFont(font);
    hint->setDefaultTextColor(Qt::red);
    hint->setPlainText(text);
    hint->setPos(event->scenePos() + QPoint(10, 10));
    scene()->addItem(hint);
}


void Connector::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    // aktualizace polohy hodnoty konektoru
    hint->setPos(event->scenePos() + QPoint(10, 10));
}


void Connector::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
    // skryti hodnoty konektoru
    scene()->removeItem(hint);
    if (hint != 0) {
        delete hint;
        hint = 0;
    }
}

QString Connector::save() {
    SaveData data;
    data.setObjectName("Connector");
    data.addFromItem(startItem()->scenePos());
    data.addToItem(endItem()->scenePos(), inPortID);
    return data.toString();
}
