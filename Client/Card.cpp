#include "Card.h"
#include <string>

// Set values
void Card::setDescription(std::string iDescription) {
	description = iDescription;
}

void Card::setAmountMoney(int iAmountMoney) {
	amountMoney = iAmountMoney;
}

void Card::setMoveTo(int iMoveTo) {
	moveTo = iMoveTo;
}

void Card::setCardType(std::string iCardType) {
	cardType = iCardType;
}

// Get values
std::string Card::getDescription() {
	return description;
}

int Card::getAmountMoney() {
	return amountMoney;
}

int Card::getMoveTo() {
	return moveTo;
}

std::string Card::getCardType() {
	return cardType;
}