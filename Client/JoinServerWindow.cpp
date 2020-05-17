#include "JoinServerWindow.h"
#include <qregularexpression.h>
#include <qvalidator.h>

JoinServerWindow::JoinServerWindow(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);
	setWindowTitle("Join Monopoly Server");

	// Exclude special characters from name
	QRegularExpression regexp("[a-zA-Z][a-zA-Z0-9._?! ]+");
	QValidator* validator = new QRegularExpressionValidator(regexp, this);
	nameEdit->setValidator(validator);

	nameEdit->setMaxLength(20);
	joinServerButton->setDisabled(true);
	
}

JoinServerWindow::~JoinServerWindow()
{
}

void JoinServerWindow::on_joinServerButton_clicked() {
	accept();
}

void JoinServerWindow::on_cancelButton_clicked() {
	reject();
}

void JoinServerWindow::on_nameEdit_textChanged() {
	if (nameEdit->text().isEmpty()) {
		joinServerButton->setDisabled(true);
	}
	else {
		joinServerButton->setEnabled(true);
	}
}