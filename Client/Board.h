#pragma once
#include "WinSock.h"
#include "Position.h"
#include "Card.h"
#include "Player.h"
#include <string>
#include <vector>

class Board
{
public:

	Position positions[40];
	Card chanceCards[11];
	Card communityChestCards[15];
	Player* players;

	// constructor
	Board();

	void createPlayers(std::vector<std::string> names, std::vector<uint16> slotNums);
	bool hasProperties(int playerIndex);
	std::vector<std::string> getOwnedProperties(int playerIndex);
	void buyProperty(int playerIndex, int propertyIndex);
	int sellProperty(int playerIndex, int propertyIndex);
	void buyHouse(int playerIndex, int propertyIndex);
	int calculateRent(int propertyIndex, int diceRoll = -1);
	void payRent(int playerIndex, int ownerIndex, int amount);
	void bankrupt(int playerIndex);
};
