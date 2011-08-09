#ifndef LABJACKTHREAD_H
#define LABJACKTHREAD_H

#include <QThread>
#include "labjack.h"
#include <speed.h>
#include <QTimer>

class LabjackThread : public QThread
{
    Q_OBJECT
public:
    explicit LabjackThread(QObject *parent = 0);
    QString status;
protected:
    void run();


private:
    LabJack *vespaLabJack;
    QFile *logfile;
    Speed wheelSpeed; // not the best place to have this - later make Vespa class
    QTimer *readTimer;


signals:

public slots:
    virtual void slotStreamUpdate(void);
    virtual void stop(void);
    virtual void slotPulseGen(void);

};

#endif // LABJACKTHREAD_H
