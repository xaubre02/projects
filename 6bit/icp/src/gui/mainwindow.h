/**
 * @file    mainwindow.h
 * @author  Tomáš Aubrecht (xaubre02)
 * @date    květen 2018
 * @brief   Hlavní okno grafického rozhraní aplikace.
 */

#pragma once

#include "editoritem.h"
#include <QMainWindow>
// deklarace tříd
class EditorScene;
class QButtonGroup;
class QGraphicsView;
class QFontComboBox;
class QComboBox;
class QAbstractButton;
class QToolButton;
class QToolBox;


/**
 * @brief třída reprezentující hlavní okno grafického rozhraní aplikace
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief konstruktor třídy
     */
    MainWindow();

    /**
     * @brief destruktor třídy
     */
    ~MainWindow() {}

protected:
    /**
     * @brief zpracovává událost při pokusu o zavření aplikace
     * @param event událost
     */
    void closeEvent(QCloseEvent *event) override;

private:

    /**
     * @brief vytvoří a inicializuje všechny akce
     */
    void createActions();

    /**
     * @brief vytvoří a inicializuje všechna menu
     */
    void createMenus();

    /**
     * @brief vytvoří a inicializuje toolbox
     */
    void createToolBox();

    /**
     * @brief vytvoří a inicializuje všechny toolbary
     */
    void createToolBars();

    /**
     * @brief načte konfiguraci aplikace ze souboru
     */
    void loadSettings();

    /**
     * @brief uloží konfiguraci aplikace do souboru
     */
    void saveSettings();

    /**
     * @brief zpracovává neuložené změny
     * @return vrací true, pokud nejsou žádné změny, pokud je uživatel uloží nebo zahodí, jinak false
     */
    bool confirmChanges();

    /**
     * @brief uložení aktuálního blokového schématu do souboru
     * @param fileName jméno souboru
     * @return úspěšnost akce
     */
    bool saveFile(const QString &fileName);

    /**
     * @brief načtení obsahu souboru
     * @param fileName jméno souboru
     */
    void loadFile(const QString &fileName);

    /**
     * @brief vytvoří widget představující tlačítko funkčního bloku s jeho ikonou
     * @param text název funkčního bloku
     * @param type typ funkčního bloku
     * @return vytvořený widget
     */
    QWidget *createCellWidget(const QString &text, EditorItem::EditorItemType type);

    /**
     * @brief vytvoří widget představující tlačítko pozadí
     * @param text název pozadí
     * @param image obrázek tlačítka
     * @return vytvořený widget
     */
    QWidget *createBackgroundCellWidget(const QString &text, const QString &image);

    /**
     * @brief pomocná funkce při načítání dat ze souboru
     * @param points seznam bodů
     * @return seznam funkčních bloků nacházejících se na daných bodech
     */
    QList<EditorItem *> getEditorItemsFromScene(QList<QPointF> points);

private slots:
    /**
     * @brief vytvoří nové schéma
     */
    void newFile();

    /**
     * @brief otevře soubor a načte z něj blokové schéma
     */
    void open();

    /**
     * @brief uloží aktuální blokové schéma
     */
    bool save();

    /**
     * @brief uloží aktuální blokové schéma do nového souboru
     */
    bool saveAs();

    /**
     * @brief zobrazí autora
     */
    void author();

    /**
     * @brief zobrazí popisek
     */
    void help();

    /**
     * @brief zpracovává kliknutí tlačítka myši na tlačítko nějakého bloku
     * @param id ID tlačítka
     */
    void blockGroupClicked(int id);

    /**
     * @brief zpracovává kliknutí tlačítka myši na nějaké tlačítko pozadí
     * @param button tlačítko
     */
    void backgroundGroupClicked(QAbstractButton *button);

    /**
     * @brief zpracovává událost vložení funkčního bloku do scény
     * @param item funkční blok
     */
    void itemInserted(EditorItem *item);

    /**
     * @brief zpracovává událost vložení poznámky do scény
     * @param note poznámka
     */
    void noteInserted(QGraphicsTextItem *note);

    /**
     * @brief zpracovává událost výběru poznámky
     * @param note poznámka
     */
    void noteSelected(QGraphicsItem *note);

    /**
     * @brief zpracovává změny fontu
     */
    void fontHasChanged();

    /**
     * @brief zpracovává kliknutí tlačítka myši na ukazatel či na konektor v editoru
     * @param id ID tlačítka
     */
    void pointerGroupClicked(int id);

    /**
     * @brief zpracovává událost změny zoomu scény
     * @param scale nová hodnota zoomu
     */
    void sceneScaleChanged(const QString &scale);

    /**
     * @brief zpracovává událost změny vybraného objektu
     */
    void selectionHasChanged();

    /**
     * @brief zpracovává událost změny grafické scény, kde nastaví flag neuložených změn
     */
    void sceneHasChanged();

    /**
     * @brief provede výpočet vybraného bloku
     */
    void calculateBlock();

    /**
     * @brief nastaví vstupní hodnoty vybraného bloku
     */
    void setBlockInput();

    /**
     * @brief přenese vybraný objekt na popředí
     */
    void bringToFront();

    /**
     * @brief přenese vybraný objekt na pozadí
     */
    void sendToBack();

    /**
     * @brief odstraní vybraný objekt
     */
    void deleteItem();

    /**
     * @brief detekuje cykly ve schématu
     */
    bool detectCycle();

    /**
     * @brief provede výpočet celého schématu
     */
    void calculateDiagram();

    /**
     * @brief restartuje krokování výpočtu
     */
    void restartSteps();

    /**
     * @brief provede krok vpřed v dílčím výpočtu
     */
    void stepForward();

    /**
     * @brief provede krok zpět v dílčím výpočtu
     */
    void stepBackwards();


private:
    /**
     * @brief menu obsahující úpravy schématu
     */
    QMenu *fileMenu;

    /**
     * @brief akce pro vytvoření nového schématu
     */
    QAction *actionNew;

    /**
     * @brief akce pro otevření souboru
     */
    QAction *actionOpen;

    /**
     * @brief akce pro uložení schématu do aktuálního souboru
     */
    QAction *actionSave;

    /**
     * @brief akce pro uložení schématu do nového souboru
     */
    QAction *actionSaveAs;

    /**
     * @brief akce pro ukončení editoru
     */
    QAction *actionExit;

    /**
     * @brief menu obsahující úpravy objektu
     */
    QMenu *editMenu;

    /**
     * @brief akce pro přenesení objektu na popředí
     */
    QAction *actionToFront;

    /**
     * @brief akce pro přenesení objektu na pozadí
     */
    QAction *actionToBack;

    /**
     * @brief akce pro odstranění bloku
     */
    QAction *actionDelete;

    /**
     * @brief akce pro výpočet jednoho bloku
     */
    QAction *actionCalculate;

    /**
     * @brief akce pro nastavení hodnoty vstupního portu
     */
    QAction *actionSetInput;

    /**
     * @brief menu obsahující autora a popisek
     */
    QMenu *helpMenu;

    /**
     * @brief akce pro zobrazení autora
     */
    QAction *actionAuthor;

    /**
     * @brief akce pro zobrazení popisku
     */
    QAction *actionHelp;

    /**
     * @brief akce pro výpočet celého schématu
     */
    QAction *actionCalculateScheme;

    /**
     * @brief akce pro resetování dílčích kroků
     */
    QAction *actionRestartSteps;

    /**
     * @brief akce pro dílčí krok výpočtu vpřed
     */
    QAction *actionStepForward;

    /**
     * @brief akce pro dílčí krok výpočtu zpět
     */
    QAction *actionStepBackwards;

    /**
     * @brief seznam objektů v předchozím krokuvýpočtu
     */
    QList<EditorItem *> prevStep;

    /**
     * @brief seznam objektů v aktuálním kroku výpočtu
     */
    QList<EditorItem *> curStep;

    /**
     * @brief seznam objektů v následujícím kroku výpočtu
     */
    QList<EditorItem *> nextStep;

    /**
     * @brief počítadlo dílčích kroků výpočtu
     */
    int stepCnt;

    /**
     * @brief toolbox pro vytvoření rozložení tlačítek editoru
     */
    QToolBox *toolBox;

    /**
     * @brief skupina tlačítek pro výběr funkčního bloku
     */
    QButtonGroup *blockGroup;

    /**
     * @brief skupina tlačítek pro změnu pozadí
     */
    QButtonGroup *backgroundGroup;


    /**
     * @brief grafická scéna
     */
    EditorScene *scene;

    /**
     * @brief widget pro zobrazení obsahu grafické scény
     */
    QGraphicsView *view;

    /**
     * @brief aktuální soubor
     */
    QString curFile;

    /**
     * @brief zoom grafické scény
     */
    QComboBox *sceneScaleCombo;

    /**
     * @brief velikost fontu
     */
    QComboBox *fontSizeCombo;

    /**
     * @brief font family
     */
    QFontComboBox *fontCombo;

    /**
     * @brief akce fontu pro jeho ztučnění
     */
    QAction *actionBold;

    /**
     * @brief akce fontu pro jeho naklonění
     */
    QAction *actionItalic;

    /**
     * @brief akce fontu pro jeho podtržení
     */
    QAction *actionUnderline;

    /**
     * @brief skupina tlačítek pro ukazatele editoru
     */
    QButtonGroup *pointerTypeGroup;

    /**
     * @brief flag první změny grafické scény
     */
    bool firstChange;

    /**
     * @brief flag neuložených změn
     */
    bool unsavedChanges;
};
