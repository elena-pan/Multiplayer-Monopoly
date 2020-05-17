#include "PlayerChoiceWindow.h"

PlayerChoiceWindow::PlayerChoiceWindow(std::string iText, std::string iButtonText, std::string iButtonText2, QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);
	setWindowTitle("");
	textLabel->setText(iText.c_str());
	yesButton->setText(iButtonText.c_str());
	noButton->setText(iButtonText2.c_str());
	if (iButtonText == "Start Game") {
		yesButton->setStyleSheet("font: bold;");
		textLabel->setAlignment(Qt::AlignLeft);
	}
}

PlayerChoiceWindow::~PlayerChoiceWindow()
{
}

void PlayerChoiceWindow::setText(std::string iText) {
	textLabel->setText(iText.c_str());
}

void PlayerChoiceWindow::disableStartButton() {
	yesButton->setDisabled(true);
}

void PlayerChoiceWindow::enableStartButton() {
	yesButton->setEnabled(true);
}

void PlayerChoiceWindow::on_yesButton_clicked() {
	accept();
}

void PlayerChoiceWindow::on_noButton_clicked() {
	reject();
}