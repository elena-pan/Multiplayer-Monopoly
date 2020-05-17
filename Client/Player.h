#pragma once
#include <string>

class Player
{
	std::string playerName;
	std::string gamePiece;
	int balance; // Amount of money
	int location;
	bool ownedProperty[40];
	bool isBankrupt;

public:

	// Set values
	void setPlayerName(std::string iPlayerName);
	void setGamePiece(std::string iGamePiece);
	void setBalance(int iBalance);
	void setLocation(int iLocation);
	void setOwnedProperty(int index, bool own);
	void setIsBankrupt(bool iIsBankrupt);

	// Get values
	std::string getPlayerName();
	std::string getGamePiece();
	int getBalance();
	int getLocation();
	bool getOwnedProperty(int index);
	bool getIsBankrupt();

	// Other
	void movePlayer(int steps, int moveTo = -1);
	void payMoney(int amount);
	void receiveMoney(int amount);

};