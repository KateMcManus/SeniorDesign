#ifndef EMCSETTINGS_H
#define EMCSETTINGS_H

#include <QDialog>
#include "include/EMCMessageDefinitions.hpp"

namespace Ui {
class EMCSettings;
}

class EMCSettings : public QDialog
{
    Q_OBJECT

public:
    explicit EMCSettings(QWidget *parent = nullptr);
    ~EMCSettings();
    //EMC_RUN_SETUP_STRUCT emcSetupSelections;
    bool isSelected = 0;

    int accepted = 0;
    int pitchType = 0;
    int rollType = 0;
    int yawType = 0;
    int planetaryType = 0;
    int heaveType = 0;
    int horizontalType = 0;

private slots:
    void on_btnSettingsConfirm_clicked();

    void on_pushButton_clicked();

private:
    Ui::EMCSettings *ui;
    void returnSettings();

    bool selections[6] = {0,0,0,0,0,0};
    int missingSelections = 0;

};

#endif // EMCSETTINGS_H
