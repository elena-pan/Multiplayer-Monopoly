#pragma once

#include <QDialog>
#include "ui_JoinServerWindow.h"

class JoinServerWindow : public QDialog, public Ui::JoinServerWindow
{
	Q_OBJECT

public:
	JoinServerWindow(QWidget *parent = Q_NULLPTR);
	~JoinServerWindow();

private slots:
	void on_joinServerButton_clicked();
	void on_cancelButton_clicked();
	void on_nameEdit_textChanged();
};
