#pragma once

#include <QDialog>
#include "ui_PlayerChoiceWindow.h"
#include <string>

class PlayerChoiceWindow : public QDialog, public Ui::PlayerChoiceWindow
{
	Q_OBJECT

public:
	PlayerChoiceWindow(std::string iText, std::string iButtonText = "Yes", std::string iButtonText2 = "No", QWidget *parent = Q_NULLPTR);
	~PlayerChoiceWindow();
	void setText(std::string iText);
	void disableStartButton();
	void enableStartButton();


private slots:
	void on_yesButton_clicked();
	void on_noButton_clicked();
};
