#ifndef LABJACK_H
#define LABJACK_H
#include <stdio.h>
#include <windows.h>
#include <speed.h>
#include <QString>
#include <QElapsedTimer>
#include "LabJackUD.h" // Warning the repository contains a copy and will become outdated!!!
#include "LJUD_DynamicLinking.h"

class LabJack
{
public:
    LabJack();
    void LoadLabJackUD (void);
    void Configure(void);
    void Update(void);
    void TestExample (void);
    Speed wheelSpeed; // not the best place to have this - later split labjack and scooter coupling
                      // for now it is useful to define this class and use it directly
    QString status;
    QElapsedTimer timer; // use the system clock for time measurements
private:
    void ErrorHandler (LJ_ERROR lngErrorcode, long lngLineNumber, long lngIteration);
    //Define a variable to hold a handle to the loaded DLL.
    HINSTANCE hDLLInstance;
    double dblDriverVersion;
    LJ_ERROR lngErrorcode;
    LJ_HANDLE lngHandle;
    long lngIOType;
    long lngChannel;

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
};

#endif // LABJACK_H
