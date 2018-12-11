/**
 * @file    editornote.cpp
 * @author  Tomáš Aubrecht (xaubre02)
 * @date    květen 2018
 * @brief   Textový objekt reprezentující poznámku.
 */

#include "editornote.h"

#include "editorscene.h"
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>


EditorNote::EditorNote(QMenu *contextMenu, SaveData data, QGraphicsItem *parent) : QGraphicsTextItem(parent) {
    setPos(data.getPos());
    setFont(data.getFont());
    setPlainText(data.getTextData());
    curContextMenu = contextMenu;
    // nastaveni flagu objektu
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setDefaultTextColor(Qt::black);
    setZValue(1000);
}


EditorNote::EditorNote(QMenu *contextMenu, QGraphicsItem *parent) : QGraphicsTextItem(parent) {
    // inicializace vlastnosti a vzhledu
    curContextMenu = contextMenu;
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setDefaultTextColor(Qt::black);
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setZValue(1000);
}


QVariant EditorNote::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == QGraphicsItem::ItemSelectedHasChanged) {
        emit selectedChange(this);
    }
    return value;
}


void EditorNote::focusOutEvent(QFocusEvent *event) {
    setTextInteractionFlags(Qt::NoTextInteraction);
    emit lostFocus(this);
    QGraphicsTextItem::focusOutEvent(event);
}


void EditorNote::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    if (textInteractionFlags() == Qt::NoTextInteraction) {
        setTextInteractionFlags(Qt::TextEditorInteraction);
    }
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}


void EditorNote::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    // zobrazeni menu objektu
    scene()->clearSelection();
    setSelected(true);
    curContextMenu->exec(event->screenPos());
}

QString EditorNote::save() {
    SaveData data;
    data.setObjectName("EditorNote");
    data.addPos(scenePos());
    data.addFont(font());
    data.addTextData(toPlainText());
    return data.toString();
}
