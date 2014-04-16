#ifndef CARD_IDENTIFIER_H
#define CARD_IDENTIFIER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

class CardIdentifier {
private:
	void train();

public:
	CardIdentifier();

};

CardIdentifier::CardIdentifier() {
	train();
}

void CardIdentifier::train() {
	string training_image_path = "C:/Users/Jared/Documents/CS 543/Class Project/example_images/training_data_1.jpg";
	Mat training_image;
    training_image = imread(training_image_path, IMREAD_GRAYSCALE); // Read the file

	//SurfFeatureDetector detector(400);

	namedWindow( "Display window", WINDOW_AUTOSIZE ); // Create a window for display.
    imshow( "Display window", training_image ); // Show our image inside it.
    waitKey(0); // Wait for a keystroke in the window
}

#endif
