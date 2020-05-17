#include "InfoWindow.h"

InfoWindow::InfoWindow(std::string iInfoText, QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);
	infoTextLabel->setText(iInfoText.c_str());
	setWindowTitle("");
}

InfoWindow::~InfoWindow()
{
}

void InfoWindow::on_okButton_clicked() {
	close();
}