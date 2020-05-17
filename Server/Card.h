#pragma once
#include <string>

class Card
{
	std::string description;
	int amountMoney;
	int moveTo;
	std::string cardType;

public:

	// Set values
	void setDescription(std::string iDescription);
	void setAmountMoney(int iAmountMoney);
	void setMoveTo(int iMoveTo);
	void setCardType(std::string iCardType);

	// Get values
	std::string getDescription();
	int getAmountMoney();
	int getMoveTo();
	std::string getCardType();

};