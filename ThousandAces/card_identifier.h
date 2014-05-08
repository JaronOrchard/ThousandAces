#ifndef CARD_IDENTIFIER_H
#define CARD_IDENTIFIER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

// comparison function object
// http://stackoverflow.com/questions/13495207/opencv-c-sorting-contours-by-their-contourarea
bool compareContourAreas ( std::vector<cv::Point> contour1, std::vector<cv::Point> contour2 ) {
	double i = fabs( contourArea(cv::Mat(contour1)) );
	double j = fabs( contourArea(cv::Mat(contour2)) );
	return ( i < j );
}

class CardIdentifier {
private:
	void train();

	

public:
	CardIdentifier();

};

CardIdentifier::CardIdentifier() {
	train();
}

// http://arnab.org/blog/so-i-suck-24-automating-card-games-using-opencv-and-python

void CardIdentifier::train() {
	string hand_path = "C:/Users/Jared/Documents/CS 543/Class Project/example_images/hand_1.jpg";

	Mat hand_color = imread(hand_path, IMREAD_COLOR);
	Mat hand_gray = imread(hand_path, IMREAD_GRAYSCALE);
	GaussianBlur(hand_gray, hand_gray, Size(3,3), 0, 0);
	threshold(hand_gray, hand_gray, 150, 255, CV_THRESH_BINARY);
	imshow("hand_gray", hand_gray);
	waitKey(0);
	
	RNG rng(12345);
	int thresh = 100;
	Mat threshold_output;
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	/// Detect edges using canny
	//Canny( hand_gray, canny_output, thresh, thresh*2, 3 );
	/// Find contours
	// Finds only outer contours:
	threshold(hand_gray, threshold_output, 128, 255, THRESH_BINARY);
	findContours( threshold_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	//findContours( canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	// Sort the list to move the 12 biggest external contours to the front
	// (in case others were found):
	std::sort(contours.begin(), contours.end(), compareContourAreas);
	std::reverse(contours.begin(), contours.end());
	

	/// Draw contours
	Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
	Scalar color = Scalar( 128, 128, 128 );
	Scalar colorGreen = Scalar( 0, 255, 0 );
	for( int i = 0; i< contours.size(); i++ )
		{
		
		//drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
		}
	vector< vector<Point> > x;
	vector< vector<Point> > c;
	double epsilonAmt = 0.01; // 2% of perimeter
	for (int i = 0; i < 12; i++) {
		vector<Point> card = contours[i];
		double perimeter = arcLength(card, true);
		vector<Point> approximation;
		approxPolyDP(card, approximation, epsilonAmt*perimeter, true);
		RotatedRect rect = minAreaRect(approximation);
		
		//x.push_back(approximation);
		//c.push_back(card);
		//rect.points(vertices);
		
		vector<Point> hull;
		convexHull(approximation, hull);
		x.push_back(hull);

		int rot = 0; // 0 if right-side up, 3 if rotated 90 degrees
		if ((hull[1].y < hull[0].y && hull[1].y < hull[3].y &&
			 hull[2].y < hull[0].y && hull[2].y < hull[3].y) ||
			(hull[1].y > hull[0].y && hull[1].y > hull[3].y &&
			 hull[2].y > hull[0].y && hull[2].y > hull[3].y)) {
				rot = 3;
		}
		Point2f vertices[4];
		vertices[0] = hull[rot];
		vertices[1] = hull[(rot+1)%4];
		vertices[2] = hull[(rot+2)%4];
		vertices[3] = hull[(rot+3)%4];


		//for (int i = 0; i < 4; i++)
		//	line(hand_color, vertices[i], vertices[(i+1)%4], Scalar(0,255,0));
		

		Point2f vertTo[4];
		vertTo[0] = Point(0,0);
		vertTo[1] = Point(299,0);
		vertTo[2] = Point(299,399);
		vertTo[3] = Point(0,399);
		Mat tf = getPerspectiveTransform(vertices, vertTo);
		Mat whatt;
		warpPerspective(hand_color, whatt, tf, Size(300,400));

		namedWindow("card", CV_WINDOW_AUTOSIZE);
		imshow("card", whatt);
		waitKey(0);

	}
	for (int i = 0; i < x.size(); i++) {
		drawContours( hand_color, c, i, colorGreen, 2, 8, hierarchy, 0, Point() );
		drawContours( hand_color, x, i, color, 2, 8, hierarchy, 0, Point() );
		
	}
	namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
	imshow( "Contours", hand_color );

	waitKey(0);
	return;


}

#endif
