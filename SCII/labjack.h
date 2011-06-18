#ifndef LABJACK_H
#define LABJACK_H
#include <stdio.h>
#include <windows.h>
#include <QString>
#include "c:\program files\labjack\drivers\LabJackUD.h"
#include "..\DevC_LJUD\LJUD_DynamicLinking.h"

class LabJack
{
public:
    LabJack();
    void LoadLabJackUD (void);
    void Configure(void);
    void Update(void);
    void TestExample (void);
    double speedms;
    QString status;
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
