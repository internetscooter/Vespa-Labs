#include "labjackthread.h"
#include <QDebug>
#include <QDateTime>
#include <QMetaObject>

// http://thesmithfam.org/blog/2010/02/07/talking-to-qt-threads/

LabjackThread::LabjackThread(QObject *parent) :
    QThread(parent)
{
    vespaLabJack = new LabJack();
}

void LabjackThread::run(void)
{

    vespaLabJack->ConfigureStreamed();

    //update screen every x seconds
    QTimer *timer = new QTimer();
    connect(timer, SIGNAL(timeout()),this, SLOT(slotStreamUpdate()));
    timer->start(1000);

    QTimer *pulseTimer = new QTimer();
    connect(pulseTimer, SIGNAL(timeout()), this, SLOT(slotPulseGen()));
    //pulseTimer->setSingleShot(true);
    pulseTimer->start(250);

    exec(); // starts the event loop, and doesn't
            // return until it is told to quit()

}

void LabjackThread::slotStreamUpdate(void)
{
    vespaLabJack->StreamUpdate();
    //qDebug() << "thread";
}

void LabjackThread::slotPulseGen(void)
{
     vespaLabJack->CreateTestPulse(1);
    //sim = sim - 10;
        //pulseTimer->setSingleShot(true);
    //pulseTimer->start(sim);

}
void LabjackThread::stop()
{
    if(currentThread() != this)
    {
        // The caller is running in a
        // different thread, so use Qt to
        // call stop() later, on our own thread:
        QMetaObject::invokeMethod(this, "stop",
                        Qt::QueuedConnection);
    }
    else
    {
        // Now the call has arrived from our
        // own thread, yay! We can safely
        // shut down our event loop.
        quit();
    }
}
