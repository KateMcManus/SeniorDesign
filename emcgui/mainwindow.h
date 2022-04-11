#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QDataStream>
#include <QTimer>
#include "include/EMCMessageDefinitions.hpp"
#include "emcsettings.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void readyRead();
    void sendDatagram();

    //Buttons
    void on_btnEMCCommand_clicked();

    //Data Display
    void displayMCData();
    void displayEMCData();

   //Called by other functions
    void on_btnConfirm_clicked();


private:
    EMCSettings *settings;
    Ui::MainWindow *ui;
    QUdpSocket *socket;
    QTimer *timer;

    UINT32 outgoingcount = 1;


    MC_RUN_TIME_DATA_STRUCT mcData;
    EMC_RUN_TIME_DATA_STRUCT emcData;
    //EMC_COMMAND_STRUCT stopEMC;

    QString settingsSelections[7];
    int port = 49314;
    bool connected = false;
    bool settingsOpened = false;
    bool runRecieved = false;
    bool isUIVisible = true;
    bool runtimeListen = false;
    QHostAddress serverIP =  QHostAddress("192.168.200.164");
    //QHostAddress serverIP =  QHostAddress("172.30.227.157");

    QString emcSettings = "";

    void toggleUI();
    void showSettings();

    QString setupConfirmMSG = "Received MC Run Setup Message.";
    QString runConfirmMSG = "Received Run Command.";


};
#endif // MAINWINDOW_H
