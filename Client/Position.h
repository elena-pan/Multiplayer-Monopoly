#pragma once
#include <string>

class Position
{
	std::string name;
	std::string posType;
	int price;
	int owner;
	//bool mortgaged;
	int housePrice;
	int houses;
	int rent;
	int tax;
	int x, y;

public:
	// Set values
	void setName(std::string iName);
	void setPosType(std::string iPosType);
	void setPrice(int iPrice);
	void setOwner(int iOwner);
	void setHousePrice(int iHousePrice);
	void setHouses(int iHouses);
	void setRent(int iRent);
	void setTax(int iTax);
	void setX(int iX);
	void setY(int iY);

	// Get values
	std::string getName();
	std::string getPosType();
	int getPrice();
	int getOwner();
	int getHousePrice();
	int getHouses();
	int getRent();
	int getTax();
	int getX();
	int getY();

	// Other
	void buildHouse();
	void sellHouse();
	void reset();
};