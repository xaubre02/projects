/**
 * @file    editorscene.cpp
 * @author  Tomáš Aubrecht (xaubre02)
 * @date    květen 2018
 * @brief   Grafická část editoru pro práci s bloky.
 */

#include "editorscene.h"

#include "connector.h"
#include <QTextCursor>
#include <QGraphicsSceneMouseEvent>


EditorScene::EditorScene(QMenu *itemMenu, QObject *parent) : QGraphicsScene(parent) {
    curItemType = EditorItem::Adder;
    myItemMenu = itemMenu;
    // výchozí mód je pohyb
    curMode = MoveItem;
    line = 0;
}


void EditorScene::setFont(const QFont &font) {
    curFont = font;
    // nastaveni fontu u poznamky, pokud je vybrana
    foreach (QGraphicsItem *item, selectedItems()) {
        if (item->type() == EditorNote::Type){
            EditorNote *note = qgraphicsitem_cast<EditorNote *>(item);
            // mohlo dojit ke zmene vybrane polozky
            if (note) {
                note->setFont(curFont);
            }
        }
    }
}


void EditorScene::setMode(Mode mode) {
    curMode = mode;
}


void EditorScene::setItemType(EditorItem::EditorItemType type) {
    curItemType = type;
}


void EditorScene::editorLostFocus(EditorNote *note) {
    QTextCursor cursor = note->textCursor();
    cursor.clearSelection();
    note->setTextCursor(cursor);

    // odstraneni poznamky, pokud neobsahuje zadny text
    if (note->toPlainText().isEmpty()) {
        removeItem(note);
        note->deleteLater();
    }
}


void EditorScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    // pokud to neni leve tlacitko, nic nedela
    if (mouseEvent->button() != Qt::LeftButton)
        return;

    EditorItem *item;
    EditorNote *note;
    switch (curMode) {
        // vlozeni bloku
        case InsertItem:
            item = new EditorItem(curItemType, myItemMenu);
            item->setPos(mouseEvent->scenePos() - QPointF(73, 73)); // vycentrovani objektu
            addItem(item);
            emit itemInserted(item);
            break;
        // vlozeni primky
        case InsertLine:
            line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(), mouseEvent->scenePos()));
            addItem(line);
            break;
        // vlozeni poznamky
        case InsertNote:
            note = new EditorNote(myItemMenu);
            note->setFont(curFont);
            note->setPos(mouseEvent->scenePos());
            // propojeni se signaly
            connect(note, SIGNAL(lostFocus(EditorNote*)), this, SLOT(editorLostFocus(EditorNote*)));
            connect(note, SIGNAL(selectedChange(QGraphicsItem*)), this, SIGNAL(noteSelected(QGraphicsItem*)));

            addItem(note);
            emit noteInserted(note);
        default:
            ;
    }
    // vychozi zpracovani udalosti
    QGraphicsScene::mousePressEvent(mouseEvent);
}


void EditorScene::loadObject(QString object, SaveData loader) {
    if (object == "EditorNote") {
        EditorNote *note = new EditorNote(myItemMenu, loader);
        connect(note, SIGNAL(lostFocus(EditorNote*)), this, SLOT(editorLostFocus(EditorNote*)));
        connect(note, SIGNAL(selectedChange(QGraphicsItem*)), this, SIGNAL(noteSelected(QGraphicsItem*)));
        addItem(note);
    }
    else if (object == "EditorItem") {
        EditorItem *item = new EditorItem(myItemMenu, loader);
        addItem(item);
        item->loadRest(loader);
        emit itemInserted(item);
    }
    else if (object == "Connector") {
        int portID;
        QPointF toPoint = loader.getToItem(portID);
        QPointF fromPoint = loader.getFromItem();

        // nalezeni bloku, ktere konektor spojuje
        EditorItem *toItem = 0;
        EditorItem *fromItem = 0;
        foreach(QGraphicsItem *sceneItem, items()) {
            if (sceneItem->scenePos() == fromPoint && sceneItem->type() == EditorItem::Type) {
                fromItem = qgraphicsitem_cast<EditorItem *>(sceneItem);
            }
            else if (sceneItem->scenePos() == toPoint && sceneItem->type() == EditorItem::Type) {
                toItem = qgraphicsitem_cast<EditorItem *>(sceneItem);
            }
        }

        // pokud byly bloky uspesne nalezeny (stejne jako pri vkladani)
        if (fromItem != 0 && toItem != 0) {
            if (fromItem->connectTo(toItem, portID)) {
                Connector *connector = new Connector(fromItem, toItem, portID, myItemMenu);
                fromItem->addConnector(connector);
                toItem->addConnector(connector, portID);
                connector->setZValue(-100.0);
                addItem(connector);
                connector->updatePosition();
                fromItem->hideResult();
            }
        }
    }
}


void EditorScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    // tvoreni usecky pri tahnuti mysi
    if (curMode == InsertLine && line != 0) {
        QLineF newLine(line->line().p1(), mouseEvent->scenePos());
        line->setLine(newLine);
    // posunovani objektu
    } else if (curMode == MoveItem) {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}


void EditorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (curMode == InsertLine && line != 0) {
        // prvni objekty nachazejici se na dane pozici jsou dane primky, proto je musme odstranit
        QList<QGraphicsItem *> startItems = items(line->line().p1());
        if (startItems.count() && startItems.first() == line)
            startItems.removeFirst();
        QPointF dropPoint = line->line().p2();
        QList<QGraphicsItem *> endItems = items(dropPoint);
        if (endItems.count() && endItems.first() == line)
            endItems.removeFirst();

        removeItem(line);
        delete line;

        // musi se jednat o dva ruzne bloky nachazejici se pod zacatkem a koncem primky
        if (startItems.count() > 0 && endItems.count() > 0 && startItems.first()->type() == EditorItem::Type && endItems.first()->type() == EditorItem::Type && startItems.first() != endItems.first()) {
            EditorItem *startItem = qgraphicsitem_cast<EditorItem *>(startItems.first());
            EditorItem *endItem   = qgraphicsitem_cast<EditorItem *>(endItems.first());
            // zjisteni, na kterou cast bloku smerovala mys
            int inPortID = whichPartOfItem(endItem, dropPoint);
            // pripojeni se k danemu bloku s konkretnim portem
            if (startItem->connectTo(endItem, inPortID)) {
                Connector *connector = new Connector(startItem, endItem, inPortID, myItemMenu);
                // pridani konektoru do objektu
                startItem->addConnector(connector);
                endItem->addConnector(connector, inPortID);
                // konektory chceme vzdy pod objekty
                connector->setZValue(-100.0);
                addItem(connector);
                connector->updatePosition();
                // Schovani vysledku u vychoziho bloku
                startItem->hideResult();
            }
        }
    }
    line = 0;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

int EditorScene::whichPartOfItem(EditorItem *item, QPointF dropPoint) {
    // index oblasti
    int partID = 0;
    // pocet vstupnich portu = poctu oblasti
    int inPorts = item->getInputPorts().size();
    // souradnice Y pusteni mysi v danem bloku
    int yDropInItem = QPointF(dropPoint - item->scenePos()).ry();
    if (inPorts > 1) {
        // vyska jedne casti bloku s portem
        int partHeight = item->boundingRect().size().height() / inPorts; // celkova vyska bloku / pocet portu
        partID = yDropInItem / partHeight; // integer prijde o desetinna mista
    }
    return partID;
}
