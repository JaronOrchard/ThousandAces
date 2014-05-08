#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "card_identifier.h"
#include "pinochle_hand.h"

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
	PinochleHand pinochleHand;
	CardIdentifier cardIdentifier;
	cardIdentifier.processHand(pinochleHand, "C:/Users/Jared/Documents/CS 543/Class Project/example_images/hand_1.jpg");


    return 0;
}
