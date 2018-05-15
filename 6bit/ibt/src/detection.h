/**
* @file    detection.h
* @author  Tomáš Aubrecht (xaubre02)
* @date    květen 2018
* @brief   Deklarace funkcí pro zpracování snímků sítnic a detekci nálezů.
*/

#pragma once

// OpenCV libraries
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// Various Platforms
#ifdef __linux__
	#include <X11/Xlib.h>
	// Function parameter label
	#define _In_
	#define _Inout_
	#define _In_opt_
	#define _Out_
#elif _WIN32
	#include <Windows.h>
#endif


// Namespaces
using namespace cv;
using namespace std;


/**
* @brief	Struktura představující kružnici.
*/
struct Circle {
	/**
	* @brief	střed kružnice
	*/
	Point center;

	/**
	* @brief	poloměr kružnice
	*/
	int radius;
};

/**
* @brief	Struktura představující sítnici, která zahrnuje optický disk a foveu.
*/
struct Retina : Circle{
	/**
	* @brief	optický disk
	*/
	Circle disc;

	/**
	* @brief	fovea
	*/
	Circle fovea;
};

/**
* @brief	Struktura uchovávající parametry pro zpracování snímků.
*/
struct Parameters {
	/**
	* @brief	filtr využívaný při získávání masky pozadí
	*/
	Size filter1;
	
	/**
	* @brief	filtr využívaný při vyhlazování vstupních snímků
	*/
	Size filter2;
};

/**
* @brief	Provede změnu rozlišení vstupního snímku tak, aby svojí šířkou spadl do rozmezí 2500 až 900 pixelů.
*
* @param	image	vstupní snímek
*/
void resizeImage(_In_ Mat &image);

/**
* @brief	Určí hodnoty jednotlivých filtrů na základě šířky snímku, který se bude zpracovávat.
*
* @param	width	šířka snímku
*/
void calculateParams(_In_ int width);

/**
* @brief	Pro vstupní body kontory vypočítá poměr stran obdélníka, který tyto body ohraničuje.
*
* @param	points	body kontury
* @return	poměr stran
*/
double calcAspectRatio(_In_ InputArray points);

/**
* @brief	Pro vstupní body kontory vypočítá koeficient její šířky, který jsem určil jako obsah vydělený polovinou obvodu kontury.
*
* @param	points	body kontury
* @param	area	obsah kontury (v pixelech)
* @return	koeficient šířky kontury
*/
double calcAvgVesselWidth(_In_ InputArray points, _In_ double area);

/**
* @brief	Inicializuje a zobrazí okno s daným názvem a obsahem, jehož šířka odpovídá polovině šířky obrazovky. Okno je defaultně zobrazeno na levé straně monitoru.
*
* @param	win_name		název okna
* @param	img	šířka		obsahem okna
* @param	win_pos_right	pozice okna na obrazovce
*/
void displayWindow(_In_ const char *win_name, _In_ Mat &img, _In_opt_ bool win_pos_right = false);

/**
* @brief	Oddělí masku pozadí vstupního snímku od jeho popředí.
*
* @param	image	vstupní snímek
* @param	mask	maska pozadí vstupního snímku
*/
void getBackgroundMask(_In_ Mat &image,   _Out_ Mat &mask);

/**
* @brief	Aplikuje masku na vstupní snímek.
*
* @param	image	vstupní snímek
* @param	mask	aplikovaná maska
*/
void applyMask(     _Inout_ Mat &image,    _In_ Mat &mask);

/**
* @brief	Vypočítá střed a poloměr oblasti sítnice z masky pozadí.
*
* @param	mask	maska pozadí
* @param	retina	struktura s hodnotami sítnice
*/
void getRetinaParams(  _In_ Mat &mask,    _Out_ Retina &retina);

/**
* @brief	Lokalizuje optický disk ze vstupního snímku a uloží jeho střed, poloměr a masku.
*
* @param	image	vstupní snímek
* @param	retina	struktura s hodnotami sítnice včetně optického disku
* @param	mask_bg	maska pozadí
* @param	mask_od	maska optického disku
*/
void findOpticDisc(    _In_ Mat &image, _Inout_ Retina &retina, _In_ Mat &mask_bg, _Out_ Mat &mask_od);

/**
* @brief	Lokalizuje foveu ze vstupního snímku a uloží její střed a poloměr.
*
* @param	image	vstupní snímek
* @param	retina	struktura s hodnotami sítnice včetně fovey
*/
void findFovea(        _In_ Mat &image, _Inout_ Retina &retina);

/**
* @brief	Vytvoří masku krevního řečiště ze vstupního snímku.
*
* @param	image	vstupní snímek
* @param	retina	struktura s hodnotami sítnice
* @param	mask_bg	maska pozadí
* @param	mask_bv	maska krevního řečiště
*/
void findBloodVessels( _In_ Mat &image, _Inout_ Retina &retina, _In_ Mat &mask_bg, _Out_ Mat &mask_bv);

/**
* @brief	Lokalizuje drúzy a exudáty na vstupním snímku a zaznačí do něj jejich pozice.
*
* @param	image	vstupní snímek
* @param	retina	struktura s hodnotami sítnice
* @param	mask_bg	maska pozadí
* @param	mask_ov	maska krevního řečiště
*/
void findDiseases(     _In_ Mat &image,    _In_ Retina &retina, _In_ Mat &mask_bg,  _In_ Mat &mask_bv);