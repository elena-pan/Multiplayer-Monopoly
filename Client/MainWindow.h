#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "WinSock.h"
#include "Position.h"
#include "Card.h"
#include "Player.h"
#include <string>
#include <vector>

class MainWindow : public QMainWindow
{
	Q_OBJECT
	std::string text1;
	std::string text2;
	std::string text3;

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	Position positions[40];
	Card chanceCards[11];
	Card communityChestCards[15];
	Player* players;
	
	void createPlayers(std::vector<std::string> names, std::vector<std::string> gamepieces, std::vector<uint16> slotNums);
	std::vector<std::string> getOwnedProperties();
	int findProperty(std::string propertyName);
	void buyProperty(int propertyIndex);
	void sellProperty(int propertyIndex, int soldFor);
	void buyHouse(int propertyIndex);
	bool noMoneyOptions();
	void bankrupt(int playerIndex);

	void updateText(std::string iText);
	void disableButton(std::string iButton);
	void enableButton(std::string iButton);
	void hideGamepiece(std::string iGamepiece);
	void showGamepiece(std::string iGamepiece);
	void moveGamepiece(int playerIndex, int location);
	
private:
	Ui::MonopolyClientClass ui;

private slots:
	void on_myInfoButton_clicked();
	void on_propertyDetailsButton_clicked();
	void on_buyPropertyButton_clicked();
	void on_sellPropertyButton_clicked();
	void on_buildHouseButton_clicked();
	void on_endTurnButton_clicked();
	void on_leaveGameButton_clicked();

};
