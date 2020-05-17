#pragma once

#include <QDialog>
#include "ui_PropertiesListSelection.h"
#include <string>
#include <vector>

class PropertiesListSelection : public QDialog, public Ui::PropertiesListSelection
{
	Q_OBJECT
	std::string purpose;
	std::vector<int> sellFor;

public:
	PropertiesListSelection(std::vector<std::string> iPropertiesList, std::string iButtonText = "View Info", std::vector<int> sellFor = {}, QWidget* parent = Q_NULLPTR);
	~PropertiesListSelection();

private slots:
	void on_propertiesList_currentItemChanged();
	void on_actionButton_clicked();
	void on_cancelButton_clicked();
};
