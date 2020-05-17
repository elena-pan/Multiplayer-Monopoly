#include "Player.h"
#include <iostream>
#include <string>

// Set values
void Player::setPlayerName(std::string iPlayerName) {
	playerName = iPlayerName;
}

void Player::setGamePiece(std::string iGamePiece) {
	gamePiece = iGamePiece;
}

void Player::setBalance(int iBalance) {
	balance = iBalance;
}

void Player::setLocation(int iLocation) {
	location = iLocation;
}

void Player::setOwnedProperty(int index, bool own) {
	ownedProperty[index] = own;
}

void Player::setIsBankrupt(bool iIsBankrupt) {
	isBankrupt = iIsBankrupt;
}

// Get values
std::string Player::getPlayerName() {
	return playerName;
}

std::string Player::getGamePiece() {
	return gamePiece;
}

int Player::getBalance() {
	return balance;
}

int Player::getLocation() {
	return location;
}

bool Player::getOwnedProperty(int index) {
	return ownedProperty[index];
}

bool Player::getIsBankrupt() {
	return isBankrupt;
}


// Other
void Player::movePlayer(int steps, int moveTo) {
	if (moveTo == -1) {
		int temp = location + steps;
		if (temp > 39) {
			location = temp - 40;
		}
		else {
			location = temp;
		}
	}
	else {
		location = moveTo;
	}
}

void Player::payMoney(int amount) {
	balance = balance - amount;
}

void Player::receiveMoney(int amount) {
	balance = balance + amount;
}