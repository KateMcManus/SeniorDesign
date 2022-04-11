#include "emcsettings.h"
#include "ui_emcsettings.h"
#include <QMessageBox>

EMCSettings::EMCSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EMCSettings)
{
    ui->setupUi(this);

}

EMCSettings::~EMCSettings()
{
    delete ui;
}

void EMCSettings::on_btnSettingsConfirm_clicked()
{
    returnSettings();

    for(int i = 0; i < sizeof(selections); i++)
    {
        if(selections[i] == 0)
            missingSelections++;
    }

    if(missingSelections == 0)
    {
        isSelected = 1;
        accepted = 1;
        this->close();
    }
    else
    {
        QMessageBox::warning(this, "Setup Incomplete", "Missing " + QString::number(missingSelections) + " selections.");
        missingSelections = 0;
    }
}

void EMCSettings::returnSettings()
{
    //This is messy I don't like this?
    if(ui->rdoNoMotion_Pitch->isChecked())
    {
        pitchType = 0;
        selections[0] = 1;
    }
    else if (ui->rdoPosition_Pitch->isChecked())
    {
        pitchType = 1;
        selections[0] = 1;
    }
    else if (ui->rdoVelocity_Pitch->isChecked())
    {
        pitchType = 2;
        selections[0] = 1;
    }

    if(ui->rdoNoMotion_Roll->isChecked())
    {
        rollType = 0;
        selections[1] = 1;
    }
    else if(ui->rdoPosition_Roll->isChecked())
    {
        rollType = 1;
        selections[1] = 1;
    }
    else if(ui->rdoVelocity_Roll->isChecked())
    {
        rollType = 2;
        selections[1] = 1;
    }
    if(ui->rdoNoMotion_Yaw->isChecked())
    {
        yawType = 0;
        selections[2] = 1;
    }
    else if(ui->rdoPostion_Yaw->isChecked())
    {
        yawType = 1;
        selections[2] = 1;
    }
    else if(ui->rdoVelocity_Yaw->isChecked())
    {
        yawType = 2;
        selections[2] = 1;
    }

    if(ui->rdoNoMotion_Horiz->isChecked())
    {
        horizontalType = 0;
        selections[3] = 1;
    }
    else if(ui->rdoPosition_Horiz->isChecked())
    {
        horizontalType = 1;
        selections[3] = 1;
    }

    if(ui->rdoNoMotion_Heave->isChecked())
    {
        heaveType = 0;
        selections[4] = 1;
    }
    else if(ui->rdoPosition_Heave->isChecked())
    {
        heaveType = 1;
        selections[4] = 1;
    }

    if(ui->rdoNoMotion_Plan->isChecked())
    {
        planetaryType = 0;
        selections[5] = 1;
    }
    else if(ui->rdoPosition_Plan->isChecked())
    {
        planetaryType = 1;
        selections[5] = 1;
    }
    else if(ui->rdoVelocity_Plan->isChecked())
    {
        planetaryType = 2;
        selections[5] = 1;
    }

}


void EMCSettings::on_pushButton_clicked()
{
    ui->rdoPosition_Pitch->setChecked(true);
    ui->rdoPosition_Roll->setChecked(true);
    ui->rdoPostion_Yaw->setChecked(true);
    ui->rdoPosition_Horiz->setChecked(true);
    ui->rdoPosition_Heave->setChecked(true);
    ui->rdoPosition_Plan->setChecked(true);

    pitchType = 1;
    rollType = 1;
    yawType = 1;
    horizontalType = 1;
    heaveType = 1;
    planetaryType = 1;
}

