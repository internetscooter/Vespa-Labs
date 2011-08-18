#include "sciimainwindow.h"
#include "ui_sciimainwindow.h"
#include <QTextStream>
#include <QDateTime>
#include <iostream>
#include <QElapsedTimer>
#include <QtDebug>


SCIIMainWindow::SCIIMainWindow(QWidget *parent) :
    QMainWindow(parent),
    // wheelSpeed(0,1.345), // Diameter of a Sava MC18 10inch
    // wheelSpeed(0,1.314), // Heinadau racer 10inch
    wheelSpeed(0,1.360), // Sava MC31 10inch
    ui(new Ui::SCIIMainWindow)
{
    ui->setupUi(this);
    //ui->SVGButton->show();
    statusBar()->showMessage("Locating Vespa LabJack...");
    vespaLabJack = new LabJack();
    statusBar()->showMessage("Vespa LabJack: " + vespaLabJack->status);
    vespaLabJack->ConfigureStreamed();
    statusBar()->showMessage("Vespa LabJack: " + vespaLabJack->status);

    // Quick and dirty Temp file logging to capture some data
    // no error checking! Comment out when not being used
    // logfile = new QFile( "vespa.log" );
    // logfile->open(QIODevice::WriteOnly | QIODevice::Text);

    //update screen every x seconds
    QTimer *timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(slotUpdate()));
    timer->start(100);

    //simulate pulses (requires FI05 to be connected to FIO4)
//    pulseTimer = new QTimer();
//    connect(pulseTimer, SIGNAL(timeout()), this, SLOT(slotPulseGen()));
//    //pulseTimer->setSingleShot(true);
//    pulseTimer->start(250);

    //labjackThread->start();

    sim = 250; //used later for simulating input
}

void SCIIMainWindow::slotUpdate(void)
{
    //double period_us = vespaLabJack->GetTimer0Value();
     vespaLabJack->StreamUpdate();

    //overide value to simulate acceleration
//    sim -= 1000;
//    period_us = sim;

    // need to add reset timer here and handle "inf" period as zero speed
    //wheelSpeed.set_period_s(period_us / 1000 / 1000); //set period in seconds



    // Quick and dirty Temp file logging to capture some data
    // no error checking! Comment out when not being used
//    QDateTime now = QDateTime::currentDateTime();
//    QTextStream out(logfile);
//    out << now.toString()
//            << "," << wheelSpeed.ms()
//            << "," << wheelSpeed.kmph()
//            << "," << wheelSpeed.mss()
//            << "," << wheelSpeed.hp() << "\n";

    //statusBar()->showMessage("Vespa LabJack: " + vespaLabJack->status);
//    statusBar()->showMessage("Vespa LabJack: " + QString("%1").arg(wheelSpeed.kmph()*1000));
//    ui->lcdNumber->display(QString("%1").arg(wheelSpeed.kmph(),0,'f',2));
//    ui->svgSpeedometer->setValue(wheelSpeed.kmph()*1000);
}

void SCIIMainWindow::slotPulseGen(void)
{
     vespaLabJack->CreateTestPulse(1);
    //sim = sim - 10;
        //pulseTimer->setSingleShot(true);
    //pulseTimer->start(sim);

}

SCIIMainWindow::~SCIIMainWindow()
{
    delete ui;
}

