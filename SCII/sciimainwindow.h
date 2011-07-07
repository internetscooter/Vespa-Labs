#ifndef SCIIMAINWINDOW_H
#define SCIIMAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QFile>
#include "labjack.h"
#include <speed.h>

namespace Ui {
    class SCIIMainWindow;
}

class SCIIMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SCIIMainWindow(QWidget *parent = 0);
    ~SCIIMainWindow();

private:
    Ui::SCIIMainWindow *ui;
    LabJack *vespaLabJack;
    QFile *logfile;
    Speed wheelSpeed; // not the best place to have this - later make Vespa class
    double sim;

public slots:

        virtual void slotUpdate();	// updates things screen with fresh information
};

#endif // SCIIMAINWINDOW_H
