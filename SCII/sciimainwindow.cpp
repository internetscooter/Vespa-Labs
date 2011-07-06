#include "sciimainwindow.h"
#include "ui_sciimainwindow.h"
#include <QTextStream>
#include <QDateTime>
#include <iostream>
#include <QElapsedTimer>
#include <QtDebug>

SCIIMainWindow::SCIIMainWindow(QWidget *parent) :
    QMainWindow(parent),
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

    //update screen every 0.5 seconds (as a test)
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(slotUpdate()));
    timer->start(100);
    //ui->svgSpeedometer->setValue(10000);
}

void SCIIMainWindow::slotUpdate(void)
{
    QElapsedTimer timer;
    timer.start();
    //statusBar()->showMessage("Reading Vespa LabJack...");
    // Sleep(500);
    vespaLabJack->Update();

    // Quick and dirty Temp file logging to capture some data
    // no error checking! Comment out when not being used
    QDateTime now = QDateTime::currentDateTime();
    QTextStream out(logfile);
    out << now.toString()
            << "," << vespaLabJack->wheelSpeed.ms()
            << "," << vespaLabJack->wheelSpeed.kmph()
            << "," << vespaLabJack->wheelSpeed.mss()
            << "," << vespaLabJack->wheelSpeed.hp() << "\n";

    //statusBar()->showMessage("Vespa LabJack: " + vespaLabJack->status);
    statusBar()->showMessage("Vespa LabJack: " + QString("%1").arg(vespaLabJack->wheelSpeed.kmph()*1000));
    ui->lcdNumber->display(QString("%1").arg(vespaLabJack->wheelSpeed.kmph(),0,'f',2));
    ui->svgSpeedometer->setValue(vespaLabJack->wheelSpeed.kmph()*1000);
    qDebug() << "slotUpdate operation took" << timer.elapsed() << "milliseconds";;
}

SCIIMainWindow::~SCIIMainWindow()
{
    delete ui;
}

