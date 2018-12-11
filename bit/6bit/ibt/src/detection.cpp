/**
* @file    detection.cpp
* @author  Tomáš Aubrecht (xaubre02)
* @date    květen 2018
* @brief   Definice funkcí pro zpracování snímků sítnic a detekci nálezů.
*/

#include "detection.h"

/**
* @brief	Filtry pro zpracování snímků.
*/
Parameters params;

void resizeImage(_Inout_ Mat &image) {
	// downsample (half the origin size)
	if (image.cols > 2500) {
		pyrDown(image, image);
	}
	// upsample (twice the origin size)
	else if (image.cols < 900) {
		pyrUp(image, image);
	}
	// do not change
	else {
		return;
	}
}


void calculateParams(_In_ int width) {
	// width: 2500 - 1951
	if (width > 1950) {
		params.filter1 = Size(9, 9);
		params.filter2 = Size(13, 13);
	}
	// width: 1950 - 1401
	else if (width > 1400) {
		params.filter1 = Size(7, 7);
		params.filter2 = Size(11, 11);
	}
	// width: 1400 - 900
	else {
		params.filter1 = Size(5, 5);
		params.filter2 = Size(9, 9);
	}
}


double calcAspectRatio(_In_ InputArray points) {
	// Get the bounding rectangle
	RotatedRect rr = minAreaRect(points);
	// Aspect ratio is the ratio of the bigger side to the smaller one
	float width = MAX(rr.size.width, rr.size.height);
	float height = MIN(rr.size.width, rr.size.height);
	return double(width / height);
}


double calcAvgVesselWidth(_In_ InputArray points, _In_ double area) {
	// Calculate perimeter
	double perimeter = arcLength(points, true);
	// Approximate width is the area divided by half of the perimeter
	return double(2 * area / perimeter);
}


void displayWindow(_In_ const char *win_name, _In_ Mat &img, _In_opt_ bool win_pos_right) {
	double ratio = 1;
	// Various Platforms
	#ifdef __linux__
		Display *d = XOpenDisplay(NULL);
		Screen *s = DefaultScreenOfDisplay(d);
		int x = s->width;
		// Window should be half the size of the screen
		ratio = double(img.cols) * 2 / x;
	#elif _WIN32
		int x = GetSystemMetrics(SM_CXSCREEN);
		// Window should be half the size of the screen
		ratio = double(img.cols) * 2 / x;
	#endif

	// Calculate window resolution
	int width = int(img.cols / ratio);
	int height = int(img.rows / ratio);
  
	// Window position
	int x_pos = 0;
	if (win_pos_right) {
		x_pos = width;
	}

	// Initialize the window
	namedWindow(win_name, WINDOW_NORMAL | WINDOW_KEEPRATIO | WINDOW_GUI_EXPANDED);
	resizeWindow(win_name, Size(width, height));
	moveWindow(win_name, x_pos, 0);
	imshow(win_name, img);
}


void getBackgroundMask(_In_ Mat &image, _Out_ Mat &mask) {
	// Conversion to grayscale
	Mat gray;
	cvtColor(image, gray, COLOR_BGR2GRAY);

	// Image smoothing
	Mat smooth;
	boxFilter(gray, smooth, -1, params.filter1);

	// Mask initialization
	mask = smooth.clone();
	mask.setTo(Scalar());

	// Obtaining the regions of interest (1/10 of the height of the image)
	int s = image.rows / 10;
	Rect roi[4];
	roi[0] = Rect(0, 0, s, s);                           // top left corner
	roi[1] = Rect(image.cols - s, 0, s, s);              // top right corner
	roi[2] = Rect(0, image.rows - s, s, s);              // bottom left corner
	roi[3] = Rect(image.cols - s, image.rows - s, s, s); // bottom right corner

	// calculate average intensity of each roi
	double avg = 0;
	for (int c = 0; c < 4; c++) {
		avg += mean(image(roi[c]))[0];
	}
	avg /= 4;

	// Thresholding
	Mat thr;
	threshold(smooth, thr, cvRound(avg) + 5, 255, THRESH_BINARY);

	// Find contours
	vector<vector<Point>> contours;
	findContours(thr, contours, RETR_LIST, CHAIN_APPROX_NONE);

	// Find the biggest contour == retina mask
	vector<Point> mask_contour;
	for (size_t i = 0; i < contours.size(); i++) {
		if (contours[i].size() > mask_contour.size()) {
			mask_contour = contours[i];
		}
	}

	// Draw mask
	vector<vector<Point>> res;
	res.push_back(mask_contour);
	drawContours(mask, res, -1, Scalar(255), -1);
}


void applyMask(_Inout_ Mat &image, _In_ Mat &mask) {
	Mat tmp;
	image.copyTo(tmp, mask);
	image = tmp;
}


void getRetinaParams(_In_ Mat &mask, _Out_ Retina &retina) {
	// Approximate the radius of the retina
	int area(countNonZero(mask));
	double apxRadius = sqrt(area / CV_PI);
	double deviation = apxRadius * 0.01; // 0.1%

	// optimalization
	std::vector<cv::Vec3f> circles;
	double minDist = MAX(mask.cols, mask.rows);
	int minRadius = int(apxRadius - deviation);
	int maxRadius = MAX(mask.cols, mask.rows) / 2;

	// Apply the Hough Transform to find the circles
	HoughCircles(mask, circles, HOUGH_GRADIENT, 1, minDist, 10, 10, minRadius, maxRadius);
	// Error
	if (circles.size() != 1) {
    // center is in the middle
		retina.center.x = mask.cols / 2;
		retina.center.y = mask.rows / 2;
    // radius is smaller than the half of the bigger side of the image 
		retina.radius = int(MAX(mask.cols, mask.rows) / 2.2);
		return;
	}

	// default values
	retina.center.x = cvRound(circles[0][0]);
	retina.center.y = cvRound(circles[0][1]);
	retina.radius = cvRound(circles[0][2]);
}


void findOpticDisc(_In_ Mat &image, _Inout_ Retina &retina, _In_ Mat &bg_mask, _Out_ Mat &od_mask) {
	// Optic disk radius
	retina.disc.radius = retina.radius / 7;

	// Extract green channel
	Mat green;
	extractChannel(image, green, 1); // BGR color space

	// Narrow optic disc search area
	Mat mid_sec = bg_mask.clone();
	mid_sec.setTo(Scalar());
	// rectangle height is one fifth of its diameter
	Point p1(0, retina.center.y + int(retina.radius / 2.5));
	Point p2(mid_sec.cols, retina.center.y - int(retina.radius / 2.5));
	rectangle(mid_sec, p1, p2, Scalar(255), -1);
	// combine with background mask and apply
	bitwise_and(bg_mask, mid_sec, mid_sec);
	applyMask(green, mid_sec);

	// Image smoothing
	Mat blurred;
	boxFilter(green, blurred, -1, params.filter2);

	// Area limit
	double area_limit = retina.radius * 11;

	Mat thr;
	od_mask = bg_mask.clone();
	od_mask.setTo(Scalar());
	for (int c = 255; c >= 0; c--) {
		// Threshold image
		threshold(blurred, thr, c, 255, THRESH_BINARY);
		
		// Find contours
		vector<vector<Point> > contours;
		findContours(thr, contours, RETR_LIST, CHAIN_APPROX_NONE);

		// Iterate through all contours
		for (size_t i = 0; i < contours.size(); i++) {
			if (contourArea(contours[i]) > area_limit) {
				Point2f center;
				float radius;
				minEnclosingCircle(contours[i], center, radius);
				if (radius > retina.disc.radius * 1.2) {
					continue;
				}

				// Get the countour center of mass
				Moments m = moments(contours[i], true);
				retina.disc.center.x = int(m.m10 / m.m00);
				retina.disc.center.y = int(m.m01 / m.m00);

				// Calculate the deviation
				int deviation = cvRound(retina.disc.radius / 2.5);

				// Shift the center
				if (retina.disc.center.x > retina.center.x) {
					retina.disc.center.x += deviation;
				}
				else {
					retina.disc.center.x -= deviation;
				}

				// Create a mask of the found area (circle)
				circle(od_mask, retina.disc.center, retina.disc.radius, Scalar(255), -1, LINE_AA);

				// Exclude only found area
				Mat extr_area;
				green.copyTo(extr_area, od_mask);

				boxFilter(extr_area, blurred, -1, Size(5, 5));
				threshold(blurred, thr, c - 2, 255, THRESH_BINARY);

				// Invert and save mask
				bitwise_not(thr, thr);
				od_mask = thr;

				// break
				c = 0;
				break;
			}
		}
	}
}


void findFovea(_In_ Mat &image, _Inout_ Retina &retina) {
	// Fovea radius
	retina.fovea.radius = retina.radius / 10;

	// Extract green channel
	Mat green;
	extractChannel(image, green, 1); // BGR color space

	// Area approximation
	int radius = retina.disc.radius * 2;
	Point center = retina.disc.center;
	if (center.x > retina.center.x) {
		center.x -= int(4.5 * retina.disc.radius);
	}
	else {
		center.x += int(4.5 * retina.disc.radius);
	}

	Mat f_mask = green.clone();
	f_mask.setTo(Scalar());

	// Create a mask of the approximate area (circle)
	circle(f_mask, center, radius, Scalar(255, 255, 255), -1, LINE_AA);
	applyMask(green, f_mask);

	// Image smoothing
	Mat blurred;
	boxFilter(green, blurred, -1, params.filter2);

	// Area limits
	double area_low = retina.radius * 6;
	double area_upp = area_low * 2.4;

	Mat thr;
	for (int c = 0; c <= 255; c++) {
		// Threshold image
		threshold(blurred, thr, c, 255, THRESH_BINARY);

		// Create border around the search area
		circle(thr, center, radius, Scalar(255), 6);

		// Find contours
		vector<vector<Point> > contours;
		findContours(thr, contours, RETR_LIST, CHAIN_APPROX_NONE);

		// Iterate through all contours
		for (size_t i = 0; i < contours.size(); i++) {
			double area = contourArea(contours[i]);
			if (area > area_low && area < area_upp) {
				// Get the countour center of mass
				Moments m = moments(contours[i], true);
				retina.fovea.center.x = int(m.m10 / m.m00);
				retina.fovea.center.y = int(m.m01 / m.m00);

				// Break
				c = 255;
				break;
			}
		}
	}
}


void findBloodVessels(_In_ Mat &image, _Inout_ Retina &retina, _In_ Mat &bg_mask, _Out_ Mat &bv_mask) {
	// Extract green channel
	Mat green;
	extractChannel(image, green, 1); // BGR color space

	// Adaptive Threshold
	Mat tmp, blurred;
	adaptiveThreshold(green, tmp, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 49, 3);

	// Invert
	bitwise_not(tmp, tmp);

	// Erode background mask and remove background border
	Mat dilate_mask;
	Mat elem = getStructuringElement(MORPH_ELLIPSE, Size(25, 25));
	erode(bg_mask, bg_mask, elem);
	applyMask(tmp, bg_mask);

	// First median blur
	medianBlur(tmp, blurred, 5);

	// Second median blur
	medianBlur(blurred, blurred, 5);

	// Blood Vessels mask
	Mat disk = blurred.clone();
	disk.setTo(Scalar());
	circle(disk, retina.disc.center, retina.disc.radius, Scalar(198), 2, LINE_AA);

	// Find intersections of optic disk circle and blood vessels
	Mat anding = blurred.clone();
	bitwise_and(disk, anding, anding);
	vector<Vec4i> lines;
	HoughLinesP(anding, lines, 1, CV_PI / 180, 10, 5);

	// Create the set of seeds
	vector<Point> seed;
	for (size_t i = 0; i < lines.size(); i++) {
		// Seed is the center of the line
		Point center((lines[i][0] + lines[i][2]) / 2, (lines[i][1] + lines[i][3]) / 2);
		seed.push_back(center);
	}

	// Flood fill the blood vessels, starting from the optic disk
	for (size_t i = 0; i < seed.size(); i++) {
		// Seed has to be a white pixel
		if ((int)blurred.at<uchar>(seed[i].y, seed[i].x) != 255) {
			continue;
		}
		floodFill(blurred, seed[i], Scalar(120));
	}

	Mat result = (blurred == 120);
	bitwise_not(result, result);

	// Remove blood vessels from blurred image
	bitwise_and(result, blurred, blurred);

	// Find contours
	vector<vector<Point> > contours;
	findContours(blurred, contours, RETR_LIST, CHAIN_APPROX_NONE);
  
	// Iterate through all contours
	for (int i = 0; i < int(contours.size()); i++) {
		// calculate contour area
		double area = contourArea(contours[i]);
		if (area > 4500) {
			// blood vessel width approximation
			double vess = calcAvgVesselWidth(contours[i], area);
			if (vess < 8.0) {
				// this is the blood vessel part
				drawContours(result, contours, i, Scalar(), -1, LINE_AA);
			}
		}
		else if (area > 1600) {
			// calculate aspect ration of the bounding rectangle and so on ...
			double aspect_ratio = calcAspectRatio(contours[i]);
			if (aspect_ratio > 10.0) {
				drawContours(result, contours, i, Scalar(), -1, LINE_AA);
			}
			else {
				double vess = calcAvgVesselWidth(contours[i], area);
				if (vess < 6.8) {
					drawContours(result, contours, i, Scalar(), -1, LINE_AA);
				}
			}
		}
		else if (area > 300) {
			double aspect_ratio = calcAspectRatio(contours[i]);
			if (aspect_ratio > 5.0) {
				drawContours(result, contours, i, Scalar(), -1, LINE_AA);
			}
			else {
				Point2f center;
				float radius;
				// calculate minimal enclosing circle
				minEnclosingCircle(contours[i], center, radius);
				if (radius > 40) {
					double vess = calcAvgVesselWidth(contours[i], area);
					if (vess < 5.0) {
						drawContours(result, contours, i, Scalar(), -1, LINE_AA);
					}
				}
			}
		}
	}

	// Save mask
	bv_mask = result;
}


void findDiseases(_In_ Mat &image, _In_ Retina &retina, _In_ Mat &mask_bg, _In_ Mat &mask_bv) {
	// Extract green channel
	Mat green;
	extractChannel(image, green, 1); // BGR color space

	Mat tmp, blurred;
	blur(green, blurred, Size(7, 7));

	// Adaptive Threshold
	adaptiveThreshold(blurred, tmp, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 5, 0);

	// Apply background mask and exclude area of the optic disc
	applyMask(tmp, mask_bg);
	circle(tmp, retina.disc.center, retina.disc.radius, Scalar(), -1);

	// Dilate bloodvessels mask
	Mat elem = getStructuringElement(MORPH_ELLIPSE, Size(15, 15));
	Mat mask_bv_dilated;
	erode(mask_bv, mask_bv_dilated, elem);

	// Apply dilated blood vessels mask
	applyMask(tmp, mask_bv_dilated);

	// Median blur
	medianBlur(tmp, tmp, 5);

	// Find contours
	vector<vector<Point> > contours;
	findContours(tmp, contours, RETR_LIST, CHAIN_APPROX_NONE);

	// Blank picture
	Mat blank = green.clone();
	blank.setTo(Scalar());

	// Convert to HSV color space
	Mat hsv;
	cvtColor(image, hsv, COLOR_BGR2HSV);

	// Iterate through all contours
	for (int i = 0; i < int(contours.size()); i++) {
		double area = contourArea(contours[i]);
		if (area > 3) {
			// Select the area
			Rect rect = boundingRect(contours[i]);
			Mat area_mask = tmp(rect);
			Mat area_hsv = hsv(rect);

			// Calculate the mean color of the selected area
			Scalar color = mean(area_hsv, area_mask);

			// Upper color value limit
			if (color[0] <= 30 && color[1] <= 255 && color[2] <= 255) {
				// Lower color value limit
				if ((color[0] >= 12 && color[1] >= 170 && color[2] >= 120) ||
					(color[0] >= 15 && color[1] >= 120 && color[2] >= 84) ||
					(color[0] >= 19 && color[1] >= 187 && color[2] >= 75)) {

					// Calculate the center of mass
					Moments m = moments(contours[i], true);
					Point center;
					center.x = int(m.m10 / m.m00);
					center.y = int(m.m01 / m.m00);
					// Mark the symptom
					circle(blank, center, 60, Scalar(255), 2);
				}
			}
		}
	}

	// Mark the signs to the input image (only the external contours)
	vector<vector<Point> > circles;
	findContours(blank, circles, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	drawContours(image, circles, -1, Scalar(255, 0, 255), 3);
}