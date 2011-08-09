#include "labjackthread.h"
#include <QDebug>
#include <QDateTime>

LabjackThread::LabjackThread(QObject *parent) :
    QThread(parent)
{
}

void LabjackThread::run(void)
{
    while(true)
    {
        qDebug() << "thread";
        sleep(10);
        //Sleep(1000);
    }
}

