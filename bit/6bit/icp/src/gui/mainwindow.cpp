/**
 * @file    mainwindow.cpp
 * @author  Tomáš Aubrecht (xaubre02)
 * @date    květen 2018
 * @brief   Hlavní okno grafického rozhraní aplikace.
 */

#include "editornote.h"
#include "editorscene.h"
#include "mainwindow.h"

#include <QtWidgets>

/**
 * @brief konstanta uchovávající ID objektu poznámky
 */
const int noteID = 42;


MainWindow::MainWindow() {
    // vytvoreni akci, menu, toolbaru
    createActions();
    createMenus();
    createToolBox();
    stepCnt = 0;
    firstChange = true;

    // propojeni signalu s danymi funkcemi
    scene = new EditorScene(editMenu, this);
    scene->setSceneRect(QRectF(0, 0, 5000, 5000));
    connect(scene, SIGNAL(itemInserted(EditorItem*)),        this, SLOT(itemInserted(EditorItem*)));
    connect(scene, SIGNAL(noteInserted(QGraphicsTextItem*)), this, SLOT(noteInserted(QGraphicsTextItem*)));
    connect(scene, SIGNAL(noteSelected(QGraphicsItem*)),     this, SLOT(noteSelected(QGraphicsItem*)));
    connect(scene, SIGNAL(selectionChanged()),               this, SLOT(selectionHasChanged()));
    connect(scene, SIGNAL(changed(QList<QRectF>)),           this, SLOT(sceneHasChanged()));
    createToolBars();

    // vytvoreni layoutu
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(toolBox);
    view = new QGraphicsView(scene);
    layout->addWidget(view);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    setCentralWidget(widget);
    setWindowTitle("BlockEditor");

    // Vychozi pozadi
    QAbstractButton * background = backgroundGroup->buttons()[0];
    background->setChecked(true);
    backgroundGroupClicked(background);

    // nacteni nastaveni z predchozi relace
    loadSettings();
    unsavedChanges= false;
    statusBar()->showMessage(tr("Ready"));
}


void MainWindow::closeEvent(QCloseEvent *event) {
    if (confirmChanges()) {
        // ulozeni nastaveni a exit
        saveSettings();
        event->accept();
    } else {
        // exit zrusen
        event->ignore();
    }
}


void MainWindow::createActions() {
    // File menu akce

    QIcon icon = QIcon::fromTheme("document-new", QIcon(":/images/new.png")); // vytvoreni ikony bud ze systemu a pokud nelze, tak ze souboru
    actionNew = new QAction(icon, tr("&New"), this); // vytvoreni akce
    actionNew->setShortcuts(QKeySequence::New); // nastaveni klavesove zkratky
    actionNew->setStatusTip(tr("Create a new file")); // nastaveni napovedy
    connect(actionNew, SIGNAL(triggered()), this, SLOT(newFile())); // propojeni akce s funkci, ktera se ma nasledne vykonat

    // stejne/podobne jako u predchozi akce
    icon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    actionOpen = new QAction(icon, tr("&Open..."), this);
    actionOpen->setShortcuts(QKeySequence::Open);
    actionOpen->setStatusTip(tr("Open a file"));
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(open()));

    icon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    actionSave = new QAction(icon, tr("&Save"), this);
    actionSave->setShortcuts(QKeySequence::Save);
    actionSave->setStatusTip(tr("Save the current file"));
    actionSave->setEnabled(false);
    connect(actionSave, SIGNAL(triggered()), this, SLOT(save()));

    icon = QIcon::fromTheme("document-save-as", QIcon(":/images/save.png"));
    actionSaveAs = new QAction(icon, tr("&SaveAs..."), this);
    actionSaveAs->setShortcut(tr("Ctrl+Shift+S"));
    actionSaveAs->setStatusTip(tr("Create a new save file"));
    connect(actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));

    icon = QIcon::fromTheme("application-exit", QIcon(":/images/exit.png"));
    actionExit = new QAction(icon, tr("&Exit"), this);
    actionExit->setShortcuts(QKeySequence::Quit);
    actionExit->setStatusTip(tr("Exit the application"));
    connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));

    // Edit menu akce
    icon = QIcon(":/images/calculate.png");
    actionCalculate = new QAction(icon, tr("&Calculate"), this);
    actionCalculate->setStatusTip(tr("Calculate actual block"));
    actionCalculate->setEnabled(false);
    connect(actionCalculate, SIGNAL(triggered()), this, SLOT(calculateBlock()));

    icon = QIcon(":/images/setinput.png");
    actionSetInput = new QAction(icon, tr("&Set input"), this);
    actionSetInput->setStatusTip(tr("Set input values of the actual block"));
    actionSetInput->setEnabled(false);
    connect(actionSetInput, SIGNAL(triggered()), this, SLOT(setBlockInput()));

    icon = QIcon(":/images/bringtofront.png");
    actionToFront = new QAction(icon, tr("Bring to &Front"), this);
    actionToFront->setShortcut(tr("Ctrl+F"));
    actionToFront->setStatusTip(tr("Bring selected item to the front"));
    actionToFront->setEnabled(false);
    connect(actionToFront, SIGNAL(triggered()), this, SLOT(bringToFront()));

    icon = QIcon(":/images/sendtoback.png");
    actionToBack = new QAction(icon, tr("Send to &Back"), this);
    actionToBack->setShortcut(tr("Ctrl+T"));
    actionToBack->setStatusTip(tr("Send selected item to the back"));
    actionToBack->setEnabled(false);
    connect(actionToBack, SIGNAL(triggered()), this, SLOT(sendToBack()));

    icon = QIcon(":/images/delete.png");
    actionDelete = new QAction(icon, tr("&Delete"), this);
    actionDelete->setShortcut(tr("Delete"));
    actionDelete->setStatusTip(tr("Delete selected item from editor"));
    actionDelete->setEnabled(false);
    connect(actionDelete, SIGNAL(triggered()), this, SLOT(deleteItem()));

    // Help menu akce
    icon = QIcon::fromTheme("help-about", QIcon(":/images/author.png"));
    actionAuthor = new QAction(icon, tr("&Author"), this);
    actionAuthor->setStatusTip(tr("Author of the BlockEditor"));
    connect(actionAuthor, SIGNAL(triggered()), this, SLOT(author()));

    icon = QIcon::fromTheme("help-contents", QIcon(":/images/help.png"));
    actionHelp = new QAction(icon, tr("&Help"), this);
    actionHelp->setShortcut(tr("F1"));
    actionHelp->setStatusTip(tr("BlockEditor Description"));
    connect(actionHelp, SIGNAL(triggered()), this, SLOT(help()));

    // Font akce
    icon = QIcon(":/images/bold.png");
    actionBold = new QAction(icon, tr("Bold"), this);
    actionBold->setCheckable(true);
    actionBold->setShortcut(tr("Ctrl+B"));
    actionBold->setStatusTip(tr("Bold text"));
    connect(actionBold, SIGNAL(triggered()), this, SLOT(fontHasChanged()));

    icon = QIcon(":/images/italic.png");
    actionItalic = new QAction(icon, tr("Italic"), this);
    actionItalic->setCheckable(true);
    actionItalic->setShortcut(tr("Ctrl+I"));
    actionItalic->setStatusTip(tr("Italic text"));
    connect(actionItalic, SIGNAL(triggered()), this, SLOT(fontHasChanged()));

    icon = QIcon(":/images/underline.png");
    actionUnderline = new QAction(icon, tr("Underline"), this);
    actionUnderline->setCheckable(true);
    actionUnderline->setShortcut(tr("Ctrl+U"));
    actionUnderline->setStatusTip(tr("Underline the text"));
    connect(actionUnderline, SIGNAL(triggered()), this, SLOT(fontHasChanged()));

    // Schema akce
    icon = QIcon(":/images/play.png");
    actionCalculateScheme = new QAction(icon, tr("Calculate"), this);
    actionCalculateScheme->setStatusTip(tr("Calculate Diagram"));
    actionCalculateScheme->setEnabled(false);
    connect(actionCalculateScheme, SIGNAL(triggered()), this, SLOT(calculateDiagram()));

    icon = QIcon(":/images/stop.png");
    actionRestartSteps = new QAction(icon, tr("Restart steps"), this);
    actionRestartSteps->setStatusTip(tr("Make step from the beginning"));
    actionRestartSteps->setEnabled(false);
    connect(actionRestartSteps, SIGNAL(triggered()), this, SLOT(restartSteps()));

    icon = QIcon(":/images/next.png");
    actionStepForward = new QAction(icon, tr("Next step"), this);
    actionStepForward->setStatusTip(tr("Next calculation step"));
    actionStepForward->setEnabled(false);
    connect(actionStepForward, SIGNAL(triggered()), this, SLOT(stepForward()));

    icon = QIcon(":/images/previous.png");
    actionStepBackwards = new QAction(icon, tr("Previous step"), this);
    actionStepBackwards->setStatusTip(tr("Previous calculation step"));
    actionStepBackwards->setEnabled(false);
    connect(actionStepBackwards, SIGNAL(triggered()), this, SLOT(stepBackwards()));
}


void MainWindow::createMenus() {
    // File menu
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(actionNew);
    fileMenu->addAction(actionOpen);
    fileMenu->addAction(actionSave);
    fileMenu->addAction(actionSaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction(actionExit);

    // Edit menu
    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(actionCalculate);
    editMenu->addAction(actionSetInput);
    editMenu->addSeparator();
    editMenu->addAction(actionToFront);
    editMenu->addAction(actionToBack);
    editMenu->addSeparator();
    editMenu->addAction(actionDelete);

    // Help menu
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(actionAuthor);
    helpMenu->addSeparator();
    helpMenu->addAction(actionHelp);
}


void MainWindow::createToolBox() {
    // vytvoreni skupiny tlacitek - muze byt stisknuto maximalne jedno v jednom okamziku
    blockGroup = new QButtonGroup(this);
    blockGroup->setExclusive(false);
    connect(blockGroup, SIGNAL(buttonClicked(int)), this, SLOT(blockGroupClicked(int)));

    // rozlozeni Unary blocks
    QGridLayout *unaryLayout = new QGridLayout;
    unaryLayout->addWidget(createCellWidget(tr("Not"),    EditorItem::Not),    0, 0);
    unaryLayout->addWidget(createCellWidget(tr("Sine"),   EditorItem::Sine),   0, 1);
    unaryLayout->addWidget(createCellWidget(tr("Cosine"), EditorItem::Cosine), 1, 0);

    unaryLayout->setRowStretch(2, 10);
    unaryLayout->setColumnStretch(2, 10);

    // rozlozeni Binary blocks
    QGridLayout *binaryLayout = new QGridLayout;
    binaryLayout->addWidget(createCellWidget(tr("Adder"),      EditorItem::Adder),      0, 0);
    binaryLayout->addWidget(createCellWidget(tr("Subtractor"), EditorItem::Subtractor), 0, 1);
    binaryLayout->addWidget(createCellWidget(tr("Multiplier"), EditorItem::Multiplier), 1, 0);
    binaryLayout->addWidget(createCellWidget(tr("Multiplier"), EditorItem::Divider),    1, 1);

    binaryLayout->setRowStretch(3, 10);
    binaryLayout->setColumnStretch(2, 10);

    // rozlozeni Logical blocks
    QGridLayout *logicalLayout = new QGridLayout;
    logicalLayout->addWidget(createCellWidget(tr("AND"), EditorItem::LogAND), 0, 0);
    logicalLayout->addWidget(createCellWidget(tr("OR"),  EditorItem::LogOR),  0, 1);
    logicalLayout->addWidget(createCellWidget(tr("XOR"), EditorItem::LogXOR), 1, 0);
    logicalLayout->addWidget(createCellWidget(tr("NOT"), EditorItem::LogNOT), 1, 1);

    logicalLayout->setRowStretch(3, 10);
    logicalLayout->setColumnStretch(2, 10);

    backgroundGroup = new QButtonGroup(this);
    connect(backgroundGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(backgroundGroupClicked(QAbstractButton*)));
    // rozlozeni pozadi
    QGridLayout *backgroundLayout = new QGridLayout;
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("No Grid"),    ":/images/background3.png"), 0, 0);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("White Grid"), ":/images/background1.png"), 1, 0);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("Gray Grid"),  ":/images/background2.png"), 2, 0);

    backgroundLayout->setRowStretch(3, 10);
    backgroundLayout->setColumnStretch(1, 10);

    // Widgety
    QWidget *unaryWidget = new QWidget;
    unaryWidget->setLayout(unaryLayout);

    QWidget *binaryWidget = new QWidget;
    binaryWidget->setLayout(binaryLayout);

    QWidget *logicalWidget = new QWidget;
    logicalWidget->setLayout(logicalLayout);

    QWidget *backgroundWidget = new QWidget;
    backgroundWidget->setLayout(backgroundLayout);

    // ToolBox
    toolBox = new QToolBox;
    toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    toolBox->setMinimumWidth(binaryWidget->sizeHint().width());
    toolBox->addItem(unaryWidget, tr("Unary blocks"));
    toolBox->addItem(binaryWidget, tr("Binary blocks"));
    toolBox->addItem(logicalWidget, tr("Logical blocks"));
    toolBox->addItem(backgroundWidget, tr("Background"));
}


void MainWindow::createToolBars() {
    // Ukazatel mysi
    QToolButton *pointerButton = new QToolButton;
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setIcon(QIcon(":/images/pointer.png"));
    pointerButton->setStatusTip("Interact with blocks");

    // ukazatel konektoru
    QToolButton *linePointerButton = new QToolButton;
    linePointerButton->setCheckable(true);
    linePointerButton->setIcon(QIcon(":/images/linepointer.png"));
    linePointerButton->setStatusTip("Connect two blocks");

    // skupina tlacitek pro mys a konektor
    pointerTypeGroup = new QButtonGroup(this);
    pointerTypeGroup->addButton(pointerButton, int(EditorScene::MoveItem));
    pointerTypeGroup->addButton(linePointerButton, int(EditorScene::InsertLine));
    connect(pointerTypeGroup, SIGNAL(buttonClicked(int)), this, SLOT(pointerGroupClicked(int)));

    // zoom editoru
    QComboBox *sceneScaleCombo = new QComboBox;
    QStringList scales;
    scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
    sceneScaleCombo->addItems(scales);
    sceneScaleCombo->setCurrentIndex(2);
    sceneScaleCombo->setStatusTip("Editor scale");
    connect(sceneScaleCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(sceneScaleChanged(QString)));

    // vkladani poznamek
    QToolButton *noteButton = new QToolButton;
    noteButton->setCheckable(true);
    blockGroup->addButton(noteButton, noteID);
    noteButton->setIcon(QIcon(":/images/note.png"));
    noteButton->setStatusTip("Add a note");

    // Font family
    fontCombo = new QFontComboBox();
    fontCombo->setStatusTip("Select font family");
    connect(fontCombo, SIGNAL(currentFontChanged(QFont)), this, SLOT(fontHasChanged()));

    // velikost fontu
    fontSizeCombo = new QComboBox;
    fontSizeCombo->setEditable(true);
    fontSizeCombo->setStatusTip("Select font size");
    for (int i = 8; i <= 32; i = i + 2) {
        fontSizeCombo->addItem(QString().setNum(i));
    }
    // kontrola velikosti fontu, pokud si uzivatel zada jiny nez z poskytnuteho vyberu
    QIntValidator *validator = new QIntValidator(2, 128, this);
    fontSizeCombo->setValidator(validator);
    connect(fontSizeCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(fontHasChanged()));

    // File ToolBar
    QToolBar *fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(actionNew);
    fileToolBar->addAction(actionOpen);
    fileToolBar->addAction(actionSave);

    // Edit ToolBar
    QToolBar *editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(actionCalculate);
    editToolBar->addAction(actionSetInput);
    editToolBar->addSeparator();
    editToolBar->addAction(actionToFront);
    editToolBar->addAction(actionToBack);
    editToolBar->addAction(actionDelete);

    // ToolBar ukazatelu
    QToolBar *pointerToolbar = addToolBar(tr("Pointer type"));
    pointerToolbar->addWidget(pointerButton);
    pointerToolbar->addWidget(linePointerButton);
    pointerToolbar->addWidget(sceneScaleCombo);

    // Toolbar poznamek
    QToolBar *noteToolBar = addToolBar(tr("Note"));
    noteToolBar->addWidget(noteButton);
    noteToolBar->addSeparator();
    noteToolBar->addWidget(fontCombo);
    noteToolBar->addWidget(fontSizeCombo);
    noteToolBar->addSeparator();
    noteToolBar->addAction(actionBold);
    noteToolBar->addAction(actionItalic);
    noteToolBar->addAction(actionUnderline);

    // Toolbar schématu
    QToolBar *diagramToolBar = addToolBar(tr("Diagram"));
    diagramToolBar->addAction(actionStepBackwards);
    diagramToolBar->addAction(actionRestartSteps);
    diagramToolBar->addAction(actionStepForward);
    diagramToolBar->addSeparator();
    diagramToolBar->addAction(actionCalculateScheme);
}


void MainWindow::loadSettings() {
    // konfiguracni soubor je ulozen na stejnem miste, jako je spustitelny soubor aplikace
    QString curPath = QDir::toNativeSeparators(QFileInfo(QCoreApplication::applicationFilePath()).absoluteDir().canonicalPath());
    QString filename = curPath + QDir::separator() + "config.ini";
    // nacteni nastaveni z .ini souboru
    QSettings settings(filename, QSettings::IniFormat);
    QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    // prvni beh aplikace (azdny .ini soubor)
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2, (availableGeometry.height() - height()) / 2);
    // .ini soubor existuje a obsahuje potrebna data
    } else {
        restoreGeometry(geometry);
    }
    // nacteni souboru, na kterem se delalo naposled
    curFile = settings.value("session", QString()).toString();
    if (curFile.size() > 0) {
        loadFile(curFile);
    }
}


void MainWindow::saveSettings() {
    // konfiguracni soubor je ulozen na stejnem miste, jako je spustitelny soubor aplikace
    QString curPath = QDir::toNativeSeparators(QFileInfo(QCoreApplication::applicationFilePath()).absoluteDir().canonicalPath());
    QString filename = curPath + QDir::separator() + "config.ini";
    // ulozeni nastaveni do .ini souboru
    QSettings settings(filename, QSettings::IniFormat);
    settings.clear();
    settings.setValue("geometry", saveGeometry());
    // ulozeni 'relace'
    if (curFile.size() > 0)
        settings.setValue("session", curFile);
}


bool MainWindow::confirmChanges() {
    // pokud neni co uklada
    if (!unsavedChanges)
        return true;

    const QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("BlockEditor"), tr("The diagram has been modified.\n"
                                                                                             "Do you want to save your changes?"),
                                                                 QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
        // ulozeni zmen
        case QMessageBox::Save:
            return save();
        // zruseni akce
        case QMessageBox::Cancel:
            return false;
        // zahozeni zmen
        default:
            unsavedChanges = false;
            return true;
    }
}


void MainWindow::loadFile(const QString &fileName) {
    QFile file(fileName);
    // nacteni souboru
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("BlockEditor"),
                             tr("Cannot read file %1: %2.").arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream in(&file);
    QString line;
    while(true) {
        // cteni po radku
        line = in.readLine();
        if (line.size() == 0) break;

        SaveData loader(line);
        QString object = loader.getObjectName();
        if (object == "EditorNote" || object == "EditorItem" || object == "Connector") {
            // nacteni objektu do sceny
            scene->loadObject(object, loader);
        }
        // nacteni pomocnych promennych pro krokovani vypoctu
        else if (object == "stepCnt") {
            stepCnt = loader.getCounter();
        }
        else if (object == "prevStep") {
            prevStep = getEditorItemsFromScene(loader.getPosList());
        }
        else if (object == "curStep") {
            curStep = getEditorItemsFromScene(loader.getPosList());
        }
        else if (object == "nextStep") {
            nextStep = getEditorItemsFromScene(loader.getPosList());
        }
        // nastaveni pozadi
        else if (object == "background") {
            int id = loader.getBackgroundID();
            QAbstractButton *background = backgroundGroup->button(id);
            background->setChecked(true);
            backgroundGroupClicked(background);
        }
    }
    // nastaveni aktualniho souboru
    curFile = fileName;
    statusBar()->showMessage(tr("File successfully loaded"), 2000);
}


bool MainWindow::saveFile(const QString &fileName) {
    QFile file(fileName);
    // otevreni souboru
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("BlockEditor"),
                             tr("Cannot write file %1: %2.").arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return false;
    }

    QTextStream out(&file);
    // ulozeni grafickych objektu
    foreach (QGraphicsItem *item, scene->items()) {
        // ulozeni poznamek
        if (item->type() == EditorNote::Type) {
            out << qgraphicsitem_cast<EditorNote *>(item)->save() << "\n";
        }
        // ulozeni bloku
        else if (item->type() == EditorItem::Type) {
            out << qgraphicsitem_cast<EditorItem *>(item)->save() << "\n";
        }
        // ulozeni spoje
        else if (item->type() == Connector::Type) {
            out << qgraphicsitem_cast<Connector *>(item)->save() << "\n";
        }
    }
    // ulozeni pomocnych promennych pro krokovani vypoctu
    SaveData aux;
    aux.setObjectName("stepCnt");
    aux.addCounter(stepCnt);
    out << aux.toString() << "\n";

    aux.clear();
    aux.setObjectName("prevStep");
    foreach(EditorItem *item, prevStep) {
        aux.appendPos(item->scenePos());
    }
    out << aux.toString() << "\n";

    aux.clear();
    aux.setObjectName("curStep");
    foreach(EditorItem *item, curStep) {
        aux.appendPos(item->scenePos());
    }
    out << aux.toString() << "\n";

    aux.clear();
    aux.setObjectName("nextStep");
    foreach(EditorItem *item, nextStep) {
        aux.appendPos(item->scenePos());
    }
    out << aux.toString() << "\n";

    // ulozeni pozadi
    aux.clear();
    aux.setObjectName("background");
    // ziskani aktualniho zvoleneho pozadi
    foreach (QAbstractButton *myButton, backgroundGroup->buttons()) {
        if (myButton->isChecked()) {
            aux.addBackgroundID(backgroundGroup->id(myButton));
            break;
        }
    }
    out << aux.toString() << "\n";

    curFile = fileName;
    statusBar()->showMessage(tr("File successfully saved"), 2000);
    return true;
}


QWidget *MainWindow::createCellWidget(const QString &text, EditorItem::EditorItemType type) {
    // ziskani ikony bloku
    EditorItem item(type, editMenu);
    QIcon icon(item.icon());

    // vytvoreni tlacitka
    QToolButton *button = new QToolButton;
    button->setIcon(icon);
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    blockGroup->addButton(button, int(type));
    // vytvoreni rozlozeni
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}


QWidget *MainWindow::createBackgroundCellWidget(const QString &text, const QString &image) {
    // vytvoreni tlacitka
    QToolButton *button = new QToolButton;
    QIcon icon = QIcon(image);

    button->setText(text);
    button->setIcon(icon);
    button->setIconSize(QSize(42, 42));

    button->setCheckable(true);
    backgroundGroup->addButton(button);
    // vytvoreni rozlozeni
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}


QList<EditorItem *> MainWindow::getEditorItemsFromScene(QList<QPointF> points) {
    QList<EditorItem *> itemList;
    // kontrola vsech grafickych objektu ve scene
    foreach(QGraphicsItem *sceneItem, scene->items()) {
        // hledaji se pouze objekty 'EditorItem'
        if (sceneItem->type() == EditorItem::Type) {
            // prochazeni vsech poskytnutych bodu
            foreach (QPointF p, points) {
                if (sceneItem->scenePos() == p) {
                    itemList << qgraphicsitem_cast<EditorItem *>(sceneItem);
                }
            }
        }
    }
    return itemList;
}


void MainWindow::newFile() {
    if (confirmChanges()) {
        // Odstrani vsechny objekty
        foreach (QGraphicsItem *item, scene->items()) {
            if (item->type() == EditorItem::Type || item->type() == EditorNote::Type || item->type() == Connector::Type) {
                scene->removeItem(item);
                delete item;
            }
        }
        // nastaveni hodnot flagu
        curFile = QString();
        firstChange = true;
        unsavedChanges = false;
        actionSave->setEnabled(false);
    }
}


void MainWindow::open() {
    if (confirmChanges()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        // nacteni daneho souboru
        if (!fileName.isEmpty()){
            // odstraneni aktualniho schematu
            newFile();
            loadFile(fileName);
            // prepnuti sceny do stavu premistovani objektu
            scene->setMode(EditorScene::MoveItem);
            pointerTypeGroup->button(int(EditorScene::MoveItem))->setChecked(true);
            foreach (QAbstractButton *button, blockGroup->buttons()) {
                button->setChecked(false);
            }
        }
        // nastaveni hodnot flagu
        firstChange = true;
        unsavedChanges = false;
        actionSave->setEnabled(false);
    }
}


bool MainWindow::save() {
    bool saved;
    if (curFile.isEmpty()) // prvni ulozeni
        saved = saveAs();
    else // soubor pro ulozeni byl jiz specifikovan
        saved = saveFile(curFile);

    if (saved) {
        unsavedChanges = false;
        actionSave->setEnabled(false);
    }

    return saved;
}


bool MainWindow::saveAs() {
    // inicializace dialogu
    QFileDialog dialog(this);
    dialog.selectFile("new");
    dialog.setDefaultSuffix("dia");
    // filtr pripon
    QStringList filters;
    filters << "Block Diagram (*.dia)"
            << "Normal text file (*.txt)"
            << "All types (*.*)";
    dialog.setNameFilters(filters);
    // nastveni dialogoveho okna
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return false;
    else
        return saveFile(dialog.selectedFiles().first());
}


void MainWindow::author() {
    QMessageBox::about(this, tr("Author"),
                       tr("<b>Tomas Aubrecht</b> (xaubre02)"));
}


void MainWindow::help() {
    QMessageBox::about(this, tr("About"),
                       tr("ICP Project 2018"));
}


void MainWindow::blockGroupClicked(int id) {
    // vymacknuti ostatnich tlacitek
    foreach (QAbstractButton *button, blockGroup->buttons()) {
        if (blockGroup->button(id) != button)
            button->setChecked(false);
    }
    // zmena modu sceny na zaklade id tlacitka, ktere bylo stisknuto
    if (id == noteID) {
        scene->setMode(EditorScene::InsertNote);
    } else {
        scene->setItemType(EditorItem::EditorItemType(id));
        scene->setMode(EditorScene::InsertItem);
    }
}


void MainWindow::backgroundGroupClicked(QAbstractButton *clickedButton) {
    // zvoleni pozadi na zaklade id tlacitka, tkere bylo stisknuto
    QString text = clickedButton->text();
    if (text == tr("White Grid"))
        scene->setBackgroundBrush(QPixmap(":/images/background1.png"));
    else if (text == tr("Gray Grid"))
        scene->setBackgroundBrush(QPixmap(":/images/background2.png"));
    else
        scene->setBackgroundBrush(QPixmap(":/images/background3.png"));

    // aktualizace vykresleni
    scene->update();
    view->update();
}


void MainWindow::itemInserted(EditorItem *item) {
    // aktivace tlacitek pro vypocty
    actionCalculateScheme->setEnabled(true);
    actionStepForward->setEnabled(true);
    actionRestartSteps->setEnabled(true);
    actionStepBackwards->setEnabled(true);

    // aktualizace hodnot flagu
    pointerTypeGroup->button(int(EditorScene::MoveItem))->setChecked(true);
    scene->setMode(EditorScene::Mode(pointerTypeGroup->checkedId()));
    blockGroup->button(int(item->editorItemType()))->setChecked(false);
}


void MainWindow::noteSelected(QGraphicsItem *note) {
    EditorNote *editorNote = qgraphicsitem_cast<EditorNote *>(note);

    // nastaveni vlastnosti fontu editoru podle aktualni poznamky
    QFont font = editorNote->font();
    fontCombo->setCurrentFont(font);
    fontSizeCombo->setEditText(QString().setNum(font.pointSize()));
    actionBold->setChecked(font.weight() == QFont::Bold);
    actionItalic->setChecked(font.italic());
    actionUnderline->setChecked(font.underline());
}


void MainWindow::noteInserted(QGraphicsTextItem *) {
    // vrati scenu do modu v jakem byla pred vlozenim poznamky
    blockGroup->button(noteID)->setChecked(false);
    scene->setMode(EditorScene::Mode(pointerTypeGroup->checkedId()));
}


void MainWindow::calculateBlock() {
    EditorItem *item = qgraphicsitem_cast<EditorItem *>(scene->selectedItems().first());
    // nastaveni vstupu
    double min, max;
    int decimals;
    // pokud se jedna o booleanovske hodnoty, muze se jednat pouze o hodnoty 1 nebo 0
    if (item->editorItemType() == EditorItem::LogAND ||
        item->editorItemType() == EditorItem::LogOR  ||
        item->editorItemType() == EditorItem::LogNOT ||
        item->editorItemType() == EditorItem::LogXOR) {
        min = 0.0;
        max = 1.0;
        decimals = 0;
    } else {
        min = -2147483647;
        max = 2147483647;
        decimals = 2;
    }
    bool confirmed;
    for (auto port: item->getInputPorts()) {
        // Port musi mit zadane vstupy
        if (port->hasCustomizableValue() && !port->hasValueSet()) {
            QString name("Port IN" + QString::number(port->index() + 1) + ":");
            double number = QInputDialog::getDouble(this, tr(item->getName().c_str()), tr(name.toStdString().c_str()), port->portValue(), min, max, decimals, &confirmed);
            if (confirmed) {
                // Nastavení hodnoty portu
                port->setValue(number);
                item->addInputLabel(port->index(), QString::number(number));
            } else {
                return;
            }
        }
    }

    if (item->calculate()){
        item->showHighlight(false);
        // zobraz vysledek, pokud vystupni port neni pripojen
        if (!item->outputPortConnected()) {
            item->displayResult();
        }
    } else {
        // chyba
        item->showHighlight(true, Qt::red);
        QMessageBox msgBox;
        msgBox.setText("Zero division!");
        msgBox.exec();
    }
}


void MainWindow::setBlockInput() {
    EditorItem *item = qgraphicsitem_cast<EditorItem *>(scene->selectedItems().first());
    double min, max;
    int decimals;
    // pokud se jedna o booleanovske hodnoty, muze se jednat pouze o hodnoty 1 nebo 0
    if (item->editorItemType() == EditorItem::LogAND ||
        item->editorItemType() == EditorItem::LogOR  ||
        item->editorItemType() == EditorItem::LogNOT ||
        item->editorItemType() == EditorItem::LogXOR) {
        min = 0.0;
        max = 1.0;
        decimals = 0;
    } else {
        min = -2147483647;
        max = 2147483647;
        decimals = 2;
    }

    bool confirmed;
    for (auto port: item->getInputPorts()) {
        // Pokud lze portu nastavit vstupni hodnotu
        if (port->hasCustomizableValue()) {
            QString name("Port IN" + QString::number(port->index() + 1) + ":");
            double number = QInputDialog::getDouble(this, tr(item->getName().c_str()), tr(name.toStdString().c_str()), port->portValue(), min, max, decimals, &confirmed);
            if (confirmed) {
                // Nastavení hodnoty portu
                port->setValue(number);
                item->addInputLabel(port->index(), QString::number(number));
            }
        }
    }
}


bool MainWindow::detectCycle() {
    bool cycle = false;
    foreach(QGraphicsItem *item, scene->items()) {
        if (item->type() == EditorItem::Type){
            // zjisteni, zdali se nektery blok muze dostat do cyklu pri vypoctu diagramu
            EditorItem *editorItem = qgraphicsitem_cast<EditorItem *>(item);
            if (editorItem->isGoingToCycle()) {
                // oznaceni bloku, ktery se zacykli
                editorItem->showHighlight(true, Qt::red);
                cycle = true;
            }
        }
    }
    return cycle;
}


void MainWindow::calculateDiagram() {
    // Inicializace
    restartSteps();
    // Nalezeni cyklu
    if (detectCycle()) {
        QMessageBox msgBox;
        msgBox.setText("Cycle detected!");
        msgBox.exec();
        return;
    }
    // Kontrola pripojenych vstupu
    foreach(QGraphicsItem *item, scene->items()) {
        if (item->type() == EditorItem::Type){
            EditorItem *editorItem = qgraphicsitem_cast<EditorItem *>(item);
            double min, max;
            int decimals;
            if (editorItem->editorItemType() == EditorItem::LogAND ||
                editorItem->editorItemType() == EditorItem::LogOR  ||
                editorItem->editorItemType() == EditorItem::LogNOT ||
                editorItem->editorItemType() == EditorItem::LogXOR) {
                min = 0.0;
                max = 1.0;
                decimals = 0;
            } else {
                min = -2147483647;
                max = 2147483647;
                decimals = 2;
            }
            bool confirmed;
            foreach(auto port, editorItem->getInputPorts()) {
                // Pokud port nema nastavenou hodnotu
                if (!port->hasValueSet()) {
                    QString name("Port IN" + QString::number(port->index() + 1) + ":");
                    double number = QInputDialog::getDouble(this, tr(editorItem->getName().c_str()), tr(name.toStdString().c_str()), port->portValue(), min, max, decimals, &confirmed);
                    if (confirmed) {
                        // Nastavení hodnoty portu
                        port->setValue(number);
                        editorItem->addInputLabel(port->index(), QString::number(number));
                    } else {
                        return;
                    }
                }
            }
        }
    }

    // Zacatek vypoctu
    foreach(QGraphicsItem *item, scene->items()) {
        if (item->type() == EditorItem::Type){
            EditorItem *editorItem = qgraphicsitem_cast<EditorItem *>(item);
            if (editorItem->isInputBlock()) {
                editorItem->startCalculation();
            }
        }
    }
    // Zobrazeni vysledeku, pokud vystupni port neni pripojen
    foreach(QGraphicsItem *item, scene->items()) {
        if (item->type() == EditorItem::Type){
            EditorItem *editorItem = qgraphicsitem_cast<EditorItem *>(item);
            if (!editorItem->outputPortConnected()) {
                editorItem->displayResult();
            }
        }
    }
}


void MainWindow::restartSteps() {
    // Schovani zvyrazneni a vysledku
    foreach(QGraphicsItem *item, scene->items()) {
        if (item->type() == EditorItem::Type) {
            EditorItem *editorItem = qgraphicsitem_cast<EditorItem *>(item);
            editorItem->showStepResult(false);
            editorItem->showHighlight(false);
            editorItem->hideResult();
        }
    }

    // vycisteni seznamu
    prevStep.clear();
    curStep.clear();
    nextStep.clear();
    stepCnt = 0;
}


void MainWindow::stepForward() {
    // prvni krok
    if (stepCnt == 0) {
        // Nalezeni cyklu
        if (detectCycle()) {
            QMessageBox msgBox;
            msgBox.setText("Cycle detected!");
            msgBox.exec();
            return;
        }
        // vsechny objekty ve scene
        foreach(QGraphicsItem *item, scene->items()) {
            // blok editoru
            if (item->type() == EditorItem::Type){
                EditorItem *editorItem = qgraphicsitem_cast<EditorItem *>(item);
                double min, max;
                int decimals;
                if (editorItem->editorItemType() == EditorItem::LogAND ||
                    editorItem->editorItemType() == EditorItem::LogOR  ||
                    editorItem->editorItemType() == EditorItem::LogNOT ||
                    editorItem->editorItemType() == EditorItem::LogXOR) {
                    min = 0.0;
                    max = 1.0;
                    decimals = 0;
                } else {
                    min = -2147483647;
                    max = 2147483647;
                    decimals = 2;
                }
                bool confirmed;
                foreach(auto port, editorItem->getInputPorts()) {
                    // Pokud port nema nastavenou hodnotu
                    if (!port->hasValueSet()) {
                        QString name("Port IN" + QString::number(port->index() + 1) + ":");
                        double number = QInputDialog::getDouble(this, tr(editorItem->getName().c_str()), tr(name.toStdString().c_str()), port->portValue(), min, max, decimals, &confirmed);
                        if (confirmed) {
                            // Nastavení hodnoty portu
                            port->setValue(number);
                            editorItem->addInputLabel(port->index(), QString::number(number));
                        } else {
                            return;
                        }
                    }
                }
                // ulozeni vstupnich bloku
                if (editorItem->isInputBlock()) {
                    curStep << editorItem;
                }
            }
        }
    } else {
        // zruseni highlightu
        foreach(EditorItem *item, curStep) {
            item->showHighlight(false);
            item->showStepResult(false);
        }

        prevStep = curStep;
        curStep = nextStep;
        nextStep.clear();
    }
    // reset
    if (curStep.size() == 0) {
        restartSteps();
        return;
    }

    // Vypocet bloku
    foreach(EditorItem *item, curStep) {
        if (item->calculate()){
            item->showHighlight(true, Qt::green);
            item->showStepResult(true);
            // zobraz vysledek, pokud vystupni port neni pripojen
            if (!item->outputPortConnected()) {
                item->displayResult();
            }
            // pridani nasledujicich kroku
            foreach (EditorItem * nextItem, item->getOutputConnections()) {
                int index = nextStep.indexOf(nextItem);
                // odstraneni duplicit
                if (index == -1){
                    nextStep << nextItem;
                }
            }
        } else {
            item->showHighlight(true, Qt::red);
            QMessageBox msgBox;
            msgBox.setText("Zero division!");
            msgBox.exec();
        }
    }

    // zvyseni poctu kroku
    stepCnt++;
}


void MainWindow::stepBackwards() {
    // zpatky na zacatku
    if (stepCnt <= 1) {
        restartSteps();
        return;
    }

    // Schovani highlightu a vysledku
    foreach(EditorItem *item, curStep) {
        item->showHighlight(false);
        item->showStepResult(false);
        item->hideResult();
    }

    nextStep = curStep;
    curStep = prevStep;
    prevStep.clear();

    // Vypocet bloku
    foreach(EditorItem *item, curStep) {
        if (item->calculate()){
            item->showHighlight(true, Qt::green);
            item->showStepResult(true);
            // zobraz vysledek, pokud vystupni port neni pripojen
            if (!item->outputPortConnected()) {
                item->displayResult();
            }
            // pridani predchazejiciho kroku
            foreach (EditorItem * prevItem, item->getInputConnections()) {
                int index = prevStep.indexOf(prevItem);
                // odstraneni duplicit
                if (index == -1){
                    prevStep << prevItem;
                }
            }
        } else {
            item->showHighlight(true, Qt::red);
            QMessageBox msgBox;
            msgBox.setText("Zero division!");
            msgBox.exec();
        }
    }

    stepCnt--;
}


void MainWindow::sendToBack() {
    // pokud neni vybran zadny objekt (nemelo by se sem vubec dostat)
    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    // objekty, ktere se prekryvaji
    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    // naleznuti takove Z-hodnoty, aby byl objekt vzadu
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() <= zValue && item->type() == EditorItem::Type) {
            zValue = item->zValue() - 1.0;
        }
    }
    selectedItem->setZValue(zValue);
    // Aktualizace vstupu a vystupu bloku
    if (selectedItem->type() == EditorItem::Type){
        qgraphicsitem_cast<EditorItem *>(selectedItem)->setLabelsZValue(zValue);
    }
}


void MainWindow::bringToFront() {
    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    // naleznuti takove Z-hodnoty, aby byl objekt na popredi
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() >= zValue && item->type() == EditorItem::Type) {
            zValue = item->zValue() + 1.0;
        }
    }
    selectedItem->setZValue(zValue);
    // Aktualizace vstupu a vystupu bloku
    if (selectedItem->type() == EditorItem::Type){
        qgraphicsitem_cast<EditorItem *>(selectedItem)->setLabelsZValue(zValue);
    }
}


void MainWindow::deleteItem() {
    QGraphicsItem *item = scene->selectedItems().first();
    if (item->type() == Connector::Type) {
        Connector *connector = qgraphicsitem_cast<Connector *>(item);
        connector->startItem()->removeConnector(connector);
        connector->endItem()->removeConnector(connector);
    } else if (item->type() == EditorItem::Type) {
        // odstraneni spoju
        EditorItem *editorItem = qgraphicsitem_cast<EditorItem *>(item);
        editorItem->removeConnectors();
        // odstraneni z predchoziho kroku
        int index = prevStep.indexOf(editorItem);
        if (index != -1) prevStep.removeAt(index);
        // odstraneni z aktualniho kroku
        index = curStep.indexOf(editorItem);
        if (index != -1) curStep.removeAt(index);
        // odstraneni z nasledujiciho kroku
        index = nextStep.indexOf(editorItem);
        if (index != -1) nextStep.removeAt(index);
    }
    scene->removeItem(item);
    delete item;

    // ve scene musi byt alespon jeden blok, aby se mohlo pocitat
    bool contains = false;
    foreach (QGraphicsItem *item, scene->items()) {
        if (item->type() == EditorItem::Type) {
            contains = true;
            break;
        }
    }
    if (!contains) {
        actionCalculateScheme->setEnabled(false);
        actionStepForward->setEnabled(false);
        actionRestartSteps->setEnabled(false);
        actionStepBackwards->setEnabled(false);
    }
}


void MainWindow::fontHasChanged() {
    // aktualizace fontu
    QFont font = fontCombo->currentFont();
    font.setPointSize(fontSizeCombo->currentText().toInt());
    font.setWeight(actionBold->isChecked() ? QFont::Bold : QFont::Normal);
    font.setItalic(actionItalic->isChecked());
    font.setUnderline(actionUnderline->isChecked());

    scene->setFont(font);
}


void MainWindow::pointerGroupClicked(int) {
    scene->setMode(EditorScene::Mode(pointerTypeGroup->checkedId()));
}


void MainWindow::sceneScaleChanged(const QString &scale) {
    // prepocitani priblizeni editoru
    double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
    QMatrix oldMatrix = view->matrix();
    view->resetMatrix();
    view->translate(oldMatrix.dx(), oldMatrix.dy());
    view->scale(newScale, newScale);
}


void MainWindow::selectionHasChanged() {
    if (scene->selectedItems().size() > 0) {
        actionToFront->setEnabled(true);
        actionToBack->setEnabled(true);
        actionDelete->setEnabled(true);
        // Pokud se jedná o výpočetní blok
        if (scene->selectedItems().first()->type() == EditorItem::Type) {
            actionCalculate->setEnabled(true);
            // Konverze na EditorItem
            EditorItem *item = qgraphicsitem_cast<EditorItem *>(scene->selectedItems().first());
            // Pokud má blok nějaké nepřipojené vstupní porty
            if (!item->inputPortsConnected())
                actionSetInput->setEnabled(true);
        }
    } else {
        // jinak se deaktivuji tlacitka pro praci s objekty ve scene
        actionToFront->setEnabled(false);
        actionToBack->setEnabled(false);
        actionDelete->setEnabled(false);
        actionCalculate->setEnabled(false);
        actionSetInput->setEnabled(false);
    }
}

void MainWindow::sceneHasChanged() {
    // prvni zmena je vzdy vykresleni sceny
    if (firstChange) {
        firstChange = false;
        return;
    }
    // jsou zde neulozene zmeny -> muzeme ulozit
    unsavedChanges = true;
    actionSave->setEnabled(true);
}
