#include "savedata.h"


void SaveData::clear() {
    data.clear();
    atts.clear();
    value.clear();
}

void SaveData::appendPos(QPointF point) {
    data.append(QString::number(point.rx()));
    data.append(",");
    data.append(QString::number(point.ry()));
    data.append("&");
}

void SaveData::setObjectName(QString objectName) {
    data.append("object:");
    data.append(objectName);
    data.append(";");
}

void SaveData::addPos(QPointF point) {
    data.append("pos:");
    data.append(QString::number(point.rx()));
    data.append(",");
    data.append(QString::number(point.ry()));
    data.append(";");
}

void SaveData::addFont(QFont font) {
    data.append("font:");
    data.append(font.family());
    data.append(",");
    data.append(QString::number(font.pointSize()));
    data.append(";");
}

void SaveData::addTextData(QString text) {
    data.append("text:");
    data.append(text);
    data.append(";");
}

void SaveData::addItemType(int type) {
    data.append("itemtype:");
    data.append(QString::number(type));
    data.append(";");
}

void SaveData::addOutputPort(double portVal, bool label) {
    data.append("portout:");
    data.append(QString::number(portVal));
    data.append(",");
    data.append(QString::number(label));
    data.append(";");
}

void SaveData::addInputPort(int ID, double portVal, bool label) {
    data.append("portin:");
    data.append(QString::number(ID));
    data.append(",");
    data.append(QString::number(portVal));
    data.append(",");
    data.append(QString::number(label));
    data.append(";");
}

void SaveData::addHighlight(bool show, QColor color) {
    data.append("highlight:");
    data.append(QString::number(show));
    data.append(",");
    data.append(color.name());
    data.append(";");
}

void SaveData::addShowStepResult(bool show) {
    data.append("stepresult:");
    data.append(QString::number(show));
    data.append(";");
}

void SaveData::addFromItem(QPointF point) {
    data.append("fromitem:");
    data.append(QString::number(point.rx()));
    data.append(",");
    data.append(QString::number(point.ry()));
    data.append(";");
}

void SaveData::addToItem(QPointF point, int portID) {
    data.append("toitem:");
    data.append(QString::number(point.rx()));
    data.append(",");
    data.append(QString::number(point.ry()));
    data.append(",");
    data.append(QString::number(portID));
    data.append(";");
}

void SaveData::addCounter(int counter) {
    data.append("counter:");
    data.append(QString::number(counter));
    data.append(";");
}

void SaveData::addBackgroundID(int id) {
    data.append("backgroundid:");
    data.append(QString::number(id));
    data.append(";");
}

QString SaveData::getObjectName() {
    QString name;
    foreach(QString att, atts){
        if(att.startsWith("object:")){
            name = QStringRef(&att, 7, att.size() - 7).toString();
            break;
        }
    }
    return name;
}

QPointF SaveData::getPos() {
    QPointF pos;
    foreach(QString att, atts){
        if(att.startsWith("pos:")){
            QString num = att.right(att.size() - 4);
            value = num.split(",");
            pos.setX(value.at(0).toDouble());
            pos.setY(value.at(1).toDouble());
            break;
        }
    }
    return pos;
}

QFont SaveData::getFont() {
    QFont font;
    foreach(QString att, atts){
        if(att.startsWith("font:")){
            QString num = att.right(att.size() - 5);
            value = num.split(",");
            font.setFamily(value.at(0));
            font.setPointSize(value.at(1).toDouble());
            break;
        }
    }
    return font;
}

QString SaveData::getTextData() {
    QString text;
    foreach(QString att, atts){
        if(att.startsWith("text:")){
            text = QStringRef(&att, 5, att.size() - 5).toString();
            break;
        }
    }
    return text;
}

int SaveData::getItemType() {
    int type = 0;
    foreach(QString att, atts){
        if(att.startsWith("itemtype:")){
            type = QStringRef(&att, 9, att.size() - 9).toInt();
            break;
        }
    }
    return type;
}

void SaveData::getOutputPort(double &portVal, bool &label) {
    // inicializace
    portVal = 0;
    label = false;
    foreach(QString att, atts){
        if (att.startsWith("portout:")) {
            QString num = att.right(att.size() - 8);
            value = num.split(",");
            portVal = value.at(0).toDouble();
            label = bool(value.at(1).toInt());
            break;
        }
    }
}

void SaveData::getInputPort(int ID, double &portVal, bool &label) {
    // inicializace
    portVal = 0;
    label = false;
    foreach(QString att, atts){
        if (att.startsWith("portin:")) {
            QString num = att.right(att.size() - 7);
            value = num.split(",");
            if (value.at(0).toInt() == ID) {
                portVal = value.at(1).toDouble();
                label = bool(value.at(2).toInt());
                break;
            }
        }
    }
}

void SaveData::getHighlight(bool &show, QColor &color) {
    // inicializace
    show = false;
    color = QColor();
    foreach(QString att, atts){
        if(att.startsWith("highlight:")){
            QString num = att.right(att.size() - 10);
            value = num.split(",");
            show = bool(value.at(0).toInt());
            color = QColor(value.at(1));
            break;
        }
    }
}

bool SaveData::getShowStepResult() {
    bool show = false;
    foreach(QString att, atts){
        if(att.startsWith("stepresult:")){
            show = bool(QStringRef(&att, 11, att.size() - 11).toInt());
            break;
        }
    }
    return show;
}

QPointF SaveData::getFromItem() {
    QPointF pos;
    foreach(QString att, atts){
        if(att.startsWith("fromitem:")){
            QString num = att.right(att.size() - 9);
            value = num.split(",");
            pos.setX(value.at(0).toDouble());
            pos.setY(value.at(1).toDouble());
            break;
        }
    }
    return pos;
}

QPointF SaveData::getToItem(int &portID) {
    portID = 0;
    QPointF pos;
    foreach(QString att, atts){
        if(att.startsWith("toitem:")){
            QString num = att.right(att.size() - 7);
            value = num.split(",");
            pos.setX(value.at(0).toDouble());
            pos.setY(value.at(1).toDouble());
            portID = value.at(2).toInt();
            break;
        }
    }
    return pos;
}

int SaveData::getCounter() {
    int cnt = 0;
    foreach(QString att, atts){
        if(att.startsWith("counter:")){
            cnt = QStringRef(&att, 8, att.size() - 8).toInt();
            break;
        }
    }
    return cnt;
}

QList<QPointF> SaveData::getPosList() {
    QList<QPointF> posList;
        QStringList positions = atts[1].split("&");
        QPointF p;
        foreach(QString pos, positions){
            value = pos.split(",");
            if (value.size() > 1) {
                posList << QPointF(value.at(0).toDouble(), value.at(1).toDouble());
            }
        }
    return posList;
}

int SaveData::getBackgroundID() {
    int id = 0;
    foreach(QString att, atts){
        if(att.startsWith("backgroundid:")){
            id = QStringRef(&att, 13, att.size() - 13).toInt();
            break;
        }
    }
    return id;
}