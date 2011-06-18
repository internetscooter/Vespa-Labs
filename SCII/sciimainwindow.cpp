#include "sciimainwindow.h"
#include "ui_sciimainwindow.h"



SCIIMainWindow::SCIIMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SCIIMainWindow)
{

    //vespaLabJack->TestExample();
    ui->setupUi(this);
    //statusBar()->show();
    statusBar()->showMessage("Locating Vespa LabJack...");
    vespaLabJack = new LabJack();
    statusBar()->showMessage("Vespa LabJack: " + vespaLabJack->status);
    vespaLabJack->Configure();
    statusBar()->showMessage("Vespa LabJack: " + vespaLabJack->status);
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(slotUpdate()));
    timer->start(500);
}

void SCIIMainWindow::slotUpdate(void)
{
    //statusBar()->showMessage("Reading Vespa LabJack...");
    //Sleep(500);
    vespaLabJack->Update();
    statusBar()->showMessage("Vespa LabJack: " + vespaLabJack->status);
    ui->lcdNumber->display(QString("%1").arg(vespaLabJack->speedms,0,'f',2));
}

SCIIMainWindow::~SCIIMainWindow()
{
    delete ui;
}

