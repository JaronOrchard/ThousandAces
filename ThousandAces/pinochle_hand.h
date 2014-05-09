#ifndef PINOCHLE_HAND_H
#define PINOCHLE_HAND_H

using namespace std;

class PinochleHand {
private:
	int run_clubs, run_diamonds, run_hearts, run_spades;
	int marriage_clubs, marriage_diamonds, marriage_hearts, marriage_spades;
	int hundred_aces, eighty_kings, sixty_queens, forty_jacks, pinochle;
	int meld_clubs, meld_diamonds, meld_hearts, meld_spades;

	void calculateMeld();

	void addToClubs(int meld);
	void addToDiamonds(int meld);
	void addToHearts(int meld);
	void addToSpades(int meld);
	void addToAll(int meld);
	
public:
	int ace_clubs, ten_clubs, king_clubs, queen_clubs, jack_clubs, nine_clubs;
	int ace_diamonds, ten_diamonds, king_diamonds, queen_diamonds, jack_diamonds, nine_diamonds;
	int ace_hearts, ten_hearts, king_hearts, queen_hearts, jack_hearts, nine_hearts;
	int ace_spades, ten_spades, king_spades, queen_spades, jack_spades, nine_spades;

	PinochleHand();
	int determineBid();

};

PinochleHand::PinochleHand() {
	// Set counts of all cards and meld combinations to 0:
	ace_clubs = ten_clubs = king_clubs = queen_clubs = jack_clubs = nine_clubs =
		ace_diamonds = ten_diamonds = king_diamonds = queen_diamonds = jack_diamonds = nine_diamonds =
		ace_hearts = ten_hearts = king_hearts = queen_hearts = jack_hearts = nine_hearts =
		ace_spades = ten_spades = king_spades = queen_spades = jack_spades = nine_spades =
		run_clubs = run_diamonds = run_hearts = run_spades =
		marriage_clubs = marriage_diamonds = marriage_hearts = marriage_spades =
		hundred_aces = eighty_kings = sixty_queens = forty_jacks = pinochle =
		meld_clubs = meld_diamonds = meld_hearts = meld_spades =
		0;
}

int PinochleHand::determineBid() {
	// Calculate the meld for this hand:
	calculateMeld();
	
	// Determine the strength of the hand:
	vector<string> strengthReasons;
	string handStrength = "Low";
	string strengthReasonsString = "";
	int num_clubs = ace_clubs + ten_clubs + king_clubs + queen_clubs + jack_clubs + nine_clubs;
	int num_diamonds = ace_diamonds + ten_diamonds + king_clubs + queen_diamonds + jack_diamonds + nine_diamonds;
	int num_hearts = ace_hearts + ten_hearts + king_hearts + queen_hearts + jack_hearts + nine_hearts;
	int num_spades = ace_spades + ten_spades + king_spades + queen_spades + jack_spades + nine_spades;
	int num_aces = ace_clubs + ace_diamonds + ace_hearts + ace_spades;
	int no_cards_in_suit = 0;
	if (num_clubs == 0) no_cards_in_suit++;
	if (num_diamonds == 0) no_cards_in_suit++;
	if (num_hearts == 0) no_cards_in_suit++;
	if (num_spades == 0) no_cards_in_suit++;

	if (no_cards_in_suit == 1) strengthReasons.push_back("Three-suited");
	if (no_cards_in_suit == 2) strengthReasons.push_back("Two-suited");
	if (no_cards_in_suit == 3) strengthReasons.push_back("Single-suited");
	if (num_clubs > 5) strengthReasons.push_back(to_string((long long)num_clubs) + " clubs");
	if (num_diamonds > 5) strengthReasons.push_back(to_string((long long)num_diamonds) + " diamonds");
	if (num_hearts > 5) strengthReasons.push_back(to_string((long long)num_hearts) + " hearts");
	if (num_spades > 5) strengthReasons.push_back(to_string((long long)num_spades) + " spades");
	if (num_aces > 2) strengthReasons.push_back(to_string((long long)num_aces) + " aces");
	// No need to make a while loop to calculate/concatenate these since there can only be up to 3:
	if (strengthReasons.size() == 1) {
		handStrength = "Low/moderate";
		strengthReasonsString = strengthReasons[0];
	} else if (strengthReasons.size() == 2) {
		handStrength = "Moderate";
		strengthReasonsString = strengthReasons[0] + ", " + strengthReasons[1];
	} else if (strengthReasons.size() == 3) {
		handStrength = "High";
		strengthReasonsString = strengthReasons[0] + ", " + strengthReasons[1] + ", " + strengthReasons[2];
	}
	
	// Determine what to bid, assuming the player goes first and has no partner bid yet:
	int minimumMeld = min(meld_clubs, min(meld_diamonds, min(meld_hearts, meld_spades)));
	int maximumMeld = max(meld_clubs, max(meld_diamonds, max(meld_hearts, meld_spades)));
	int meldDifference = maximumMeld - minimumMeld;
	int suggestedBid = 0;

	if (hundred_aces == 1) { // Hundred aces is always a 205 bid
		suggestedBid = 205;
	} else if (run_clubs || run_diamonds || run_hearts || run_spades) { // Run bids are ~300, or 280 + extra meld
		int run_meld;
		if (run_clubs) { run_meld = meld_clubs; }
		else if (run_diamonds) { run_meld = meld_diamonds; }
		else if (run_hearts) { run_meld = meld_hearts; }
		else { run_meld = meld_spades; }
		suggestedBid = 280 + run_meld - 150;
	} else if (meldDifference >= 60 && !strengthReasons.empty() || // 60+ meld for calling trumps and an okay hand?  Take it.
			strengthReasons.size() >= 2) { // Or maybe you've got a good hand in general?  Take it and call trumps.
		suggestedBid = (maximumMeld / 2) + 150;
	} else { // Make a "meld bid" that tells your partner how helpful you'll be
		suggestedBid = (minimumMeld / 2) + 145;
		if (suggestedBid == 145) {
			suggestedBid = 0; // Worthless hand; don't even bid!
		} else if (suggestedBid == 205) { // 205 is reserved for hundred aces only!  Don't confuse your partner.
			suggestedBid = 195;
		}
	}
	
	// Print results:
	cout << "=== Total meld:" << endl
		 << "If clubs are trumps:    " << meld_clubs << endl
		 << "If diamonds are trumps: " << meld_diamonds << endl
		 << "If hearts are trumps:   " << meld_hearts << endl
		 << "If spades are trumps:   " << meld_spades << endl
		 << "Minimum meld: " << minimumMeld << endl
		 << "Maximum meld: " << maximumMeld << endl
		 << "Strength of hand: " << handStrength << (strengthReasonsString == "" ? "" : " (" + strengthReasonsString + ")") << endl
		 << endl
		 << "=== Suggested bid: " << (suggestedBid == 0 ? "Pass" : to_string((long long)suggestedBid)) << endl
		 << endl;
	return suggestedBid;
}

/*
 * Go through different card combinations to determine how
 * much meld (bonus points) the current hand has
 */
void PinochleHand::calculateMeld() {
	// Step 1: Isolate all meld combinations
	
	// Run: A,10,K,Q,J		Double run: A,A,10,10,K,K,Q,Q,J,J
	run_clubs = min(ace_clubs, min(ten_clubs, min(king_clubs, min(queen_clubs, jack_clubs))));
	run_diamonds = min(ace_diamonds, min(ten_diamonds, min(king_diamonds, min(queen_diamonds, jack_diamonds))));
	run_hearts = min(ace_hearts, min(ten_hearts, min(king_hearts, min(queen_hearts, jack_hearts))));
	run_spades = min(ace_spades, min(ten_spades, min(king_spades, min(queen_spades, jack_spades))));
	// Marriage: K,Q		Double marriage: K,K,Q,Q		(Runs negate marriages)
	marriage_clubs = min(king_clubs, queen_clubs) - run_clubs;
	marriage_diamonds = min(king_diamonds, queen_diamonds) - run_diamonds;
	marriage_hearts = min(king_hearts, queen_hearts) - run_hearts;
	marriage_spades = min(king_spades, queen_spades) - run_spades;
	// Hundred aces, eighty kings, sixty queens, forty jacks: 1 or 2 of given rank, in each suit
	hundred_aces = min(ace_clubs, min(ace_diamonds, min(ace_hearts, ace_spades)));
	eighty_kings = min(king_clubs, min(king_diamonds, min(king_hearts, king_spades)));
	sixty_queens = min(queen_clubs, min(queen_diamonds, min(queen_hearts, queen_spades)));
	forty_jacks = min(jack_clubs, min(jack_diamonds, min(jack_hearts, jack_spades)));
	// Pinochle: Q♠,J♦		Double Pinochle: Q♠,Q♠,J♦,J♦
	pinochle = min(queen_spades, jack_diamonds);

	// Step 2: Determine the amount of meld the player would
	//         have depending on the trump suit for the hand
	cout << "=== Possible meld combinations:" << endl;

	if (run_clubs == 1) {
		cout << "Run in clubs (150 if clubs)" << endl;
		addToClubs(150);
	} else if (run_clubs == 2) {
		cout << "Double run in clubs (1500 if clubs)" << endl;
		addToClubs(1500);
	}
	if (run_diamonds == 1) {
		cout << "Run in diamonds (150 if diamonds)" << endl;
		addToDiamonds(150);
	} else if (run_diamonds == 2) {
		cout << "Double run in diamonds (1500 if diamonds)" << endl;
		addToDiamonds(1500);
	}
	if (run_hearts == 1) {
		cout << "Run in hearts (150 if hearts)" << endl;
		addToHearts(150);
	} else if (run_hearts == 2) {
		cout << "Double run in hearts (1500 if hearts)" << endl;
		addToHearts(1500);
	}
	if (run_spades == 1) {
		cout << "Run in spades (150 if spades)" << endl;
		addToSpades(150);
	} else if (run_spades == 2) {
		cout << "Double run in spades (150 if spades)" << endl;
		addToSpades(1500);
	}

	if (hundred_aces == 1) {
		cout << "Hundred aces (100)" << endl;
		addToAll(100);
	} else if (hundred_aces == 2) {
		cout << "Thousand aces (1000)" << endl;
		addToAll(1000);
	}
	if (eighty_kings == 1) {
		cout << "Eighty kings (80)" << endl;
		addToAll(80);
	} else if (eighty_kings == 2) {
		cout << "Eight hundred kings (800)" << endl;
		addToAll(800);
	}
	if (sixty_queens == 1) {
		cout << "Sixty queens (60)" << endl;
		addToAll(60);
	} else if (sixty_queens == 2) {
		cout << "Six hundred queens (600)" << endl;
		addToAll(600);
	}
	if (forty_jacks == 1) {
		cout << "Forty jacks (40)" << endl;
		addToAll(40);
	} else if (forty_jacks == 2) {
		cout << "Four hundred jacks (400)" << endl;
		addToAll(400);
	}

	if (pinochle == 1) {
		cout << "Pinochle (40)" << endl;
		addToAll(40);
	} else if (pinochle == 2) {
		cout << "Double Pinochle (300)" << endl;
		addToAll(300);
	}

	if (marriage_clubs) cout << "Marriage in clubs" << (marriage_clubs == 2 ? " (x2)"  : "") << " (" << (40 * marriage_clubs) << " if clubs, " << (20 * marriage_clubs) << " otherwise)" << endl;
	if (marriage_diamonds) cout << "Marriage in diamonds" << (marriage_diamonds == 2 ? " (x2)"  : "") << " (" << (40 * marriage_diamonds) << " if diamonds, " << (20 * marriage_diamonds) << " otherwise)" << endl;
	if (marriage_hearts) cout << "Marriage in hearts" << (marriage_hearts == 2 ? " (x2)"  : "") << " (" << (40 * marriage_hearts) << " if hearts, " << (20 * marriage_hearts) << " otherwise)" << endl;
	if (marriage_spades) cout << "Marriage in spades" << (marriage_spades == 2 ? " (x2)"  : "") << " (" << (40 * marriage_spades) << " if spades, " << (20 * marriage_spades) << " otherwise)" << endl;
	addToAll(20 * marriage_clubs); addToClubs(20 * marriage_clubs);
	addToAll(20 * marriage_diamonds); addToDiamonds(20 * marriage_diamonds);
	addToAll(20 * marriage_hearts); addToHearts(20 * marriage_hearts);
	addToAll(20 * marriage_spades); addToSpades(20 * marriage_spades);

	if (nine_clubs) cout << "9 of clubs" << (nine_clubs == 2 ? " (x2)" : "") << " (" << (10 * nine_clubs) << " if clubs)" << endl;
	if (nine_diamonds) cout << "9 of diamonds" << (nine_diamonds == 2 ? " (x2)" : "") << " (" << (10 * nine_diamonds) << " if diamonds)" << endl;
	if (nine_hearts) cout << "9 of hearts" << (nine_hearts == 2 ? " (x2)" : "") << " (" << (10 * nine_hearts) << " if hearts)" << endl;
	if (nine_spades) cout << "9 of spades" << (nine_spades == 2 ? " (x2)" : "") << " (" << (10 * nine_spades) << " if spades)" << endl;
	addToClubs(10 * nine_clubs);
	addToDiamonds(10 * nine_diamonds);
	addToHearts(10 * nine_hearts);
	addToSpades(10 * nine_spades);

	cout << endl;
}

void PinochleHand::addToClubs(int meld) { meld_clubs += meld; }

void PinochleHand::addToDiamonds(int meld) { meld_diamonds += meld; }

void PinochleHand::addToHearts(int meld) { meld_hearts += meld; }

void PinochleHand::addToSpades(int meld) { meld_spades += meld; }

void PinochleHand::addToAll(int meld) {
	addToClubs(meld);
	addToDiamonds(meld);
	addToHearts(meld);
	addToSpades(meld);
}

#endif
