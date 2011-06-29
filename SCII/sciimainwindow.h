#ifndef SCIIMAINWINDOW_H
#define SCIIMAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QFile>
#include "labjack.h"

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

public slots:

        virtual void slotUpdate();	// updates things screen with fresh information
};

#endif // SCIIMAINWINDOW_H
