#ifndef CARD_IDENTIFIER_H
#define CARD_IDENTIFIER_H

#include "pinochle_hand.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

// Comparison function object for sorting contours by their area, taken from:
// http://stackoverflow.com/questions/13495207/opencv-c-sorting-contours-by-their-contourarea
bool compareContourAreas ( std::vector<cv::Point> contour1, std::vector<cv::Point> contour2 ) {
	double i = fabs( contourArea(cv::Mat(contour1)) );
	double j = fabs( contourArea(cv::Mat(contour2)) );
	return ( i < j );
}

class CardIdentifier {
private:
	void processTrainingData();
	int identifyCard(Mat currentCard);
	void addToHand(PinochleHand & hand, int card);
	string getCardName(int card);
	
	vector<Mat> trainingCards;

public:
	CardIdentifier();
	void processHand(PinochleHand & pinochleHand, string hand_path);
	
};

CardIdentifier::CardIdentifier() {
	processTrainingData();
}

void CardIdentifier::processTrainingData() {
	vector<string> card_paths;
	// Original set of training data:
	card_paths.push_back("9C.jpg");		//  0: 9 of clubs
	card_paths.push_back("9CI.jpg");	//  1: 9 of clubs (inverted)
	card_paths.push_back("9D.jpg");		//  2: 9 of diamonds
	card_paths.push_back("9H.jpg");		//  3: 9 of hearts
	card_paths.push_back("9HI.jpg");	//  4: 9 of hearts (inverted)
	card_paths.push_back("9S.jpg");		//  5: 9 of spades
	card_paths.push_back("9SI.jpg");	//  6: 9 of spades (inverted)
	card_paths.push_back("10C.jpg");	//  7: 10 of clubs
	card_paths.push_back("10D.jpg");	//  8: 10 of diamonds
	card_paths.push_back("10H.jpg");	//  9: 10 of hearts
	card_paths.push_back("10S.jpg");	// 10: 10 of spades
	card_paths.push_back("AC.jpg");		// 11: Ace of clubs
	card_paths.push_back("ACI.jpg");	// 12: Ace of clubs (inverted)
	card_paths.push_back("AD.jpg");		// 13: Ace of diamonds
	card_paths.push_back("AH.jpg");		// 14: Ace of hearts
	card_paths.push_back("AHI.jpg");	// 15: Ace of hearts (inverted)
	card_paths.push_back("AS.jpg");		// 16: Ace of spades
	card_paths.push_back("ASI.jpg");	// 17: Ace of spades (inverted)
	card_paths.push_back("JC.jpg");		// 18: Jack of clubs
	card_paths.push_back("JD.jpg");		// 19: Jack of diamonds
	card_paths.push_back("JH.jpg");		// 20: Jack of hearts
	card_paths.push_back("JS.jpg");		// 21: Jack of spades
	card_paths.push_back("KC.jpg");		// 22: King of clubs
	card_paths.push_back("KD.jpg");		// 23: King of diamonds
	card_paths.push_back("KH.jpg");		// 24: King of hearts
	card_paths.push_back("KS.jpg");		// 25: King of spades
	card_paths.push_back("QC.jpg");		// 26: Queen of clubs
	card_paths.push_back("QD.jpg");		// 27: Queen of diamonds
	card_paths.push_back("QH.jpg");		// 28: Queen of hearts
	card_paths.push_back("QS.jpg");		// 29: Queen of spades
	// Extra training data added after achieving 77% first-round accuracy:
	card_paths.push_back("9H_2.jpg");	// 30: 9 of hearts
	card_paths.push_back("10H_2.jpg");	// 31: 10 of hearts
	card_paths.push_back("AD_2.jpg");	// 32: Ace of diamonds
	card_paths.push_back("KH_2.jpg");	// 33: King of hearts
	card_paths.push_back("JH_2.jpg");	// 34: Jack of hearts
	card_paths.push_back("QS_2.jpg");	// 35: Queen of spades
	card_paths.push_back("9D_2.jpg");	// 36: 9 of diamonds
	card_paths.push_back("JS_2.jpg");	// 37: Jack of spades
	card_paths.push_back("JC_2.jpg");	// 38: Jack of clubs
	card_paths.push_back("QC_2.jpg");	// 39: Queen of clubs
	card_paths.push_back("JD_2.jpg");	// 40: Jack of diamonds
	card_paths.push_back("9H_3.jpg");	// 41: 9 of hearts
	card_paths.push_back("10H_3.jpg");	// 42: 10 of hearts
	card_paths.push_back("QD_2.jpg");	// 43: Queen of diamonds
	card_paths.push_back("QH_2.jpg");	// 44: Queen of hearts
	card_paths.push_back("QD_3.jpg");	// 45: Queen of diamonds
	card_paths.push_back("JS_3.jpg");	// 46: Jack of spades
	
	// For each training image, preprocess it:
	trainingCards.clear();
	for (unsigned int i = 0; i < card_paths.size(); i++) {
		string current_path = "C:/Users/Jared/Documents/CS 543/Class Project/example_images/training_images/" + card_paths[i];
		Mat current_card = imread(current_path, IMREAD_GRAYSCALE);
		GaussianBlur(current_card, current_card, Size(5,5), 2);
		adaptiveThreshold(current_card, current_card, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 3, 2);
		trainingCards.push_back(current_card);
	}
}


// The following site was useful in determining how to extract and compare the cards:
// http://arnab.org/blog/so-i-suck-24-automating-card-games-using-opencv-and-python
void CardIdentifier::processHand(PinochleHand & pinochleHand, string hand_path) {
	cout << "=== Processing hand..." << endl;
	Mat hand_color = imread(hand_path, IMREAD_COLOR);
	Mat hand_gray = imread(hand_path, IMREAD_GRAYSCALE);
	GaussianBlur(hand_gray, hand_gray, Size(3,3), 2);
	threshold(hand_gray, hand_gray, 150, 255, CV_THRESH_BINARY);
	
	Mat threshold_output;
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;

	// Find outer contours only:
	threshold(hand_gray, threshold_output, 128, 255, THRESH_BINARY);
	findContours( threshold_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	
	// Sort the list to move the 12 biggest external contours to the front
	// (in case others were found):
	std::sort(contours.begin(), contours.end(), compareContourAreas);
	std::reverse(contours.begin(), contours.end());
	
	
	double epsilonAmt = 0.02; // 2% of perimeter
	// For each of the 12 spread cards...
	for (int i = 0; i < 12; i++) {
		vector<Point> card_contours = contours[i];
		double perimeter = arcLength(card_contours, true);
		vector<Point> approximation, hull;
		// Approximate the contour's bounding box and create a convex hull from it:
		approxPolyDP(card_contours, approximation, epsilonAmt*perimeter, true);
		convexHull(approximation, hull);
		
		// (Code snippet that fixes horizontally-rotated cards:)
		int rot = 0; // 0 if right-side up, 3 if rotated 90 degrees
		if ((hull[1].y < hull[0].y && hull[1].y < hull[3].y &&
			 hull[2].y < hull[0].y && hull[2].y < hull[3].y) ||
			(hull[1].y > hull[0].y && hull[1].y > hull[3].y &&
			 hull[2].y > hull[0].y && hull[2].y > hull[3].y)) {
				rot = 3;
		}

		Point2f vertices[4];
		Point2f vertTo[4];
		vertices[0] = hull[rot];
		vertices[1] = hull[(rot+1)%4];
		vertices[2] = hull[(rot+2)%4];
		vertices[3] = hull[(rot+3)%4];
		vertTo[0] = Point(0,0);
		vertTo[1] = Point(299,0);
		vertTo[2] = Point(299,399);
		vertTo[3] = Point(0,399);
		
		// Warp the card in the image onto a 300x400 matrix:
		Mat tf = getPerspectiveTransform(vertices, vertTo);
		Mat card, card_modified;
		warpPerspective(hand_color, card, tf, Size(300,400));

		// Do adaptive thresholding on the card, blurring before and after:
		cvtColor(card, card_modified, CV_BGR2GRAY);
		GaussianBlur(card_modified, card_modified, Size(5,5), 2);
		adaptiveThreshold(card_modified, card_modified, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 5, 1.5);
		GaussianBlur(card_modified, card_modified, Size(5,5), 5);

		// Identify the card by matching it against all known training cards:
		int card_index = identifyCard(card_modified);
		addToHand(pinochleHand, card_index);
		cout << "Found card: " << getCardName(card_index) << endl;
	}
	cout << endl;
}

int CardIdentifier::identifyCard(Mat currentCard) {
	double best_sum = DBL_MAX;
	int best_index = -1;
	for (unsigned int i = 0; i < trainingCards.size(); i++) {
		Mat combined;
		absdiff(currentCard, trainingCards[i], combined);
		GaussianBlur(combined, combined, Size(5,5), 5, 5);
		threshold(combined, combined, 200, 255, THRESH_BINARY);
		double diff_sum = sum(combined)[0];
		if (diff_sum < best_sum) {
			best_sum = diff_sum;
			best_index = i;
		}
	}
	return best_index;
}

void CardIdentifier::addToHand(PinochleHand & hand, int card) {
	switch (card) {
		case 0: case 1:						hand.nine_clubs++; break;
		case 2: case 36:					hand.nine_diamonds++; break;
		case 3: case 4: case 30: case 41:	hand.nine_hearts++; break;
		case 5: case 6:						hand.nine_spades++; break;
		case 7:								hand.ten_clubs++; break;
		case 8:								hand.ten_diamonds++; break;
		case 9: case 31: case 42:			hand.ten_hearts++; break;
		case 10:							hand.ten_spades++; break;
		case 11: case 12:					hand.ace_clubs++; break;
		case 13: case 32:					hand.ace_diamonds++; break;
		case 14: case 15:					hand.ace_hearts++; break;
		case 16: case 17:					hand.ace_spades++; break;
		case 18: case 38:					hand.jack_clubs++; break;
		case 19: case 40:					hand.jack_diamonds++; break;
		case 20: case 34:					hand.jack_hearts++; break;
		case 21: case 37: case 46:			hand.jack_spades++; break;
		case 22:							hand.king_clubs++; break;
		case 23:							hand.king_diamonds++; break;
		case 24: case 33:					hand.king_hearts++; break;
		case 25:							hand.king_spades++; break;
		case 26: case 39:					hand.queen_clubs++; break;
		case 27: case 43: case 45:			hand.queen_diamonds++; break;
		case 28: case 44:					hand.queen_hearts++; break;
		case 29: case 35:					hand.queen_spades++; break;
	}
}

string CardIdentifier::getCardName(int card) {
	switch (card) {
		case 0: case 1:						return "9 of clubs";
		case 2: case 36:					return "9 of diamonds";
		case 3: case 4: case 30: case 41:	return "9 of hearts";
		case 5: case 6:						return "9 of spades";
		case 7:								return "10 of clubs";
		case 8:								return "10 of diamonds";
		case 9: case 31: case 42:			return "10 of hearts";
		case 10:							return "10 of spades";
		case 11: case 12:					return "Ace of clubs";
		case 13: case 32:					return "Ace of diamonds";
		case 14: case 15:					return "Ace of hearts";
		case 16: case 17:					return "Ace of spades";
		case 18: case 38:					return "Jack of clubs";
		case 19: case 40:					return "Jack of diamonds";
		case 20: case 34:					return "Jack of hearts";
		case 21: case 37: case 46:			return "Jack of spades";
		case 22:							return "King of clubs";
		case 23:							return "King of diamonds";
		case 24: case 33:					return "King of hearts";
		case 25:							return "King of spades";
		case 26: case 39:					return "Queen of clubs";
		case 27: case 43: case 45:			return "Queen of diamonds";
		case 28: case 44:					return "Queen of hearts";
		case 29: case 35:					return "Queen of spades";
		default:							return "Unknown";
	}
}

#endif
