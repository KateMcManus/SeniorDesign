#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "include/main.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QUdpSocket(this);

    //Hide everything until command recieved
    toggleUI();

    //Bind UDP socket and connect
    socket->bind(QHostAddress::AnyIPv4, port);
    connected = connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

//    QTimer *timer = new QTimer(this);
//    connect(timer, SIGNAL(timeout()), this, SLOT(timedUDP()));
//    timer->start(10);
    readyRead();
}

MainWindow::~MainWindow()
{
    delete ui;

    socket->abort();
    socket->close();
    delete socket;
}

void MainWindow::readyRead()
{
    QHostAddress sender;
    uint16_t port;
    while (socket->hasPendingDatagrams())
    {
               QByteArray datagram;
               datagram.resize(socket->pendingDatagramSize());
               socket->readDatagram(datagram.data(),datagram.size(),&sender,&port);
               qDebug() <<"Message From: " << sender.toString();
               qDebug() <<"Port: "<< port;
               qDebug() <<"Message: " << datagram;
              // ui->lblTestUDP->setText("Message: " + datagram);

               QString dataAsString = QString(datagram);

               if(!runRecieved && dataAsString.contains(setupConfirmMSG))
               {
                   toggleUI();
                   runRecieved = true;
                   ui->lblWaiting->setVisible(false);

                   //Open settings window
                   settingsOpened = true;
                   settings = new EMCSettings(this);
                   settings->setModal(true);
                   settings->show();

                   ui->lblTestUDP->setText("Message: " + datagram);
               }
               else if(dataAsString.contains(runConfirmMSG))
               {
                   ui->lblTestUDP->setText("Message: " + datagram);
                   ui->btnEMCCommand->setVisible(true);
               }
               else
               {
                  double a,c,d,e,f,g,h;
                  int b;
                  sscanf(datagram, "%lf,%i,%lf,%lf,%lf,%lf,%lf,%lf", &a,&b,&c,&d,&e,&f,&g,&h);
                  emcData.vdcSetPoint_0_30V = a;
                  emcData.vdsBlackoutStatus = b;
                  emcData.planetaryCommand = c;
                  emcData.horizontalCommand = d;
                  emcData.heaveCommand = e;
                  emcData.rollCommand = f;
                  emcData.yawCommand = g;
                  emcData.pitchCommand = h;

                  displayEMCData();
               }
    }
}

void MainWindow::on_btnConfirm_clicked()
{
    if(settingsOpened) //crashes if settings window has not been opened yet
    {
        char outText[18];
        snprintf(outText, sizeof(outText), "%i,%i,%i,%i,%i,%i,%i\n",
                 settings->accepted,
                 settings->pitchType,
                 settings->rollType,
                 settings->yawType,
                 settings->horizontalType,
                 settings->heaveType,
                 settings->planetaryType);
        emcSettings = outText;

        ui->lblPitchSetting->setText("Pitch: " + QString::number(settings->pitchType));
        ui->lblRollSetting->setText("Roll: " +  QString::number(settings->rollType));
        ui->lblYawSetting->setText("Yaw: " +  QString::number(settings->yawType));
        ui->lblHorizSetting->setText("Horizontal: " +  QString::number(settings->horizontalType));
        ui->lblHeaveSetting->setText("Heave: " +  QString::number(settings->heaveType));
        ui->lblPlanetarySetting->setText("Planetary: " +  QString::number(settings->planetaryType));
    }
    if(settings->isSelected)
    {
        qDebug() << "Sending...";
        socket->writeDatagram(emcSettings.toStdString().c_str(), serverIP, port );
    }
}


void MainWindow::sendDatagram()
{
//    QByteArray Data;
//    QDataStream out(&Data, QIODevice::WriteOnly);
//    out << Data;
//    socket->writeDatagram(Data, QHostAddress("192.168.200.164"), port);

}

//Hides all UI elements
void MainWindow::toggleUI()
{
    if(isUIVisible)
    {
        ui->grpEMC->setVisible(false);
        ui->grpMC->setVisible(false);
        ui->grpSettings->setVisible(false);
        ui->btnEMCCommand->setVisible(false);
        ui->btnConfirm->setVisible(false);
        ui->lblTestUDP->setVisible(false);
        isUIVisible = false;
    }
    else
    {
        ui->grpEMC->setVisible(true);
        ui->grpMC->setVisible(true);
        ui->grpSettings->setVisible(true);
        //ui->btnEMCCommand->setVisible(true);
        ui->btnConfirm->setVisible(true);
        ui->lblTestUDP->setVisible(true);
        isUIVisible = true;
    }
}

//TODO: Send this to server
void MainWindow::on_btnEMCCommand_clicked()
{
    qDebug() << "Sending Stop Command...";
    QByteArray Data;
    Data.append("Stop data collection.");
    socket->writeDatagram(Data, serverIP, port);

    socket->abort();
    socket->close();
}

//////////////////DATA DISPLAY//////////////////////
void MainWindow::displayMCData()
{
    ui->lblElapsedRuntime->setText("Elapsed Runtime: " + QString::number(mcData.elapsedRuntime));
    ui->lblPitchActualPos->setText("Pitch Actual Position: " + QString::number(mcData.pitchActualPosition));
    ui->lblPitchActualVel->setText("Pitch Actual Velocity: " + QString::number(mcData.pitchActualVelocity));
    ui->lblRollActualPos->setText("Roll Actual Position: " + QString::number(mcData.rollActualPosition));
    ui->lblRollActualVel->setText("Roll Actual Velocity: " + QString::number(mcData.rollActualVelocity));
    ui->lblYawActualPos->setText("Yaw Actual Position: " + QString::number(mcData.yawActualPosition));
    ui->lblYawActualVel->setText("Yaw Actual Velocity: " + QString::number(mcData.yawActualVelocity));
    ui->lblHorizontalActualPos->setText("Horizontal Actual Position: " + QString::number(mcData.horizontalActualPosition));
    ui->lblHorizontalActualPos->setText("Horizontal Actual Velocity: " + QString::number(mcData.horizontalActualVelocity));
    ui->lblHeaveActualPos->setText("Heave Actual Position: " + QString::number(mcData.heaveActualPosition));
    ui->lblHeaveActualPos->setText("Heave Actual Velocity: " + QString::number(mcData.heaveActualVelocity));
    ui->lblPlanetaryActualPos->setText("Planetary Actual Position: " + QString::number(mcData.planetarylActualPosition));
    ui->lblPlanetaryActualVel->setText("Planetary Actual Velocity: " + QString::number(mcData.planetaryActualVelocity));
}

void MainWindow::displayEMCData()
{
    ui->lblPitchCommand->setText("Pitch Command: " + QString::number(emcData.pitchCommand));
    ui->lblRollCommand->setText("Roll Command: " + QString::number(emcData.rollCommand));
    ui->lblYawCommand->setText("Yaw Command: " + QString::number(emcData.yawCommand));
    ui->lblHorizontalCommand->setText("Horizontal Command: " + QString::number(emcData.horizontalCommand));
    ui->lblHeaveCommand->setText("Heave Command: " + QString::number(emcData.heaveCommand));
    ui->lblPlanetaryCommand->setText("Planetary Command: " + QString::number(emcData.planetaryCommand));
    ui->lblSetPoint->setText("VDC Set Point: " + QString::number(emcData.vdcSetPoint_0_30V));
    ui->lblBlackoutStatus->setText("VDS Blackout Status: " + QString::number(emcData.vdsBlackoutStatus));
}
