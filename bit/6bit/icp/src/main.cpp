/**
 * @file    main.cpp
 * @author  Tomáš Aubrecht (xaubre02)
 * @date    květen 2018
 * @brief   Vstupní bod aplikace.
 */

/**
 * \mainpage BlockEditor
 * \section sec_uvod Úvod
 *
 * Aplikace pro vytváření, editaci, ukládání a načítání blokových schémat.
 * Vytvořil Tomáš Aubrecht jako semestrální projekt do předmětu ICP v roce 2018.
 *
 * \section sec_makefile Makefile
 *
 * \subsection make1 Vytvoření spustitelného souboru
 *
 * make
 *
 * \subsection make2 Spuštění vytvořeného souboru
 *
 * make run
 *
 * \subsection make3 Vygenerování dokumentace
 *
 * make doxygen
 *
 * \subsection make4 Vyčištění vygenerovaného obsahu
 *
 * make clean
 *
 * \subsection make5 Zabalení zdrojových souborů
 *
 * make pack
 */

#include "gui/mainwindow.h"
#include <QApplication>

/**
 * @brief   Vstupní bod programu
 *
 * @param   argc    Počet vstupních argumentů
 * @param   argv    Pole vstupních argumentů
 */
int main(int argc, char *argv[]) {
    // Inicializace zdrojů
    Q_INIT_RESOURCE(resources);
    QApplication app(argc, argv);
    // Hlavní okno programu
    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
