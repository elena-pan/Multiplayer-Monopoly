#include "PropertiesListSelection.h"
#include "PlayerChoiceWindow.h"

PropertiesListSelection::PropertiesListSelection(std::vector<std::string> iPropertiesList, std::string iButtonText, std::vector<int> iSellFor, QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);

    purpose = iButtonText;
    actionButton->setText(iButtonText.c_str());
    sellFor = iSellFor;

    for (int i = 0; i < iPropertiesList.size(); i++) {
        new QListWidgetItem(tr(iPropertiesList[i].c_str()), propertiesList);
    }
}

PropertiesListSelection::~PropertiesListSelection()
{
}

void PropertiesListSelection::on_actionButton_clicked() {

    QListWidgetItem* curItem = propertiesList->currentItem();
    std::string propertyName = curItem->text().toStdString();

    if (purpose == "Sell Property") {
        PlayerChoiceWindow sellPropertySure("Are you sure you want to sell " + propertyName + "?");
        if (sellPropertySure.exec()) {
            accept();
        }
    }
    else {
        accept();
    }
}

void PropertiesListSelection::on_cancelButton_clicked() {
    reject();
}

void PropertiesListSelection::on_propertiesList_currentItemChanged()
{
    QListWidgetItem* curItem = propertiesList->currentItem();

    // Display selected property (and sell price, if applicable)
    std::string propertyLabelText;
    if (curItem) {
        if (purpose == "Sell Property") {
            propertyLabelText = "Sell for $" + std::to_string(sellFor[propertiesList->currentRow()]);
        }
        else {
            propertyLabelText = curItem->text().toStdString();
        }
        propertyLabel->setText(propertyLabelText.c_str());

    }
    else {
        propertyLabel->setText("< No item selected >");
    }
}