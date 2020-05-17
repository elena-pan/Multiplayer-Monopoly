#include "Position.h"
#include <string>

// Set values
void Position::setName(std::string iName) {
	name = iName;
}


void Position::setPosType(std::string iPosType) {
	posType = iPosType;
}

void Position::setPrice(int iPrice) {
	price = iPrice;
}

void Position::setOwner(int iOwner) {
	owner = iOwner;
}

void Position::setHousePrice(int iHousePrice) {
	housePrice = iHousePrice;
}

void Position::setHouses(int iHouses) {
	houses = iHouses;
}

void Position::setRent(int iRent) {
	rent = iRent;
}

void Position::setTax(int iTax) {
	tax = iTax;
}

void Position::setX(int iX) {
	x = iX;
}

void Position::setY(int iY) {
	y = iY;
}


// Get values
std::string Position::getName() {
	return name;
}

std::string Position::getPosType() {
	return posType;
}

int Position::getPrice() {
	return price;
}

int Position::getOwner() {
	return owner;
}

int Position::getHousePrice() {
	return housePrice;
}

int Position::getHouses() {
	return houses;
}

int Position::getRent() {
	return rent;
}

int Position::getTax() {
	return tax;
}

int Position::getX() {
	return x;
}

int Position::getY() {
	return y;
}

// Other
void Position::buildHouse() {
	switch (houses) {
	case 0:
		rent = rent * 5;
		break;
	case 1:
		rent = rent * 3;
		break;
	case 2:
		rent = rent * 3;
		break;
	case 3:
		rent = rent + 200;
		break;
	case 4:
		rent = rent + 200;
		break;
	}
	houses++;
}

void Position::sellHouse() {
	switch (houses) {
	case 1:
		rent = rent / 5;
		break;
	case 2:
		rent = rent / 3;
		break;
	case 3:
		rent = rent / 3;
		break;
	case 4:
		rent = rent - 200;
		break;
	case 5:
		rent = rent - 200;
		break;
	}
	houses--;
}

void Position::reset() {
	owner = -1;
	switch (houses) {
	case 0:
		break;
	case 1:
		rent = rent / 5;
		break;
	case 2:
		rent = rent / 15;
		break;
	case 3:
		rent = rent / 45;
		break;
	case 4:
		rent = (rent - 200) / 45;
		break;
	case 5:
		rent = (rent - 400) / 45;
		break;
	}
	houses = 0;
}
