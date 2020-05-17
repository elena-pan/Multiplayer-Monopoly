#pragma once

#include <QDialog>
#include "ui_PasswordWindow.h"

class PasswordWindow : public QDialog, public Ui::PasswordWindow
{
	Q_OBJECT

public:
	PasswordWindow(QWidget *parent = Q_NULLPTR);
	~PasswordWindow();

private slots:
	void on_okButton_clicked();
	void on_cancelButton_clicked();
	void on_portEdit_textChanged();
};
