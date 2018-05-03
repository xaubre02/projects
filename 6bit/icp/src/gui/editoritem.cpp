/**
 * @file    editoritem.cpp
 * @author  Tomáš Aubrecht (xaubre02)
 * @date    květen 2018
 * @brief   Grafický objekt reprezentující funkční blok.
 */

#include "editoritem.h"

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QTextBlockFormat>
#include <QTextCursor>


EditorItem::EditorItem(QMenu *contextMenu, SaveData data, QGraphicsItem *parent) : QGraphicsPixmapItem(parent) {
    curContextMenu = contextMenu;
    curItemType = EditorItemType(data.getItemType());
    init();
    // nacteni pozice
    setPos(data.getPos());
    // nacteni hodnoty vystupniho portu
    double portVal;
    data.getOutputPort(portVal, resDisplayed);
    getOutputPort()->setValue(portVal);

    // nacteni vstupnich portu
    int id = 0;
    bool label;
    foreach (auto port, getInputPorts()){
        data.getInputPort(id, portVal, label);
        port->setValue(portVal);
        id++;
    }
}

EditorItem::EditorItem(EditorItemType itemType, QMenu *contextMenu, QGraphicsItem *parent) : QGraphicsPixmapItem(parent) {
    curItemType = itemType;
    curContextMenu = contextMenu;
    resDisplayed = false;
    stepResDisplayed = false;

    init();
}


EditorItem::~EditorItem() {
    delete highlight;
    delete coreBlock;
    delete result;
    delete stepResult;
    for (auto label: inputLabels) {
        if (label != nullptr)
            delete label;
    }
}

void EditorItem::init(){
    // Zvyrazneni
    highlight = new QGraphicsRectItem(1, 1, 144, 144);
    highlighted = false;
    highlight->setZValue(zValue() - 0.1); // kvuli pripojovani ostatnich bloku

    // Vysledek
    QFont font;
    font.setPixelSize(20);
    result = new QGraphicsTextItem();
    result->setFont(font);
    result->setDefaultTextColor(Qt::blue);
    result->setZValue(this->zValue());

    // Vysledek krokovani
    stepResult = new QGraphicsTextItem();
    stepResult->setFont(font);
    stepResult->setDefaultTextColor(Qt::green);
    stepResult->setZValue(this->zValue());

    curPixmap = QPixmap(146, 146);
    curPixmap.fill(Qt::transparent);
    QPainter painter(&curPixmap);
    painter.translate(73, 73);

    QRect rect(-30, -30, 60, 60);
    font.setPixelSize(38);
    font.setBold(true);

    // inicializace vzhledu bloku a jeho jadra
    switch (curItemType) {
        case Not:
            coreBlock = new core::Not; // backend jadro bloku
            drawBlockShape(&painter, 1, QColor(255, 225, 100)); // tvar a barva bloku
            font.setPixelSize(24); // velikost pisma v bloku
            painter.setFont(font);
            painter.drawText(rect, Qt::AlignCenter, "Not"); // text bloku
            break;
        case Sine:
            coreBlock = new core::Sine;
            drawBlockShape(&painter, 1, QColor(255, 225, 100));
            font.setPixelSize(24);
            painter.setFont(font);
            painter.drawText(rect, Qt::AlignCenter, "Sin");
            break;
        case Cosine:
            coreBlock = new core::Cosine;
            drawBlockShape(&painter, 1, QColor(255, 225, 100));
            font.setPixelSize(24);
            painter.setFont(font);
            painter.drawText(rect, Qt::AlignCenter, "Cos");
            break;
        case Adder:
            coreBlock = new core::Adder;
            drawBlockShape(&painter, 2, QColor(255, 150, 150));
            painter.setFont(font);
            painter.drawText(rect, Qt::AlignCenter, "+");
            break;
        case Subtractor:
            coreBlock = new core::Subtractor;
            drawBlockShape(&painter, 2, QColor(255, 150, 150));
            painter.setFont(font);
            painter.drawText(rect, Qt::AlignCenter, "–");
            break;
        case Multiplier:
            coreBlock = new core::Multiplier;
            drawBlockShape(&painter, 2, QColor(255, 150, 150));
            painter.setFont(font);
            painter.drawText(rect, Qt::AlignCenter, "×");
            break;
        case Divider:
            coreBlock = new core::Divider;
            drawBlockShape(&painter, 2, QColor(255, 150, 150));
            painter.setFont(font);
            painter.drawText(rect, Qt::AlignCenter, "÷");
            break;
        case LogAND:
            coreBlock = new core::LogicalAND;
            drawBlockShape(&painter, 2, QColor(150, 255, 100));
            font.setPixelSize(24);
            painter.setFont(font);
            painter.drawText(rect, Qt::AlignCenter, "AND");
            break;
        case LogOR:
            coreBlock = new core::LogicalOR;
            drawBlockShape(&painter, 2, QColor(150, 255, 100));
            font.setPixelSize(24);
            painter.setFont(font);
            painter.drawText(rect, Qt::AlignCenter, "OR");
            break;
        case LogXOR:
            coreBlock = new core::LogicalXOR;
            drawBlockShape(&painter, 2, QColor(150, 255, 100));
            font.setPixelSize(24);
            painter.setFont(font);
            painter.drawText(rect, Qt::AlignCenter, "XOR");
            break;
        case LogNOT:
            coreBlock = new core::LogicalNOT;
            drawBlockShape(&painter, 1, QColor(150, 255, 100));
            font.setPixelSize(24);
            painter.setFont(font);
            painter.drawText(rect, Qt::AlignCenter, "NOT");
            break;
        default:
            break;
    }
    // nastaveni flagu objektu
    setPixmap(curPixmap);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}


void EditorItem::loadRest(SaveData data) {
    // nacteni vystupniho portu
    double portVal;
    data.getOutputPort(portVal, resDisplayed);
    if (resDisplayed) {
        resDisplayed = false;
        displayResult();
    }

    // nacteni zobrazeni vstupnich portu
    int id = 0;
    bool label;
    foreach (auto port, getInputPorts()){
        data.getInputPort(id, portVal, label);
        if (label) {
            addInputLabel(id, QString::number(port->portValue()));
        }
        id++;
    }

    // nacteni zvyrazneni
    data.getHighlight(highlighted, highlightCol);
    if (highlighted) {
        highlighted = false;
        showHighlight(true, highlightCol);
    }
    // nacteni mezikroku
    stepResDisplayed = false;
    showStepResult(data.getShowStepResult());
}


void EditorItem::addConnector(Connector *connector, int portID) {
    connectors.append(connector);
    // skryti vstupu
    if (portID != -1 && inputLabels[portID] != nullptr) {
        delete inputLabels[portID];
        inputLabels[portID] = nullptr;
    }
}


void EditorItem::removeConnector(Connector *connector) {
    int index = connectors.indexOf(connector);
    if (index != -1) {
        // odstraneni GUI konektoru
        connectors.removeAt(index);
        // Vystupni blok
        if (connector->startItem() == this) {
            releaseOutputBlock(connector->endItem());
        }
        // Vstupni blok
        else {
            // odstraneni spojeni
            releaseInputPort(connector->portID());
        }
    }
}


void EditorItem::removeConnectors() {
    foreach (Connector *connector, connectors) {
        // konektor se musi odstranit z vstupniho i vystupniho bloku
        connector->startItem()->removeConnector(connector);
        connector->endItem()->removeConnector(connector);
        scene()->removeItem(connector);
        delete connector;
    }
}


void EditorItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    scene()->clearSelection();
    setSelected(true);
    curContextMenu->exec(event->screenPos());
}


QVariant EditorItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    // pokud se s objektem hybe, musi se aktualizovat i pozice jeho konektoru
    if (change == QGraphicsItem::ItemPositionChange) {
        foreach (Connector *connector, connectors) {
            connector->updatePosition();
        }
        updateLabels();
    }
    return value;
}


void EditorItem::drawBlockShape(QPainter *painter, int inputs, QColor fillColor) {
    // inicializace
    painter->setPen(QPen(Qt::black, 2));
    painter->setBrush(fillColor);

    QFont font;
    font.setPixelSize(8);
    painter->setFont(font);

    switch(inputs){
        // jeden vstup
        case 1:
            // Vstup
            inputLabels << nullptr;
            // zakladni obrys
            painter->drawEllipse(QPoint(0, 0), 48, 48);
            // Output
            painter->setBrush(Qt::green);
            painter->drawText(QPoint(30, 4), "OUT");
            painter->drawLine(QPoint(48, 0), QPoint(58, 0));
            outPoint = QPoint(62, 0);
            painter->drawEllipse(outPoint, 4, 4);
            // Inputs
            painter->drawText(QPoint(-45, 4), "IN");
            painter->drawLine(QPoint(-48, 0), QPoint(-58, 0));
            inPoints << QPoint(-62, 0);
            painter->drawEllipse(inPoints[0], 4, 4);
            break;
        // dva vystupy
        case 2:
            // Vstupy
            inputLabels << nullptr << nullptr;
            // zakladni obrys
            painter->drawRect(-48, -48, 96, 96);
            // Output
            painter->setBrush(Qt::green);
            painter->drawText(QPoint(30, 4), "OUT");
            painter->drawLine(QPoint(48, 0), QPoint(58, 0));
            outPoint = QPoint(62, 0);
            painter->drawEllipse(outPoint, 4, 4);
            // Input 1
            painter->drawText(QPoint(-45, -26), "IN1");
            painter->drawLine(QPoint(-48, -30), QPoint(-58, -30));
            inPoints << QPoint(-62, -30);
            painter->drawEllipse(inPoints[0], 4, 4);
            // Input 2
            painter->drawText(QPoint(-45, 34), "IN2");
            painter->drawLine(QPoint(-48, 30), QPoint(-58, 30));
            inPoints << QPoint(-62, 30);
            painter->drawEllipse(inPoints[1], 4, 4);
            break;
        default:
            return;
    }
}


void EditorItem::displayResult(){
    // aktualizace vysledku
    QString text = QString::number(getState());
    result->setPlainText(text);
    result->setPos(this->scenePos() + outPoint + QPoint(81, 56));
    // zobrazeni vysledku
    if (!resDisplayed) {
        scene()->addItem(result);
        resDisplayed = true;
    }
}

void EditorItem::hideResult(){
    if (resDisplayed) {
        scene()->removeItem(result);
        resDisplayed = false;
    }
}


void EditorItem::addInputLabel(int index, QString data) {
    // vstup jeste nema svuj stitek
    if (inputLabels[index] == nullptr) {
        // inicializace objektu pro kresleni
        QFont font;
        font.setPixelSize(20);
        inputLabels.insert(index, new QGraphicsTextItem);
        inputLabels[index]->setFont(font);
        inputLabels[index]->setTextWidth(200);
        inputLabels[index]->setDefaultTextColor(Qt::blue);
        inputLabels[index]->setPlainText(data);

        // Zarovnani vpravo
        QTextBlockFormat format;
        format.setAlignment(Qt::AlignRight);
        QTextCursor cursor = inputLabels[index]->textCursor();
        cursor.select(QTextCursor::Document);
        cursor.mergeBlockFormat(format);
        cursor.clearSelection();
        inputLabels[index]->setTextCursor(cursor);
        // -------------------------------------
        inputLabels[index]->setPos(this->scenePos() + inPoints[index] + QPoint(-134, 56));
        scene()->addItem(inputLabels[index]);
        inputLabels[index]->setZValue(this->zValue());
    // Aktualizace jiz existujiciho vstupu
    }else{
        inputLabels[index]->setPlainText(data);
        // Zarovnani vpravo
        QTextBlockFormat format;
        format.setAlignment(Qt::AlignRight);
        QTextCursor cursor = inputLabels[index]->textCursor();
        cursor.select(QTextCursor::Document);
        cursor.mergeBlockFormat(format);
        cursor.clearSelection();
        inputLabels[index]->setTextCursor(cursor);
        inputLabels[index]->setZValue(this->zValue());
        // -------------------------------------
    }
}

void EditorItem::updateLabels() {
    // vystup
    if (result != 0)
        result->setPos(this->scenePos() + outPoint + QPoint(81, 56));
    // vstupy
    for (int i = 0; i < inputLabels.size(); i++){
        if (inputLabels[i] != nullptr)
            inputLabels[i]->setPos(this->scenePos() + inPoints[i] + QPoint(-134, 56));
    }
    // zvyrazneni
    highlight->setPos(pos());
    // Vysledek kroku
    stepResult->setPos(this->scenePos() + QPoint(0, -30));
}

void EditorItem::setLabelsZValue(qreal value) {
    // nastaveni Z-hodnot
    result->setZValue(value);
    stepResult->setZValue(value);
    highlight->setZValue(value - 0.1);
    for (int i = 0; i < inputLabels.size(); i++){
        if (inputLabels[i] != nullptr) {
            inputLabels[i]->setZValue(value);
        }
    }
}


void EditorItem::showStepResult(bool show){
    if (show) {
        // aktualizace vysledku
        QString text = QString::number(getState());
        stepResult->setPlainText(text);
        stepResult->setPos(this->scenePos() + QPoint(0, -30));
        // zobrazeni vysledku
        if (!stepResDisplayed) {
            scene()->addItem(stepResult);
            stepResDisplayed = true;
        }
    } else {
        if (stepResDisplayed) {
            scene()->removeItem(stepResult);
            stepResDisplayed = false;
        }
    }
}


void EditorItem::showHighlight(bool show, QColor color) {
    // zobrazit
    if (show) {
        // aktualizace
        highlightCol = color;
        highlight->setPen(QPen(color, 3));
        highlight->setPos(pos());
        // zobrazeni
        if (!highlighted) {
            scene()->addItem(highlight);
            highlighted = true;
        }
    // skryt
    } else {
        if (highlighted) {
            scene()->removeItem(highlight);
            highlighted = false;
        }
    }
}


QList<EditorItem *> EditorItem::getInputConnections(){
    QList<EditorItem *> blocks;
    // prochazeni vsech pripojenych konektoru
    foreach(Connector *connector, connectors) {
        // pokud je konec konektoru pripojen na tento objekt, jedna se o vstupni konektor
        if (connector->endItem() == this) {
            blocks << connector->startItem();
        }
    }
    return blocks;
}


QList<EditorItem *> EditorItem::getOutputConnections(){
    QList<EditorItem *> blocks;
    // prochazeni vsech pripojenych konektoru
    foreach(Connector *connector, connectors) {
        // pokud je zacatek konektoru pripojen na tento objekt, jedna se o vystupni konektor
        if (connector->startItem() == this) {
            blocks << connector->endItem();
        }
    }
    return blocks;
}


QString EditorItem::save() {
    SaveData data;
    data.setObjectName("EditorItem");
    data.addItemType(editorItemType());
    data.addPos(scenePos());
    // ulozeni vystupniho portu
    data.addOutputPort(getOutputPort()->portValue(), resDisplayed);
    // ulozeni hodnot vstupnich portu
    foreach(auto port, getInputPorts()) {
        data.addInputPort(port->index(), port->portValue(), port->hasCustomizableValue() && port->hasValueSet());
    }
    // ulozeni zvyrazneni a zobrazeni vysledku
    data.addHighlight(highlighted, highlightCol);
    data.addShowStepResult(stepResDisplayed);

    return data.toString();
}
