#include "labjack.h"
#include <QObject>
#include <QMessageBox>

LabJack::LabJack():
    lngHandle(0),
    status("Initialising..."),
    wheelSpeed(0,1.345), // Diameter of a Sava MC18 10inch
    lngIOType(0), // may not be needed
    lngChannel(0) // may not be needed
{
    // Load the DLL
    LoadLabJackUD();

    // Open the first found U3 LabJack - we only expecting one (for now)
    lngErrorcode = m_pOpenLabJack (LJ_dtU3, LJ_ctUSB, "1", 1, &lngHandle);
    ErrorHandler(lngErrorcode, __LINE__, 0);

    status = "Ready";
}

// Configure the LabJack pins and timers - generic to start with later we'll have different ones
// This is still a work in progress...
// I'd actually like all these set via XML or Qt preferences so they can be updated live...
void LabJack::Configure(void)
{
    // Start by using the pin_configuration_reset IOType so that all
    // pin assignments are in the factory default condition.
    // The ePut function is used, which combines the add/go/get.
    lngErrorcode = m_pePut (lngHandle, LJ_ioPIN_CONFIGURATION_RESET, 0, 0, 0);
    ErrorHandler(lngErrorcode, __LINE__, 0);

    // The following commands will use the add-go-get method to group
    // multiple requests into a single low-level function (get is used later in LabJack::Update(void))

    // Add - Use the 48 MHz timer clock base with divider.  Since we are using clock with divisor
    // support, Counter0 is not available.
    lngErrorcode = m_pAddRequest  (lngHandle, LJ_ioPUT_CONFIG, LJ_chTIMER_CLOCK_BASE, LJ_tc48MHZ_DIV, 0, 0);
    ErrorHandler(lngErrorcode, __LINE__, 0);

    // Add - Set the divisor to 48 so the actual timer clock is 1 MHz.
    lngErrorcode = m_pAddRequest  (lngHandle, LJ_ioPUT_CONFIG, LJ_chTIMER_CLOCK_DIVISOR, 48, 0, 0);
    ErrorHandler(lngErrorcode, __LINE__, 0);

    // Add - Set the timer/counter pin offset to 4, which will put the first
    // timer/counter on FIO4.
    lngErrorcode = m_pAddRequest (lngHandle,  LJ_ioPUT_CONFIG, LJ_chTIMER_COUNTER_PIN_OFFSET, 4, 0, 0);
    ErrorHandler(lngErrorcode, __LINE__, 0);

    // Add - Enable both timers. .
    lngErrorcode = m_pAddRequest  (lngHandle, LJ_ioPUT_CONFIG, LJ_chNUMBER_TIMERS_ENABLED, 2, 0, 0);
    ErrorHandler(lngErrorcode, __LINE__, 0);

    // Add - Enable timer 32-bit rising to rising edge measurement LJ_tmRISINGEDGES32
    lngErrorcode = m_pAddRequest  (lngHandle, LJ_ioPUT_TIMER_MODE, 0, LJ_tmRISINGEDGES32, 0, 0);
    ErrorHandler(lngErrorcode, __LINE__, 0);

    // Go!!! - Execute the requests.
    lngErrorcode = m_pGoOne (lngHandle);
    ErrorHandler(lngErrorcode, __LINE__, 0);

}

// get latest info
void LabJack::Update(void)
{
    double period_us;

    // get timer value
    lngErrorcode = m_peGet (lngHandle, LJ_ioGET_TIMER, 0, &period_us, 0);
    ErrorHandler(lngErrorcode, __LINE__, 0);

    status.setNum(period_us / 1000 / 1000);
    wheelSpeed.set_period(period_us / 1000 / 1000);
    //speedms = dblValue / 1000 / 1000; //seconds
}

// from previous code (useful examples)
void LabJack::TestExample(void)
{
    long lngGetNextIteration;
    double dblValue=0;
    double Value2=9999,Value3=9999;
    double ValueDIBit=9999,ValueDIPort=9999,ValueCounter=9999;

    char ch;

    //First some configuration commands.  These will be done with the ePut
    //function which combines the add/go/get into a single call.

    //Configure FIO2 and FIO3 as analog, all else as digital.  That means we
    //will start from channel 0 and update all 16 flexible bits.  We will
    //pass a value of b0000000000001100 or d12.
    lngErrorcode = m_pePut (lngHandle, LJ_ioPUT_ANALOG_ENABLE_PORT, 0, 12, 16);
    ErrorHandler(lngErrorcode, __LINE__, 0);

    //Set the timer/counter pin offset to 7, which will put the first
    //timer/counter on FIO7.
    lngErrorcode = m_pePut (lngHandle,  LJ_ioPUT_CONFIG, LJ_chTIMER_COUNTER_PIN_OFFSET, 7, 0);
    ErrorHandler(lngErrorcode, __LINE__, 0);

    //Enable Counter1 (FIO7).
    lngErrorcode = m_pePut (lngHandle,  LJ_ioPUT_COUNTER_ENABLE, 1, 1, 0);
    ErrorHandler(lngErrorcode, __LINE__, 0);


    //The following commands will use the add-go-get method to group
    //multiple requests into a single low-level function.
    //Request AIN2 and AIN3.
    lngErrorcode = m_pAddRequest (lngHandle, LJ_ioGET_AIN, 2, 0, 0, 0);
    ErrorHandler(lngErrorcode, __LINE__, 0);

    lngErrorcode = m_pAddRequest (lngHandle, LJ_ioGET_AIN, 3, 0, 0, 0);
    ErrorHandler(lngErrorcode, __LINE__, 0);

    //Set DAC0 to 2.5 volts.
    lngErrorcode = m_pAddRequest (lngHandle, LJ_ioPUT_DAC, 0, 2.5, 0, 0);
    ErrorHandler(lngErrorcode, __LINE__, 0);

    //Read digital input FIO0.
    lngErrorcode = m_pAddRequest (lngHandle, LJ_ioGET_DIGITAL_BIT, 0, 0, 0, 0);
    ErrorHandler(lngErrorcode, __LINE__, 0);

    //Set digital output FIO1 to output-high.
    lngErrorcode = m_pAddRequest (lngHandle, LJ_ioPUT_DIGITAL_BIT, 1, 1, 0, 0);
    ErrorHandler(lngErrorcode, __LINE__, 0);

    //Read digital inputs FIO4 through FIO6.
    lngErrorcode = m_pAddRequest (lngHandle, LJ_ioGET_DIGITAL_PORT, 4, 0, 3, 0);
    ErrorHandler(lngErrorcode, __LINE__, 0);

    //Request the value of Counter1.
    lngErrorcode = m_pAddRequest (lngHandle, LJ_ioGET_COUNTER, 1, 0, 0, 0);
    ErrorHandler(lngErrorcode, __LINE__, 0);


    while (1)
    {
       //Execute the requests.
            lngErrorcode = m_pGoOne (lngHandle);
            ErrorHandler(lngErrorcode, __LINE__, 0);

            //Get all the results.  The input measurement results are stored.  All other
            //results are for configuration or output requests so we are just checking
            //whether there was an error.
            lngErrorcode = m_pGetFirstResult(lngHandle, &lngIOType, &lngChannel, &dblValue, 0, 0);
            ErrorHandler(lngErrorcode, __LINE__, 0);

            lngGetNextIteration=0;	//Used by the error handling function.

            while(lngErrorcode < LJE_MIN_GROUP_ERROR)
            {
                    switch(lngIOType)
                    {

                    case LJ_ioGET_AIN :
                            switch(lngChannel)
                            {
                                    case 2:
                                            Value2=dblValue;
                                            break;
                                    case 3:
                                            Value3=dblValue;
                                            break;
                            }
                            break;

                    case LJ_ioGET_DIGITAL_BIT :
                            ValueDIBit=dblValue;
                            break;

                    case LJ_ioGET_DIGITAL_PORT :
                            ValueDIPort=dblValue;
                            break;

                    case LJ_ioGET_COUNTER :
                            ValueCounter=dblValue;
                            break;

                    }

                    lngErrorcode = m_pGetNextResult(lngHandle, &lngIOType, &lngChannel, &dblValue, 0, 0);
                    if(lngErrorcode != LJE_NO_MORE_DATA_AVAILABLE)
                    {
                            ErrorHandler(lngErrorcode, __LINE__, lngGetNextIteration);
                    }

                    lngGetNextIteration++;

            }

            printf("AIN2 = %f\n",Value2);
            printf("AIN3 = %f\n",Value3);
            printf("FIO0 = %f\n",ValueDIBit);
            printf("FIO4-FIO6 = %f\n",ValueDIPort);  //Will read 7 if all 3 lines are pulled-high as normal.
            printf("Counter1 (FIO7) = %f\n",ValueCounter);

            printf("\nPress Enter to go again or (q) to quit\n");
            ch = getchar();
            if (ch == 'q') exit(0);
    }
}

//Start of LoadLabJackUD function.
//This is the function used to dynamically load the DLL.
void LabJack::LoadLabJackUD (void)
{
    //Now try and load the DLL.
    if (hDLLInstance = LoadLibrary(L"labjackud.dll"))
    {
        //If successfully loaded, get the address of the functions.
        m_pListAll = (tListAll)::GetProcAddress(hDLLInstance,"ListAll");
        m_pOpenLabJack = (tOpenLabJack)::GetProcAddress(hDLLInstance,"OpenLabJack");
        m_pAddRequest = (tAddRequest)::GetProcAddress(hDLLInstance,"AddRequest");
        m_pGo = (tGo)::GetProcAddress(hDLLInstance,"Go");
        m_pGoOne = (tGoOne)::GetProcAddress(hDLLInstance,"GoOne");
        m_peGet = (teGet)::GetProcAddress(hDLLInstance,"eGet");
        m_pePut = (tePut)::GetProcAddress(hDLLInstance,"ePut");
        m_pGetResult = (tGetResult)::GetProcAddress(hDLLInstance,"GetResult");
        m_pGetFirstResult = (tGetFirstResult)::GetProcAddress(hDLLInstance,"GetFirstResult");
        m_pGetNextResult = (tGetNextResult)::GetProcAddress(hDLLInstance,"GetNextResult");
        m_peAIN = (teAIN)::GetProcAddress(hDLLInstance,"eAIN");
        m_peDAC = (teDAC)::GetProcAddress(hDLLInstance,"eDAC");
        m_peDI = (teDI)::GetProcAddress(hDLLInstance,"eDI");
        m_peDO = (teDO)::GetProcAddress(hDLLInstance,"eDO");
        m_peAddGoGet = (teAddGoGet)::GetProcAddress(hDLLInstance,"eAddGoGet");
        m_peTCConfig = (teTCConfig)::GetProcAddress(hDLLInstance,"eTCConfig");
        m_peTCValues = (teTCValues)::GetProcAddress(hDLLInstance,"eTCValues");
        m_pResetLabJack = (tResetLabJack)::GetProcAddress(hDLLInstance,"ResetLabJack");
        m_pDoubleToStringAddress = (tDoubleToStringAddress)::GetProcAddress(hDLLInstance,"DoubleToStringAddress");
        m_pStringToDoubleAddress = (tStringToDoubleAddress)::GetProcAddress(hDLLInstance,"StringToDoubleAddress");
        m_pStringToConstant = (tStringToConstant)::GetProcAddress(hDLLInstance,"StringToConstant");
        m_pErrorToString = (tErrorToString)::GetProcAddress(hDLLInstance,"ErrorToString");
        m_pGetDriverVersion = (tGetDriverVersion)::GetProcAddress(hDLLInstance,"GetDriverVersion");
        m_pTCVoltsToTemp = (tTCVoltsToTemp)::GetProcAddress(hDLLInstance,"TCVoltsToTemp");
    }
    else
    {
        printf("\nFailed to load DLL\n");
        getchar();
        exit(0);
    }
    //Read the UD version.
    dblDriverVersion = m_pGetDriverVersion();

     // m_pOpenLabJack now holds a pointer to the OpenLabJack function.  The compiler
     // automatically recognizes m_pOpenLabJack as a pointer to a function and
     // calls the function with the parameters given.  If we created another
     // variable of type tOpenLabJack and simply put "pNewVar = m_pOpenLabJack",
     // then the compiler might not know to call the function.
}
//End of LoadLabJackUD function.


//This is our simple error handling function that is called after every UD
//function call.  This function displays the errorcode and string description
//of the error.  It also has a line number input that can be used with the
//macro __LINE__ to display the line number in source code that called the
//error handler.  It also has an iteration input is useful when processing
//results in a loop (getfirst/getnext).
void LabJack::ErrorHandler (LJ_ERROR lngErrorcode, long lngLineNumber, long lngIteration)
{
        char err[255];

        if (lngErrorcode != LJE_NOERROR)
        {
                m_pErrorToString(lngErrorcode,err);
                QString errorCode;
                errorCode.setNum(lngErrorcode);
                errorCode.append(" ");
                errorCode.append(err);
                errorCode.append(" ");
                errorCode.append(__FILE__);
                errorCode.append(" @ Line:" + QString().setNum(lngLineNumber));

                QMessageBox msgBox;
                msgBox.setText("LabJack Error!");
                msgBox.setInformativeText(errorCode);
                msgBox.setStandardButtons(QMessageBox::Cancel);
                msgBox.setDefaultButton(QMessageBox::Cancel);
                msgBox.exec();
                exit(0);

//                printf("Error number = %d\n",lngErrorcode);
//                printf("Error string = %s\n",err);
//                printf("Source line number = %d\n",lngLineNumber);
//                printf("Iteration = %d\n\n",lngIteration);
//                if(lngErrorcode > LJE_MIN_GROUP_ERROR)
//                {
//                        //Quit if this is a group error.
//                        getchar();
//                        exit(0);
//                }
   }
}
