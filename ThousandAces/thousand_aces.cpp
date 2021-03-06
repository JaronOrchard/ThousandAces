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
	cardIdentifier.processHand(pinochleHand, "./hand_images/hand_6.jpg");
	pinochleHand.determineBid();
	return 0;
}
