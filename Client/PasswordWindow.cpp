#include "PasswordWindow.h"
#include <qregularexpression.h>
#include <qvalidator.h>

PasswordWindow::PasswordWindow(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);
	setWindowTitle("");

	// Exclude special characters from name
	QRegularExpression regexp("[0-9]+");
	QValidator* validator = new QRegularExpressionValidator(regexp, this);
	portEdit->setValidator(validator);

	portEdit->setMaxLength(5);
	okButton->setDisabled(true);
}

PasswordWindow::~PasswordWindow()
{
}

void PasswordWindow::on_okButton_clicked() {
	accept();
}

void PasswordWindow::on_cancelButton_clicked() {
	reject();
}

void PasswordWindow::on_portEdit_textChanged() {
	if (portEdit->text().isEmpty()) {
		okButton->setDisabled(true);
	}
	else {
		okButton->setEnabled(true);
	}
}