/**
* @file    main.cpp
* @author  Tomáš Aubrecht (xaubre02)
* @date    květen 2018
* @brief   Vstupní bod programu.
*/


/**
* \mainpage Detektor příznaků věkem podmíněné makulární degenerace
* \section sec_uvod Úvod
*
* Aplikace pro automatickou detekci drúz a exudátů ze snímků sítnic.
*
* Vytvořil Tomáš Aubrecht v rámci své bakalářské práce v roce 2018.
*
* Vysoké učení technické v Brně, Fakulta informačních technologií
*
* \section sec_usage Použití
*
* \subsection subsec_make Makefile (Linux)
*
* - __make__ - provede překlad zdrojových souborů a vytvoří spustitelný soubor <i>detector</i>
* - __make run__ - spustí soubor <i>detector</i> s parametrem <i>- -help</i>
* - __make clean__ - odstraní všechny soubory vzniklé příkazem <i>make</i> a <i>make doxygen</i>
* - __make doxygen__ - vytvoří programovou dokumentaci do adresáře <i>./doc</i>
*
* \subsection subsec_run Spuštění
* 
*     ./detector -h | -i IMAGE | --collect DATABASE | --compare DATABASE | --test GROUNDTRUTH [-d] [--mark] [--mask]
*
* \subsection subsec_args Argumenty programu
*
* - __Exkluzivní argumenty__ <i>(nutné zvolit právě jeden z těchto argumentů)</i>
*   + <b>-h, - -help</b>&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&nbsp;zobrazí nápovědu
*   + <b>-i, - -image IMAGE</b>&emsp;&emsp;&emsp;&emsp;&nbsp;specifikuje vstupní snímek IMAGE pro zpracování
*   + <b>- -collect DATABASE</b>&emsp;&emsp;&emsp;manuální lokalizace optického disku a fovey ve snímcích databáze DATABASE pro vytvoření souboru groundtruth
*   + <b>- -compare DATABASE</b>&emsp;&emsp;postupné zobrazení všech snímků v databázi DATABASE a k nim odpovídajících snímků s detekovanými nálezy pro porovnání
*   + <b>- -test GROUNDTRUTH</b>&emsp;&emsp;spustí automatické testy nad souborem GROUNDTRUTH
*
*
* - __Volitelné argumenty__ <i>(mají vliv pouze při použití přepínače <b>-i/--image</b>)</i>
*   + <b>-d, - -detect</b>&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&nbsp;detekuje drúzy a exudáty
*   + <b>- -mark</b>&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&nbsp;označí na snímku optický disk a foveu
*   + <b>- -mask</b>&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&nbsp;zobrazí masku pozadí s krevním řečištěm a optickým diskem
*
* \subsection subsec_examples Příklady použití
*
*     ./detector -i image001.png --mask             zobrazí vstupní snímek spolu s vytvořenou maskou
*     ./detector -i image002.png --mark --detect    zobrazí vstupní snímek spolu se snímkem, na kterém je zaznačen optický disk, fovea a detekované nálezy
*     ./detector --collect database/FIRE            vytvoří soubor groundtruth v adresáři databáze po manuální lokalizaci optického disku a fovey uživatelem
*     ./detector --test database/FIRE/groundtruth   spustí automatické testování ze souboru groundtruth a do stejného adresáře vytvoří soubor test_results obsahující výsledky testování
*
*/

#include "tester.h"
#include <chrono>
#include <iostream>


/**
* @brief	Struktura uchovávající flagy pro jednotlivé přepínače programu.
*/
struct Args_t {
	/**
	* @brief	přepínač -h / --help
	*/
	bool help;

	/**
	* @brief	přepínač -i / --image
	*/
	bool imge;

	/**
	* @brief	přepínač --collect
	*/
	bool coll;

	/**
	* @brief	přepínač --compare
	*/
	bool comp;

	/**
	* @brief	přepínač --test
	*/
	bool test;
	
	/**
	* @brief	přepínač -d / --detect
	*/
	bool dtct;

	/**
	* @brief	přepínač --mark
	*/
	bool mark;

	/**
	* @brief	přepínač --mask
	*/
	bool mask;

	/**
	* @brief	Inicializace struktury - všechny přepínače mají hodnotu false
	*/
	Args_t() : help{ false }, imge{ false }, coll{ false }, comp{ false }, test{ false }, dtct{ false }, mark{ false }, mask{ false } {}
};

/**
* @brief	Zobrazí nápovědu pro používání tohoto programu.
*
* @param	appname	název programu
*/
void printHelp(const char *appname) {
	cout << "Usage :\n\n"
		"   ./" << appname <<
		" -h | -i IMAGE | --collect DATABASE | --compare DATABASE | --test GROUNDTRUTH [-d] [--mark] [--mask]\n\n\n"
		"Exclusive arguments :\n\n"
		"   -h, --help            show this help and exit\n\n"
		"   -i, --image IMAGE     specify input file IMAGE to process\n\n"
		"   --collect DATABASE    manual collection of OD and fovea centers from DATABASE\n\n"
		"   --compare DATABASE    show all images in the DATABASE one by one with the image of detected symptoms for comparison\n\n"
		"   --test GROUNDTRUTH    run tests over the GROUNDTRUTH file\n\n\n"
		"Optional arguments :\n\n"
		"   -d, --detect          detect drusen and exudates\n\n"
		"   --mark                mark the area of optic disc and fovea\n\n"
		"   --mask                display the background mask with optic disc and blood vessels\n\n" << endl;
}

/**
* @brief	Zpracuje vstupní argumenty a na jejich základě zvolí další postup provádění programu.
*
* @param	argc	Počet vstupních argumentů
* @param	argv	Pole vstupních argumentů
*/
int main(int argc, const char **argv) {
	if (argc == 1) {
		printHelp(argv[0]);
		return EXIT_SUCCESS;
	}

	// Arguments parsing
	// -----------------
	Args_t args;
	fs::path path;
	for (int c = 1; c < argc; c++) {
		if (!strcmp(argv[c], "-h") || !strcmp(argv[c], "--help")) {
			// duplicate argument
			if (args.help) {
				std::cerr << "Error: duplicate argument: " << argv[c] << std::endl;
				return EXIT_FAILURE;
			}
			// exclusive arguments
			else if (args.imge || args.coll || args.comp || args.test) {
				std::cerr << "Error: two or more exclusive arguments specified\n";
				return EXIT_FAILURE;
			}
			args.help = true;
		}
		else if (!strcmp(argv[c], "-i") || !strcmp(argv[c], "--image")) {
			// duplicate argument
			if (args.imge) {
				std::cerr << "Error: duplicate argument: " << argv[c] << std::endl;
				return EXIT_FAILURE;
			}
			// exclusive arguments
			else if (args.help || args.coll || args.comp || args.test) {
				std::cerr << "Error: two or more exclusive arguments specified\n";
				return EXIT_FAILURE;
			}
			// path to file
			else if (c + 1 == argc) {
				std::cerr << "Error: image path not specified\n";
				return EXIT_FAILURE;
			}
			else {
				args.imge = true;
				// save next argument and skip it
				c++;
				path = argv[c];
			}
		}
		// same as in the previous ones ...
		else if (!strcmp(argv[c], "--collect")) {
			if (args.coll) {
				std::cerr << "Error: duplicate argument: " << argv[c] << std::endl;
				return EXIT_FAILURE;
			}
			else if (args.help || args.imge || args.comp || args.test) {
				std::cerr << "Error: two or more exclusive arguments specified\n";
				return EXIT_FAILURE;
			}
			else if (c + 1 == argc) {
				std::cerr << "Error: database path not specified\n";
				return EXIT_FAILURE;
			}
			else {
				args.coll = true;
				c++;
				path = argv[c];
			}
		}
		else if (!strcmp(argv[c], "--compare")) {
			if (args.comp) {
				std::cerr << "Error: duplicate argument: " << argv[c] << std::endl;
			}
			else if (args.help || args.imge || args.coll || args.test) {
				std::cerr << "Error: two or more exclusive arguments specified\n";
				return EXIT_FAILURE;
			}
			else if (c + 1 == argc) {
				std::cerr << "Error: database path not specified\n";
				return EXIT_FAILURE;
			}
			else {
				args.comp = true;
				c++;
				path = argv[c];
			}
		}
		else if (!strcmp(argv[c], "--test")) {
			if (args.test) {
				std::cerr << "Error: duplicate argument: " << argv[c] << std::endl;
				return EXIT_FAILURE;
			}
			else if (args.help || args.imge || args.coll || args.comp) {
				std::cerr << "Error: two or more exclusive arguments specified\n";
				return EXIT_FAILURE;
			}
			else if (c + 1 == argc) {
				std::cerr << "Error: file path not specified\n";
				return EXIT_FAILURE;
			}
			else {
				args.test = true;
				c++;
				path = argv[c];
			}
		}
		else if (!strcmp(argv[c], "-d") || !strcmp(argv[c], "--detect")) {
			if (args.dtct) {
				std::cerr << "Error: duplicate argument: " << argv[c] << std::endl;
				return EXIT_FAILURE;
			}
			args.dtct = true;
		}
		else if (!strcmp(argv[c], "--mark")) {
			if (args.mark) {
				std::cerr << "Error: duplicate argument: " << argv[c] << std::endl;
				return EXIT_FAILURE;
			}
			args.mark = true;
		}
		else if (!strcmp(argv[c], "--mask")) {
			if (args.mask) {
				std::cerr << "Error: duplicate argument: " << argv[c] << std::endl;
				return EXIT_FAILURE;
			}
			args.mask = true;
		}
		else {
			std::cerr << "Error: unknown argument: " << argv[c] << std::endl;
			return EXIT_FAILURE;
		}
	}
	
	// Test class
	Tester tester;

	if (args.help) {
		printHelp(argv[0]);
		return EXIT_SUCCESS;
	}
	else if (args.imge) {
		// Timer
		auto start = std::chrono::high_resolution_clock::now();

		// Load image
		Mat src = imread(path.string(), IMREAD_COLOR);
		if (src.empty()) {
			cerr << "Error: could not load or find the image" << endl;
			return EXIT_FAILURE;
		}
		else {
			// Resize the source image
			resizeImage(src);

			// Backup - keep src unchanged
			Mat res = src.clone();

			// Background mask and parameters
			Mat mask_bg;
			calculateParams(res.cols);
			getBackgroundMask(res, mask_bg);

			// Calculate retina area parameters
			Retina retina;
			getRetinaParams(mask_bg, retina);

			// Find the optic disk
			Mat mask_od;
			findOpticDisc(res, retina, mask_bg, mask_od);

			// Find the fovea
			findFovea(res, retina);

			// Find blood vessels
			Mat mask_bv;
			findBloodVessels(res, retina, mask_bg, mask_bv);

			// Display original image
			displayWindow("Original Image", src);
			
			// Display results
			// ---------------
			if (args.mark) {
				// Mark the optic disc and fovea
				circle(res, retina.disc.center,  retina.disc.radius,  Scalar(0, 255, 0), 2, LINE_AA);
				circle(res, retina.fovea.center, retina.fovea.radius, Scalar(0, 255, 0), 2, LINE_AA);
				displayWindow("Output Image", res, true);
			}

			if (args.mask) {
				Mat mask_and;
				// background mask and optic disc mask and blood vessels mask
				bitwise_and(mask_bg, mask_od,  mask_and);
				bitwise_and(mask_bv, mask_and, mask_and);
				displayWindow("Mask Image", mask_and, true);
			}

			if (args.dtct) {
				// Find diseases
				findDiseases(res, retina, mask_bg, mask_bv);
				displayWindow("Output Image", res, true);
			}

			// Runtime
			auto finish = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> runtime = finish - start;
			cout << "Runtime: " << runtime.count() << endl;

			// Loop until "ESC" is pressed.
			while (true) {
				if (waitKey(100) == 0x1B) {
					return EXIT_SUCCESS;
				}
			}
		}
	}
	else if (args.coll) {
		if (tester.manualDataCollection(path))
			return EXIT_SUCCESS;
		else
			return EXIT_FAILURE;
	}
	else if (args.comp) {
		if (tester.inspectDetectionResults(path))
			return EXIT_SUCCESS;
		else
			return EXIT_FAILURE;
	}
	else if (args.test) {
		if (tester.runAutomatedTests(path))
			return EXIT_SUCCESS;
		else
			return EXIT_FAILURE;
	}
	else {
		std::cerr << "Error: exclusive argument not specified\n";
		return EXIT_FAILURE;
	}
}