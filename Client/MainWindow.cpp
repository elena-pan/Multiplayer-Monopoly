#include "MainWindow.h"
#include "InfoWindow.h"
#include "PlayerChoiceWindow.h"
#include "PropertiesListSelection.h"

extern uint16 thisPlayerIndex;
extern bool buyPropertyClicked, sellPropertyClicked, buildHouseClicked, endTurnClicked, leaveGameClicked;
extern uint16 sellPropertyIndex;

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.scottieDogImage->setGeometry(510, 540, 41, 41);
	ui.thimbleImage->setGeometry(510, 540, 21, 31);
	ui.racecarImage->setGeometry(510, 540, 51, 31);
	ui.battleshipImage->setGeometry(510, 540, 41, 41);
	ui.ironImage->setGeometry(510, 540, 31, 31);
	ui.topHatImage->setGeometry(510, 540, 31, 31);
	ui.shoeImage->setGeometry(510, 540, 31, 31);
	ui.wheelbarrowImage->setGeometry(510, 540, 41, 41);

	ui.scottieDogImage->hide();
	ui.thimbleImage->hide();
	ui.racecarImage->hide();
	ui.battleshipImage->hide();
	ui.ironImage->hide();
	ui.topHatImage->hide();
	ui.shoeImage->hide();
	ui.wheelbarrowImage->hide();

	// Set coordinates for gamepieces
	for (int i = 0; i < 40; i++) {
		if (i <= 10) {
			positions[i].setY(540);
		}
		else if (10 < i && i <= 20) {
			positions[i].setX(15);
		}
		else if (20 < i && i <= 30) {
			positions[i].setY(50);
		}
		else {
			positions[i].setX(510);
		}
	}

	positions[0].setX(510);
	positions[1].setX(450);
	positions[2].setX(400);
	positions[3].setX(350);
	positions[4].setX(310);
	positions[5].setX(270);
	positions[6].setX(220);
	positions[7].setX(170);
	positions[8].setX(130);
	positions[9].setX(90);
	positions[10].setX(15);
	positions[11].setY(470);
	positions[12].setY(430);
	positions[13].setY(390);
	positions[14].setY(340);
	positions[15].setY(290);
	positions[16].setY(250);
	positions[17].setY(210);
	positions[18].setY(170);
	positions[19].setY(110);
	positions[20].setY(50);
	positions[21].setX(90);
	positions[22].setX(130);
	positions[23].setX(170);
	positions[24].setX(220);
	positions[25].setX(270);
	positions[26].setX(310);
	positions[27].setX(350);
	positions[28].setX(400);
	positions[29].setX(450);
	positions[30].setX(510);
	positions[31].setY(110);
	positions[32].setY(170);
	positions[33].setY(210);
	positions[34].setY(250);
	positions[35].setY(290);
	positions[36].setY(340);
	positions[37].setY(390);
	positions[38].setY(430);
	positions[39].setY(470);

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
	chanceCards[1].setAmountMoney(50);
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
	communityChestCards[5].setAmountMoney(80);
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

void MainWindow::createPlayers(std::vector<std::string> names, std::vector<std::string> gamepieces, std::vector<uint16> slotNums) {
	players = new Player[4];

	for (int i = 0; i < slotNums.size(); i++) {
		players[slotNums[i]].setPlayerName(names[i]);
		players[slotNums[i]].setGamePiece(gamepieces[i]);

		showGamepiece(gamepieces[i]);

		players[slotNums[i]].setBalance(1500);
		players[slotNums[i]].setLocation(0);
		for (int j = 0; j < 40; j++) {
			players[slotNums[i]].setOwnedProperty(j, false);
		}
		players[slotNums[i]].setIsBankrupt(false);

	}
}

std::vector<std::string> MainWindow::getOwnedProperties() {
	std::vector<std::string> properties;
	for (int i = 0; i < 40; i++) {
		if (players[thisPlayerIndex].getOwnedProperty(i) == true) {
			properties.push_back(positions[i].getName());
		}
	}
	return properties;
}

int MainWindow::findProperty(std::string propertyName) {

	// Get index of property from name
	for (int i = 0; i < 40; i++) {
		if (positions[i].getName() == propertyName) {
			return i;
		}
	}
}

void MainWindow::buyProperty(int propertyIndex) {
	// Pay price to bank
	players[thisPlayerIndex].payMoney(positions[propertyIndex].getPrice());

	// Set to owned by player
	players[thisPlayerIndex].setOwnedProperty(propertyIndex, true);
	positions[propertyIndex].setOwner(thisPlayerIndex);
}

void MainWindow::sellProperty(int propertyIndex, int soldFor) {

	// Player gets price amount from bank
	players[thisPlayerIndex].receiveMoney(soldFor);

	// Set remove player's ownership
	players[thisPlayerIndex].setOwnedProperty(propertyIndex, false);
	positions[propertyIndex].setOwner(-1);

	// Reset property
	positions[propertyIndex].reset();
}

void MainWindow::buyHouse(int propertyIndex) {
	// Pay price to build house
	players[thisPlayerIndex].payMoney(positions[propertyIndex].getHousePrice());

	// Build house
	positions[propertyIndex].buildHouse();
}

bool MainWindow::noMoneyOptions() {
	
	PlayerChoiceWindow noMoneyWindow("You don't have enough money to pay!", "Sell Property", "Give Up :(");
	if (noMoneyWindow.exec()) {
		std::vector<int> sellFor;
		for (int i = 0; i < 40; i++) {
			if (players[thisPlayerIndex].getOwnedProperty(i) == true) {
				int price = positions[i].getPrice();
				if (positions[i].getHouses() < 0) {
					price = price + (positions[i].getHouses() * positions[i].getHousePrice() * 0.5);
				}
				sellFor.push_back(price);
			}
		}
		PropertiesListSelection chooseSellProperty(getOwnedProperties(), "Sell Property", sellFor);
		if (chooseSellProperty.exec()) {
			std::string propertyName = chooseSellProperty.propertiesList->currentItem()->text().toStdString();
			sellPropertyIndex = findProperty(propertyName);
			sellPropertyClicked = true;
		}
		return true;
	}
	else {
		return false;
	}
}

void MainWindow::bankrupt(int playerIndex) {

	players[playerIndex].setIsBankrupt(true);

	// Reset owned properties
	for (int i = 0; i < 40; i++) {
		if (players[playerIndex].getOwnedProperty(i) == true) {
			positions[i].reset();
		}
	}
	hideGamepiece(players[playerIndex].getGamePiece());
}

void MainWindow::updateText(std::string iText) {
	// Move all rows up by one, discard top row text, replace bottom one with new text
	text1 = text2;
	text2 = text3;
	text3 = iText;
	ui.textLabel1->setText(text1.c_str());
	ui.textLabel2->setText(text2.c_str());
	ui.textLabel3->setText(text3.c_str());
}

void MainWindow::disableButton(std::string iButton) {
	if (iButton == "buyProperty") {
		ui.buyPropertyButton->setDisabled(true);
	}
	else if (iButton == "sellProperty") {
		ui.sellPropertyButton->setDisabled(true);
	}
	else if (iButton == "buildHouse") {
		ui.buildHouseButton->setDisabled(true);
	}
	else if (iButton == "endTurn") {
		ui.endTurnButton->setDisabled(true);
	}
}

void MainWindow::enableButton(std::string iButton) {
	if (iButton == "buyProperty") {
		ui.buyPropertyButton->setEnabled(true);
	}
	else if (iButton == "sellProperty") {
		ui.sellPropertyButton->setEnabled(true);
	}
	else if (iButton == "buildHouse") {
		ui.buildHouseButton->setEnabled(true);
	}
	else if (iButton == "endTurn") {
		ui.endTurnButton->setEnabled(true);
	}
}

void MainWindow::hideGamepiece(std::string iGamepiece) {
	if (iGamepiece == "Scottie Dog") {
		ui.scottieDogImage->hide();
	}
	else if (iGamepiece == "Thimble") {
		ui.thimbleImage->hide();
	}
	else if (iGamepiece == "Racecar") {
		ui.racecarImage->hide();
	}
	else if (iGamepiece == "Battleship") {
		ui.battleshipImage->hide();
	}
	else if (iGamepiece == "Iron") {
		ui.ironImage->hide();
	}
	else if (iGamepiece == "Top Hat") {
		ui.topHatImage->hide();
	}
	else if (iGamepiece == "Shoe") {
		ui.shoeImage->hide();
	}
	else if (iGamepiece == "Wheelbarrow") {
		ui.wheelbarrowImage->hide();
	}
}

void MainWindow::showGamepiece(std::string iGamepiece) {
	if (iGamepiece == "Scottie Dog") {
		ui.scottieDogImage->show();
	}
	else if (iGamepiece == "Thimble") {
		ui.thimbleImage->show();
	}
	else if (iGamepiece == "Racecar") {
		ui.racecarImage->show();
	}
	else if (iGamepiece == "Battleship") {
		ui.battleshipImage->show();
	}
	else if (iGamepiece == "Iron") {
		ui.ironImage->show();
	}
	else if (iGamepiece == "Top Hat") {
		ui.topHatImage->show();
	}
	else if (iGamepiece == "Shoe") {
		ui.shoeImage->show();
	}
	else if (iGamepiece == "Wheelbarrow") {
		ui.wheelbarrowImage->show();
	}
}

void MainWindow::moveGamepiece(int playerIndex, int location) {
	std::string gamepiece = players[playerIndex].getGamePiece();
	if (gamepiece == "Scottie Dog") {
		ui.scottieDogImage->setGeometry(positions[location].getX(), positions[location].getY(), 41, 41);
	}
	else if (gamepiece == "Thimble") {
		ui.thimbleImage->setGeometry(positions[location].getX(), positions[location].getY(), 21, 31);
	}
	else if (gamepiece == "Racecar") {
		ui.racecarImage->setGeometry(positions[location].getX(), positions[location].getY(), 51, 31);
	}
	else if (gamepiece == "Battleship") {
		ui.battleshipImage->setGeometry(positions[location].getX(), positions[location].getY(), 41, 41);
	}
	else if (gamepiece == "Iron") {
		ui.ironImage->setGeometry(positions[location].getX(), positions[location].getY(), 31, 31);
	}
	else if (gamepiece == "Top Hat") {
		ui.topHatImage->setGeometry(positions[location].getX(), positions[location].getY(), 31, 31);
	}
	else if (gamepiece == "Shoe") {
		ui.shoeImage->setGeometry(positions[location].getX(), positions[location].getY(), 31, 31);
	}
	else if (gamepiece == "Wheelbarrow") {
		ui.wheelbarrowImage->setGeometry(positions[location].getX(), positions[location].getY(), 41, 41);
	}
}

void MainWindow::on_myInfoButton_clicked() {
	std::string myInfoText = players[thisPlayerIndex].getPlayerName() +
		"\nGamepiece: " + players[thisPlayerIndex].getGamePiece() +
		"\nBalance: $" + std::to_string(players[thisPlayerIndex].getBalance());
	std::vector<std::string> ownedProperties = getOwnedProperties();
	if (ownedProperties.size() != 0) {
		myInfoText = myInfoText + "\n\nProperties: ";
		for (int i = 0; i < ownedProperties.size(); i++) {
			myInfoText = myInfoText + "\n" + ownedProperties[i];
		}
	}
	myInfoText = myInfoText + "\n\nBankrupt: ";
	if (players[thisPlayerIndex].getIsBankrupt()) {
		myInfoText = myInfoText + "Yes";
	}
	else {
		myInfoText = myInfoText + "No";
	}
	InfoWindow myInfoWindow(myInfoText);
	myInfoWindow.exec();
}

void MainWindow::on_propertyDetailsButton_clicked() {
	std::vector<std::string> allPropertiesList;
	for (int i = 0; i < 40; i++) {
		if (positions[i].getPosType() == "Property" || positions[i].getPosType() == "Railroad" || positions[i].getPosType() == "Utility") {
			allPropertiesList.push_back(positions[i].getName());
		}
	}
	PropertiesListSelection viewPropertyInfo(allPropertiesList);
	if (viewPropertyInfo.exec()) {
		int propertyIndex = findProperty(viewPropertyInfo.propertiesList->currentItem()->text().toStdString());
		std::string infoText = positions[propertyIndex].getName() + "\n" + positions[propertyIndex].getPosType() +
			"\nPrice: $" + std::to_string(positions[propertyIndex].getPrice());
		
		// Display rent for each number of houses for properties
		if (positions[propertyIndex].getPosType() == "Property") {
			int baseRent = positions[propertyIndex].getRent();
			switch (positions[propertyIndex].getHouses()) {
			case 0:
				break;
			case 1:
				baseRent = baseRent / 5;
				break;
			case 2:
				baseRent = baseRent / 15;
				break;
			case 3:
				baseRent = baseRent / 45;
				break;
			case 4:
				baseRent = (baseRent - 200) / 45;
				break;
			case 5:
				baseRent = (baseRent - 400) / 45;
				break;

			}
			infoText = infoText + "\n\nRent: \n0 Houses $" + std::to_string(baseRent);
			infoText = infoText + "\n1 House $" + std::to_string(baseRent * 5);
			infoText = infoText + "\n2 Houses $" + std::to_string(baseRent * 15);
			infoText = infoText + "\n3 Houses $" + std::to_string(baseRent * 45);
			infoText = infoText + "\n4 Houses $" + std::to_string((baseRent * 45) + 200);
			infoText = infoText + "\n5 Houses (hotel) $" + std::to_string((baseRent * 45) + 400);

			infoText = infoText + "\n\nPrice for a House: " + std::to_string(positions[propertyIndex].getHousePrice());
		}
		else if (positions[propertyIndex].getPosType() == "Utility") {
			infoText = infoText + "\n\nRent: \n1 Utility Owned: $" + std::to_string(positions[propertyIndex].getRent()) + " * dice roll";
			infoText = infoText + "\n2 Utilities Owned: $" + std::to_string(10) + " * dice roll";
		}
		else if (positions[propertyIndex].getPosType() == "Railroad") {
			infoText = infoText + "\n\nRent: \n1 Railroad Owned: $" + std::to_string(25);
			infoText = infoText + "\n2 Railroads Owned: $" + std::to_string(50);
			infoText = infoText + "\n3 Railroads Owned: $" + std::to_string(100);
			infoText = infoText + "\n4 Railroads Owned: $" + std::to_string(200);
		}

		if (positions[propertyIndex].getOwner() == -1) {
			infoText = infoText + "\nOwner: None";
		}

		else {
			infoText = infoText + "\nOwner: " + players[positions[propertyIndex].getOwner()].getPlayerName();
			if (positions[propertyIndex].getPosType() == "Property") {
				infoText = infoText + "\nHouses Built: " + std::to_string(positions[propertyIndex].getHouses());
			}
		}
		InfoWindow propertyInfoWindow(infoText);
		propertyInfoWindow.infoTextLabel->setAlignment(Qt::AlignLeft);
		propertyInfoWindow.exec();
	}
}

void MainWindow::on_buyPropertyButton_clicked() {
	buyPropertyClicked = true;
}

void MainWindow::on_sellPropertyButton_clicked() {
	std::vector<int> sellFor;
	for (int i = 0; i < 40; i++) {
		if (players[thisPlayerIndex].getOwnedProperty(i) == true) {
			int price = positions[i].getPrice();
			if (positions[i].getHouses() < 0) {
				price = price + (positions[i].getHouses() * positions[i].getHousePrice() * 0.5);
			}
			sellFor.push_back(price);
		}
	}

	PropertiesListSelection chooseSellProperty(getOwnedProperties(), "Sell Property", sellFor);
	if (chooseSellProperty.exec()) {
		std::string propertyName = chooseSellProperty.propertiesList->currentItem()->text().toStdString();
		sellPropertyIndex = findProperty(propertyName);
		sellPropertyClicked = true;
	}
}

void MainWindow::on_buildHouseButton_clicked() {
	buildHouseClicked = true;
}

void MainWindow::on_endTurnButton_clicked() {
	endTurnClicked = true;
}

void MainWindow::on_leaveGameButton_clicked() {
	leaveGameClicked = true;
}
