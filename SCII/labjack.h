#ifndef LABJACK_H
#define LABJACK_H
#include <stdio.h>
#include <QString>
#include <QElapsedTimer>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include "LabJackUD.h"
#include "LJUD_DynamicLinking.h"
#include "speed.h"

class LabJack
{

public:
    LabJack();
    void LoadLabJackUD (void);
    void Configure(void);
    void StreamTest(void);
    void StreamStop(void);
    void CreateTestPulse(int milliseconds = 1);
    QString status;
    QElapsedTimer timer; // use the system clock for time measurements
    Speed localWheelspeed;
    // more debug info
    double scanNumber;

private:
    void ErrorHandler (LJ_ERROR lngErrorcode, long lngLineNumber, long lngIteration);
    void Call (LJ_ERROR lngErrorcode, long lngLineNumber); //same as above for inline error checking of calls
    //Define a variable to hold a handle to the loaded DLL.
    //HINSTANCE hDLLInstance;
    double dblDriverVersion;
    LJ_ERROR lngErrorcode;
    LJ_HANDLE lngHandle;
    long lngIOType;
    long lngChannel;
    double scanRate_Hz; // for streaming speed at samples per second
    double totalTime_ms;
    double pulseCount;
    QTimer *pulseTimer;
    QTimer *readTimer;
    QFile *logfile;
    QTextStream out;

    //Define variables for the UD functions.
    tListAll m_pListAll;
    tOpenLabJack m_pOpenLabJack;
    tAddRequest m_pAddRequest;
    tGo m_pGo;
    tGoOne m_pGoOne;
    teGet m_peGet;
    tePut m_pePut;
    tGetResult m_pGetResult;
    tGetFirstResult m_pGetFirstResult;
    tGetNextResult m_pGetNextResult;
    teAIN m_peAIN;
    teDAC m_peDAC;
    teDI m_peDI;
    teDO m_peDO;
    teAddGoGet m_peAddGoGet;
    teTCConfig m_peTCConfig;
    teTCValues m_peTCValues;
    tResetLabJack m_pResetLabJack;
    tDoubleToStringAddress m_pDoubleToStringAddress;
    tStringToDoubleAddress m_pStringToDoubleAddress;
    tStringToConstant m_pStringToConstant;
    tErrorToString m_pErrorToString;
    tGetDriverVersion m_pGetDriverVersion;
    tTCVoltsToTemp m_pTCVoltsToTemp;

public slots:
    virtual void StreamUpdate(void);	// updates things screen with fresh information
    //virtual void slotPulseGen();  // generates a pulse to simulate a reed switch trigger
};

#endif // LABJACK_H
