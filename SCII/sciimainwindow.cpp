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
    vespaLabJack->Configure();
    statusBar()->showMessage("Vespa LabJack: " + vespaLabJack->status);

    // Quick and dirty Temp file logging to capture some data
    // no error checking! Comment out when not being used
    logfile = new QFile( "vespa.log" );
    logfile->open(QIODevice::WriteOnly | QIODevice::Text);


    //update screen every 0.1 seconds
    QTimer *timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(slotUpdate()));
    timer->start(100);  // this should be smarter and checked against system time
                        // we loose about 14ms per signal. Maybe Qt has a smarter timer??

    sim = 1000000;
}

void SCIIMainWindow::slotUpdate(void)
{
    double period_us = vespaLabJack->GetTimer0Value();

    //overide value to simulate acceleration
//    sim -= 1000;
//    period_us = sim;

    // need to add reset timer here and handle "inf" period as zero speed
    wheelSpeed.set_period_s(period_us / 1000 / 1000); //set period in seconds



    // Quick and dirty Temp file logging to capture some data
    // no error checking! Comment out when not being used
    QDateTime now = QDateTime::currentDateTime();
    QTextStream out(logfile);
    out << now.toString()
            << "," << wheelSpeed.ms()
            << "," << wheelSpeed.kmph()
            << "," << wheelSpeed.mss()
            << "," << wheelSpeed.hp() << "\n";

    //statusBar()->showMessage("Vespa LabJack: " + vespaLabJack->status);
    statusBar()->showMessage("Vespa LabJack: " + QString("%1").arg(wheelSpeed.kmph()*1000));
    ui->lcdNumber->display(QString("%1").arg(wheelSpeed.kmph(),0,'f',2));
    ui->svgSpeedometer->setValue(wheelSpeed.kmph()*1000);
}

SCIIMainWindow::~SCIIMainWindow()
{
    delete ui;
}

