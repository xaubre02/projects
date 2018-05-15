/**
* @file    tester.cpp
* @author  Tomáš Aubrecht (xaubre02)
* @date    květen 2018
* @brief   Definice testovací třídy.
*/

#include "tester.h"
#include <iostream>

// Various Platforms
#ifdef __linux__
	#include <unistd.h>
	#define _chdir(x) chdir(x)
#elif _WIN32
	#include <direct.h>
#endif

// Abbreviations
using std::cout;
using std::cerr;
using std::endl;


/**
* @brief	počítadlo kliknutí myši uživatelem do snímků sítnice
*/
short clicks = 0;

/**
* @brief	Callback funkce zpracovávající událost kliknutí myši. V případě stisku levého tlačítka myši uloží do struktury sítnice souřadnice, na kterých bylo provedeno stisknutí. Pokud se jedná o první klik, uloží se tyto souřadnice do položky optický disk, pokud se jedná o druhý klik, uloží se do položky fovea.
* @param	event událost myši
* @param	x	horizontální souřadnice kliknutí do snímku
* @param	y	vertikální souřadnice kliknutí do snímku
* @param	p	uživatelská data - sítnice
*/
void mouse_callback(int event, int x, int y, int, void *p) {
	if (event == EVENT_LBUTTONDOWN) {
		Retina *retina = (Retina *)p;

		// first click is on the optic disc
		if (clicks == 0) {
			retina->disc.center = Point(x, y);
		}
		// the second one is on the fovea
		else if (clicks == 1) {
			retina->fovea.center = Point(x, y);
		}
		clicks++;
	}
}


bool Tester::inspectDetectionResults(fs::path database) {
	if (fs::is_directory(database)) {
		// recursively browse the directory
		for (fs::recursive_directory_iterator end_iter, item(database); item != end_iter; item++) {
			// item is a file
			if (fs::is_regular_file(item->path())) {
				// Get file extension
				fs::path ext = item->path().extension();
				// supported image formats
				if (ext == ".png" || ext == ".PNG" || ext == ".jpg" || ext == ".JPG") {
					image_cnt++;
					Mat src = imread(item->path().string(), IMREAD_COLOR);
					if (src.empty()) {
						cerr << "Warning: failed to load the picture: " << fs::path(item->path()) << endl;
						continue;
					}

					// Resize the picture
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

					// Find the optic disk of the retina
					Mat mask_od;
					findOpticDisc(res, retina, mask_bg, mask_od);

					// Find the optic disk of the retina
					findFovea(res, retina);

					// Find blood vessels
					Mat mask_bv;
					findBloodVessels(res, retina, mask_bg, mask_bv);

					// Find diseases
					findDiseases(res, retina, mask_bg, mask_bv);

					// Console output
					cout << "Image: " << fs::path(item->path()) << endl;

					// Display images
					displayWindow("Original Image", src);
					displayWindow("Detected Symptoms", res, true);

					// Wait until a key is pressed (Space: next image, ESC: exit)
					while (true) {
						int key = waitKey(100);
						// ESC
						if (key == 0x1B) {
							cout << "Aborted...\n";
							return true;
						}
						// Space
						else if (key == 0x20) {
							break;
						}
					}
				}
			}
		}
	}
	else {
		cerr << "Error: " << database << " is not a valid directory" << endl;
		return false;
	}

	if (image_cnt == 0) {
		cerr << "Warning: " << database << " contains no images" << endl;
	}

	return true;
}


bool Tester::manualDataCollection(fs::path database) {
	if (fs::is_directory(database)) {
		// Make absolute
		database = fs::absolute(database);
		// data file initialization
		if (!createDataFile(database)) {
			cerr << "Error: failed to create a groundtruth file" << endl;
			return false;
		}

		// recursively browse the directory
		for (fs::recursive_directory_iterator end_iter, item(database); item != end_iter; item++) {
			// Item is a file
			if (fs::is_regular_file(item->path())) {
				// Get file extension
				fs::path ext = item->path().extension();
				// Supported image formats
				if (ext == ".png" || ext == ".PNG" || ext == ".jpg" || ext == ".JPG") {
					if (!collectData(item->path(), database)) {
						cout << "Aborted...\n";
						return true;
					}
					image_cnt++;
				}
			}
		}
		cout << "\nTotal image count: " << image_cnt << endl;
		return true;
	}
	else {
		cerr << "Error: " << database << " is not a valid directory" << endl;
		return false;
	}
}


bool Tester::collectData(fs::path image, fs::path database) {
	// load image
	Mat src = imread(image.string(), IMREAD_COLOR);
	if (src.empty()) {
		cerr << "Warning: failed to load the picture: " << image << endl;
		return true;
	}
	// Resize image
	resizeImage(src);

	// Initialize window
	const char *win_name = "Data Collecting";
	namedWindow(win_name, WINDOW_NORMAL | WINDOW_KEEPRATIO | WINDOW_GUI_EXPANDED);
	// Set mouse callback to this window
	setMouseCallback(win_name, mouse_callback, &retina);
	// Display window
	displayWindow(win_name, src);

	// Wait until the second click or ESC to be pressed
	while (true) {
		// Abort collecting
		int key = waitKey(100);
		if (key == 0x1B) {
			return false;
		}
		// Collect
		if (clicks == 2) {
			clicks = 0;
			break;
		}
	}

	size_t size = database.string().size();
	// save relative path to the image (exclude the database path)
	datafile << "path:" << fs::absolute(image).string().substr(size, image.string().size() - size) << ";";

	// save optic disc location
	datafile << "optic_disc:" << retina.disc.center << ";";

	// save fovea location
	datafile << "fovea:" << retina.fovea.center << ";";

	// line end
	datafile << endl;

	return true;
}


bool Tester::createDataFile(fs::path database_dir) {
  // append filename
	database_dir /= "groundtruth";
	datafile.open(database_dir.string(), std::fstream::out);
	return datafile.is_open();
}


bool Tester::createResultsFile(fs::path path) {
  // append filename
	path /= "test_results";
	resfile.open(path.string(), std::fstream::out);
	return resfile.is_open();
}


bool Tester::runAutomatedTests(fs::path groundtruth) {
	datafile.open(groundtruth.string(), std::fstream::in);
	if (!datafile.is_open()) {
		cerr << "Error: failed to open the groundtruth file" << endl;
		return false;
	}

	// change current dir to groundtruth file dir
	fs::path cur_path = groundtruth.parent_path();
	_chdir(cur_path.string().c_str());

	// create test_results file
	if (!createResultsFile(fs::current_path())) {
		cerr << "Error: failed to create the test_results file" << endl;
		return false;
	}

	// statistics
	int images = 0;
	int numof_fov_det = 0;
	int numof_opd_det = 0;

	// read line by line
	int line_num = 1;
	std::string line;
	while (getline(datafile, line)) {
		// file line parsing was successful
		if (parseDataFileLine(line)) {
			// load the image
			Mat img = imread(cur_img.string(), IMREAD_COLOR);
			if (img.empty()) {
				cerr << "Warning: could not load or find the image: " << cur_img << endl;
				line_num++;
				continue;
			}
			// statistics
			images++;

			// Resize the source image
			resizeImage(img);

			// Background mask and parameters
			Mat mask_bg;
			calculateParams(img.cols);
			getBackgroundMask(img, mask_bg);

			// Calculate retina area parameters
			Retina tested_retina;
			getRetinaParams(mask_bg, tested_retina);

			// Find the optic disk
			Mat mask_od;
			findOpticDisc(img, tested_retina, mask_bg, mask_od);

			// Results
			cout << "Image: " << cur_img << endl;
			resfile << "Image: " << cur_img << endl;
			if (isWithinLimit(retina.disc.center, tested_retina.disc.center, tested_retina.disc.radius / 1.5)) {
				// Successful detection
				resfile << "\t- Optic disc: successfully detected" << endl;
				numof_opd_det++;

				// Find the fovea
				findFovea(img, tested_retina);
				if (isWithinLimit(retina.fovea.center, tested_retina.fovea.center, tested_retina.fovea.radius)) {
					// Successful detection
					resfile << "\t- Fovea: successfully detected" << endl;
					numof_fov_det++;
				}
				else {
					resfile << "\t- Fovea: failed to detect  " << retina.fovea.center << " vs " << tested_retina.fovea.center << endl;
				}
			}
			else {
				resfile << "\t- Optic disc: failed to detect  " << retina.disc.center << " vs " << tested_retina.disc.center << endl;
			}

			resfile << endl;
		}
		else {
			cerr << "Warning: failed to parse the line: #" << line_num << endl;
		}
		line_num++;
	}
	// statistics
	resfile << "\n-----------------------------------" << endl;
	resfile << "Optic discs detected:    " << numof_opd_det << "/" << images << "  ( " << double(numof_opd_det) / images * 100 << "% )" << endl;
	resfile << "Foveas detected:         " << numof_fov_det << "/" << images << "  ( " << double(numof_fov_det) / images * 100 << "% )" << endl;
	resfile << "Foveas after correction: " << numof_fov_det << "/" << numof_opd_det << "  ( " << double(numof_fov_det) / numof_opd_det * 100 << "% )" << endl;
	return true;
}


bool Tester::parseDataFileLine(std::string line) {
	// items delimiter
	size_t pos = line.find(";");
	if (pos == std::string::npos) {
		return false;
	}

	// first item must always be the path to the image
	if (line.substr(0, 5) != "path:") {
		return false;
	}

	cur_img = line.substr(5, pos++ - 5);

	// find next items
	while (true) {
		size_t next_pos = line.find(";", pos);
		if (next_pos == std::string::npos) {
			break;
		}
		// parse the item
		parseDataFileItem(line.substr(pos, next_pos - pos));
		pos = next_pos + 1;
	}

	return true;
}


void Tester::parseDataFileItem(std::string item) {
	// name - value delimiter
	size_t pos = item.find(":");
	if (pos == std::string::npos) {
		return;
	}
	// get name and value
	std::string name = item.substr(0, pos++);
	std::string value = item.substr(pos, item.size() - pos);

	if (name == "optic_disc") {
		retina.disc.center = string2point(value);
	}
	else if (name == "fovea") {
		retina.fovea.center = string2point(value);
	}
}


Point Tester::string2point(std::string str) {
	// remove brackets
	str = str.substr(1, str.size() - 2);

	size_t pos = str.find(",");
	if (pos == std::string::npos) {
		return Point();
	}
	// get numbers
	std::string num1 = str.substr(0, pos);
	std::string num2 = str.substr(pos + 2, str.size() - pos - 1);

	// convert strings to integers
	return Point(std::stoi(num1), std::stoi(num2));
}


bool Tester::isWithinLimit(Point p1, Point p2, double limit) {
	int x_diff = p1.x - p2.x;
	int y_diff = p1.y - p2.y;
	// Pythagorean theorem
	return (sqrt(x_diff * x_diff + y_diff * y_diff) < limit) ? true : false;
}
