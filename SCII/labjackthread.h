#ifndef LABJACKTHREAD_H
#define LABJACKTHREAD_H

#include <QThread>

class LabjackThread : public QThread
{
    Q_OBJECT
public:
    explicit LabjackThread(QObject *parent = 0);

protected:
    void run();

signals:

public slots:

};

#endif // LABJACKTHREAD_H
