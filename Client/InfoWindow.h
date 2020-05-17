#pragma once

#include <QDialog>
#include "ui_InfoWindow.h"
#include <string>

class InfoWindow : public QDialog, public Ui::InfoWindow
{
	Q_OBJECT

public:
	InfoWindow(std::string iInfoText, QWidget *parent = Q_NULLPTR);
	~InfoWindow();

private slots:
	void on_okButton_clicked();
};
