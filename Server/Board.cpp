#include "Board.h"
#include <iostream>
#include <string>
#include <random>
#include <time.h>
#include <vector>
#include <algorithm>

Board::Board() {
	// Initialize all board squares
	positions[0].setName("GO");
	positions[0].setPosType("GO");
	positions[0].setPrice(0);
	positions[0].setHousePrice(0);
	positions[0].setRent(0);
	positions[0].setTax(0);

	positions[1].setName("Mediterreanean Avenue");
	positions[1].setPosType("Property");
	positions[1].setPrice(60);
	positions[1].setHousePrice(50);
	positions[1].setRent(2);
	positions[1].setTax(0);

	positions[2].setName("Community Chest");
	positions[2].setPosType("Community Chest");
	positions[2].setPrice(0);
	positions[2].setHousePrice(0);
	positions[2].setRent(0);
	positions[2].setTax(0);

	positions[3].setName("Baltic Avenue");
	positions[3].setPosType("Property");
	positions[3].setPrice(60);
	positions[3].setHousePrice(50);
	positions[3].setRent(4);
	positions[3].setTax(0);

	positions[4].setName("Income Tax");
	positions[4].setPosType("Tax");
	positions[4].setPrice(0);
	positions[4].setHousePrice(0);
	positions[4].setRent(0);
	positions[4].setTax(200);

	positions[5].setName("Reading Railroad");
	positions[5].setPosType("Railroad");
	positions[5].setPrice(200);
	positions[5].setHousePrice(0);
	positions[5].setRent(25);
	positions[5].setTax(0);

	positions[6].setName("Oriental Avenue");
	positions[6].setPosType("Property");
	positions[6].setPrice(100);
	positions[6].setHousePrice(50);
	positions[6].setRent(6);
	positions[6].setTax(0);

	positions[7].setName("Chance");
	positions[7].setPosType("Chance");
	positions[7].setPrice(0);
	positions[7].setHousePrice(0);
	positions[7].setRent(0);
	positions[7].setTax(0);

	positions[8].setName("Vermont Avenue");
	positions[8].setPosType("Property");
	positions[8].setPrice(100);
	positions[8].setHousePrice(50);
	positions[8].setRent(6);
	positions[8].setTax(0);

	positions[9].setName("Connecticut Avenue");
	positions[9].setPosType("Property");
	positions[9].setPrice(120);
	positions[9].setHousePrice(50);
	positions[9].setRent(8);
	positions[9].setTax(0);

	positions[10].setName("Jail");
	positions[10].setPosType("Jail");
	positions[10].setPrice(0);
	positions[10].setHousePrice(0);
	positions[10].setRent(0);
	positions[10].setTax(0);

	positions[11].setName("St. Charles Place");
	positions[11].setPosType("Property");
	positions[11].setPrice(140);
	positions[11].setHousePrice(100);
	positions[11].setRent(10);
	positions[11].setTax(0);

	positions[12].setName("Electric Company");
	positions[12].setPosType("Utility");
	positions[12].setPrice(150);
	positions[12].setHousePrice(0);
	positions[12].setRent(4);
	positions[12].setTax(0);

	positions[13].setName("States Avenue");
	positions[13].setPosType("Property");
	positions[13].setPrice(140);
	positions[13].setHousePrice(100);
	positions[13].setRent(10);
	positions[13].setTax(0);

	positions[14].setName("Virginia Avenue");
	positions[14].setPosType("Property");
	positions[14].setPrice(160);
	positions[14].setHousePrice(100);
	positions[14].setRent(12);
	positions[14].setTax(0);

	positions[15].setName("Pennsylvania Railroad");
	positions[15].setPosType("Railroad");
	positions[15].setPrice(200);
	positions[15].setHousePrice(0);
	positions[15].setRent(25);
	positions[15].setTax(0);

	positions[16].setName("St. James Place");
	positions[16].setPosType("Property");
	positions[16].setPrice(180);
	positions[16].setHousePrice(100);
	positions[16].setRent(14);
	positions[16].setTax(0);

	positions[17].setName("Community Chest");
	positions[17].setPosType("Community Chest");
	positions[17].setPrice(0);
	positions[17].setHousePrice(0);
	positions[17].setRent(0);
	positions[17].setTax(0);

	positions[18].setName("Tennessee Avenue");
	positions[18].setPosType("Property");
	positions[18].setPrice(180);
	positions[18].setHousePrice(100);
	positions[18].setRent(14);
	positions[18].setTax(0);

	positions[19].setName("New York Avenue");
	positions[19].setPosType("Property");
	positions[19].setPrice(200);
	positions[19].setHousePrice(100);
	positions[19].setRent(16);
	positions[19].setTax(0);

	positions[20].setName("Free Parking");
	positions[20].setPosType("Free Parking");
	positions[20].setPrice(0);
	positions[20].setHousePrice(0);
	positions[20].setRent(0);
	positions[20].setTax(0);

	positions[21].setName("Kentucky Avenue");
	positions[21].setPosType("Property");
	positions[21].setPrice(220);
	positions[21].setHousePrice(150);
	positions[21].setRent(18);
	positions[21].setTax(0);

	positions[22].setName("Chance");
	positions[22].setPosType("Chance");
	positions[22].setPrice(0);
	positions[22].setHousePrice(0);
	positions[22].setRent(0);
	positions[22].setTax(0);

	positions[23].setName("Indiana Avenue");
	positions[23].setPosType("Property");
	positions[23].setPrice(220);
	positions[23].setHousePrice(150);
	positions[23].setRent(18);
	positions[23].setTax(0);

	positions[24].setName("Illinois Avenue");
	positions[24].setPosType("Property");
	positions[24].setPrice(240);
	positions[24].setHousePrice(150);
	positions[24].setRent(20);
	positions[24].setTax(0);

	positions[25].setName("B. & O. Railroad");
	positions[25].setPosType("Railroad");
	positions[25].setPrice(200);
	positions[25].setHousePrice(0);
	positions[25].setRent(25);
	positions[25].setTax(0);

	positions[26].setName("Atlantic Avenue");
	positions[26].setPosType("Property");
	positions[26].setPrice(260);
	positions[26].setHousePrice(150);
	positions[26].setRent(22);
	positions[26].setTax(0);

	positions[27].setName("Ventnor Avenue");
	positions[27].setPosType("Property");
	positions[27].setPrice(260);
	positions[27].setHousePrice(150);
	positions[27].setRent(22);
	positions[27].setTax(0);

	positions[28].setName("Water Works");
	positions[28].setPosType("Utility");
	positions[28].setPrice(150);
	positions[28].setHousePrice(0);
	positions[28].setRent(4);
	positions[28].setTax(0);

	positions[29].setName("Marvin Gardens");
	positions[29].setPosType("Property");
	positions[29].setPrice(280);
	positions[29].setHousePrice(150);
	positions[29].setRent(24);
	positions[29].setTax(0);

	positions[30].setName("Go To Jail");
	positions[30].setPosType("Go To Jail");
	positions[30].setPrice(0);
	positions[30].setHousePrice(0);
	positions[30].setRent(0);
	positions[30].setTax(0);

	positions[31].setName("Pacific Avenue");
	positions[31].setPosType("Property");
	positions[31].setPrice(300);
	positions[31].setHousePrice(200);
	positions[31].setRent(26);
	positions[31].setTax(0);

	positions[32].setName("North Carolina Avenue");
	positions[32].setPosType("Property");
	positions[32].setPrice(300);
	positions[32].setHousePrice(200);
	positions[32].setRent(26);
	positions[32].setTax(0);

	positions[33].setName("Community Chest");
	positions[33].setPosType("Community Chest");
	positions[33].setPrice(0);
	positions[33].setHousePrice(0);
	positions[33].setRent(0);
	positions[33].setTax(0);

	positions[34].setName("Pennsylvania Avenue");
	positions[34].setPosType("Property");
	positions[34].setPrice(320);
	positions[34].setHousePrice(200);
	positions[34].setRent(28);
	positions[34].setTax(0);

	positions[35].setName("Short Line");
	positions[35].setPosType("Railroad");
	positions[35].setPrice(200);
	positions[35].setHousePrice(0);
	positions[35].setRent(25);
	positions[35].setTax(0);

	positions[36].setName("Chance");
	positions[36].setPosType("Chance");
	positions[36].setPrice(0);
	positions[36].setHousePrice(0);
	positions[36].setRent(0);
	positions[36].setTax(0);

	positions[37].setName("Park Place");
	positions[37].setPosType("Property");
	positions[37].setPrice(350);
	positions[37].setHousePrice(200);
	positions[37].setRent(35);
	positions[37].setTax(0);

	positions[38].setName("Luxury Tax");
	positions[38].setPosType("Tax");
	positions[38].setPrice(0);
	positions[38].setHousePrice(0);
	positions[38].setRent(0);
	positions[38].setTax(100);

	positions[39].setName("Boardwalk");
	positions[39].setPosType("Property");
	positions[39].setPrice(400);
	positions[39].setHousePrice(200);
	positions[39].setRent(50);
	positions[39].setTax(0);

	for (int i = 0; i < 40; i++) {
		positions[i].setOwner(-1);
		positions[i].setHouses(0);
	}

	// Initialize all Chance cards
	chanceCards[0].setDescription("You have been elected chairman of the board. Pay the bank $50.");
	chanceCards[0].setAmountMoney(-50);
	chanceCards[0].setMoveTo(-1);

	chanceCards[1].setDescription("Advance to Boardwalk");
	chanceCards[1].setAmountMoney(0);
	chanceCards[1].setMoveTo(39);

	chanceCards[2].setDescription("Your building loan matures. Collect $150.");
	chanceCards[2].setAmountMoney(150);
	chanceCards[2].setMoveTo(-1);

	chanceCards[3].setDescription("Advance to GO.");
	chanceCards[3].setAmountMoney(0);
	chanceCards[3].setMoveTo(0);

	chanceCards[4].setDescription("Bank pays you dividend of $50.");
	chanceCards[4].setAmountMoney(50);
	chanceCards[4].setMoveTo(-1);

	chanceCards[5].setDescription("General repairs - pay $100");
	chanceCards[5].setAmountMoney(-100);
	chanceCards[5].setMoveTo(-1);

	chanceCards[6].setDescription("Advance to St. Charles Place.");
	chanceCards[6].setAmountMoney(0);
	chanceCards[6].setMoveTo(11);

	chanceCards[7].setDescription("Advance to Illinois Avenue.");
	chanceCards[7].setAmountMoney(0);
	chanceCards[7].setMoveTo(24);

	chanceCards[8].setDescription("GO TO JAIL, pay $50.");
	chanceCards[8].setAmountMoney(-50);
	chanceCards[8].setMoveTo(10);

	chanceCards[9].setDescription("Take a trip to Reading Railroad.");
	chanceCards[9].setAmountMoney(0);
	chanceCards[9].setMoveTo(5);

	chanceCards[10].setDescription("Speeding fine $15.");
	chanceCards[10].setAmountMoney(-15);
	chanceCards[10].setMoveTo(-1);

	for (int i = 0; i < 11; i++) {
		chanceCards[i].setCardType("Chance");
	}

	// Initialize all Community Chest cards
	communityChestCards[0].setDescription("Pay hospital fees of $100.");
	communityChestCards[0].setAmountMoney(-100);
	communityChestCards[0].setMoveTo(-1);

	communityChestCards[1].setDescription("From sale of stock you get $50.");
	communityChestCards[1].setAmountMoney(50);
	communityChestCards[1].setMoveTo(-1);

	communityChestCards[2].setDescription("Income tax refund. Collect $20.");
	communityChestCards[2].setAmountMoney(20);
	communityChestCards[2].setMoveTo(-1);

	communityChestCards[3].setDescription("It is your birthday. Collect $30");
	communityChestCards[3].setAmountMoney(30);
	communityChestCards[3].setMoveTo(-1);

	communityChestCards[4].setDescription("You inherit $100.");
	communityChestCards[4].setAmountMoney(100);
	communityChestCards[4].setMoveTo(-1);

	communityChestCards[5].setDescription("Street repairs - pay $80.");
	communityChestCards[5].setAmountMoney(-80);
	communityChestCards[5].setMoveTo(-1);

	communityChestCards[6].setDescription("Holiday fund matures. Receive $100.");
	communityChestCards[6].setAmountMoney(100);
	communityChestCards[6].setMoveTo(-1);

	communityChestCards[7].setDescription("You have won second prize in a beauty contest. Collect $10.");
	communityChestCards[7].setAmountMoney(10);
	communityChestCards[7].setMoveTo(-1);

	communityChestCards[8].setDescription("Receive $25 consultancy fee.");
	communityChestCards[8].setAmountMoney(25);
	communityChestCards[8].setMoveTo(-1);

	communityChestCards[9].setDescription("Bank error in your favour. Collect $200.");
	communityChestCards[9].setAmountMoney(200);
	communityChestCards[9].setMoveTo(-1);

	communityChestCards[10].setDescription("GO TO JAIL, pay $50.");
	communityChestCards[10].setAmountMoney(-50);
	communityChestCards[10].setMoveTo(10);

	communityChestCards[11].setDescription("Life insurance matures. Collect $100.");
	communityChestCards[11].setAmountMoney(100);
	communityChestCards[11].setMoveTo(-1);

	communityChestCards[12].setDescription("Doctor's fees. Pay $50.");
	communityChestCards[12].setAmountMoney(-50);
	communityChestCards[12].setMoveTo(-1);

	communityChestCards[13].setDescription("Advance to GO.");
	communityChestCards[13].setAmountMoney(0);
	communityChestCards[13].setMoveTo(0);

	communityChestCards[14].setDescription("Pay school fees of $50.");
	communityChestCards[14].setAmountMoney(-50);
	communityChestCards[14].setMoveTo(-1);

	for (int i = 0; i < 15; i++) {
		communityChestCards[i].setCardType("Community Chance");
	}
}

void Board::createPlayers(std::vector<std::string> names, std::vector<uint16> slotNums) {
	players = new Player[4];

	std::vector<std::string> gamepieces = { "Racecar", "Battleship", "Shoe", "Wheelbarrow", "Top Hat", "Scottie Dog", "Thimble", "Iron" };
	int gamepieceNum;

	for (int i = 0; i < slotNums.size(); i++) {
		players[slotNums[i]].setPlayerName(names[slotNums[i]]);

		// Assign random gamepiece to player
		srand(time(NULL)); // initialize random seed
		gamepieceNum = rand() % gamepieces.size();
		players[slotNums[i]].setGamePiece(gamepieces[gamepieceNum]);
		gamepieces.erase(gamepieces.begin() + gamepieceNum); // Delete chosen one from list

		players[slotNums[i]].setBalance(1500);
		players[slotNums[i]].setLocation(0);
		for (int j = 0; j < 40; j++) {
			players[slotNums[i]].setOwnedProperty(j, false);
		}
		players[slotNums[i]].setIsBankrupt(false);

	}
}

bool Board::hasProperties(int playerIndex) {
	for (int i = 0; i < 40; i++) {
		if (players[playerIndex].getOwnedProperty(i) == true) {
			return true;
		}
	}
	return false;
}

std::vector<std::string> Board::getOwnedProperties(int playerIndex) {
	std::vector<std::string> properties;
	for (int i = 0; i < 40; i++) {
		if (players[playerIndex].getOwnedProperty(i) == true) {
			properties.push_back(positions[i].getName());
		}
	}
	return properties;
}

void Board::buyProperty(int playerIndex, int propertyIndex) {
	// Pay price to bank
	players[playerIndex].payMoney(positions[propertyIndex].getPrice());

	// Set to owned by player
	players[playerIndex].setOwnedProperty(propertyIndex, true);
	positions[propertyIndex].setOwner(playerIndex);
}

int Board::sellProperty(int playerIndex, int propertyIndex) {

	// Player gets price amount from bank
	players[playerIndex].receiveMoney(positions[propertyIndex].getPrice());

	// Set remove player's ownership
	players[playerIndex].setOwnedProperty(propertyIndex, false);
	positions[propertyIndex].setOwner(-1);

	int houseMoney = 0;
	// If player built houses compensate 1/2 price for each house
	if (positions[propertyIndex].getHouses() != 0) {
		houseMoney = positions[propertyIndex].getHousePrice() * positions[propertyIndex].getHouses() * 0.5;
		players[playerIndex].receiveMoney(houseMoney);
	}
	int soldFor = houseMoney + positions[propertyIndex].getPrice();

	positions[propertyIndex].reset();

	return soldFor;
}

void Board::buyHouse(int playerIndex, int propertyIndex) {
	// Pay price to build house
	players[playerIndex].payMoney(positions[propertyIndex].getHousePrice());
	std::cout << "You have built a house on " << positions[propertyIndex].getName() << "\n";
	// Build house
	positions[propertyIndex].buildHouse();
}

int Board::calculateRent(int propertyIndex, int diceRoll) {
	// Calculate rent for property, railroad, or utility
	if (positions[propertyIndex].getPosType() == "Property") {
		return positions[propertyIndex].getRent();
	}

	else if (positions[propertyIndex].getPosType() == "Railroad") {
		int owner = positions[propertyIndex].getOwner();
		int numRailroads = 0;
		for (int i = 5; i < 36; i = i + 10) {
			if (players[owner].getOwnedProperty(i) == true) {
				numRailroads++;
			}
		}
		switch (numRailroads) {
		case 1:
			return 25;
			break;
		case 2:
			return 50;
			break;
		case 3:
			return 100;
			break;
		case 4:
			return 200;
		default:
			return 0;
		}
	}
	//Utilities
	else {
		// Find number of utilities owner owns
		int owner = positions[propertyIndex].getOwner();
		int numUtilities = 0;
		if (players[owner].getOwnedProperty(12) == true) {
			numUtilities++;
		}
		if (players[owner].getOwnedProperty(28) == true) {
			numUtilities++;
		}

		switch (numUtilities) {
		case 1:
			return 4 * diceRoll;
			break;
		case 2:
			return 10 * diceRoll;
			break;
		}
	}
}

void Board::payRent(int playerIndex, int ownerIndex, int amount) {
	players[playerIndex].payMoney(amount);
	players[ownerIndex].receiveMoney(amount);
}

void Board::bankrupt(int playerIndex) {
	players[playerIndex].setIsBankrupt(true);

	// Reset owned properties
	for (int i = 0; i < 40; i++) {
		if (players[playerIndex].getOwnedProperty(i) == true) {
			positions[i].reset();
		}
	}

	std::cout << players[playerIndex].getPlayerName() << " has gone bankrupt and is now out of the game.\n";
}