#include "labjack.h"
#include <QObject>
#include <QMessageBox>
#include <QtDebug>

LabJack::LabJack():
    lngHandle(0),
    scanRate(100),
    status("Initialising...")
{
    // Load the DLL
    LoadLabJackUD();

    // Open the first found U3 LabJack - we only expecting one (for now)
    Call (m_pOpenLabJack (LJ_dtU3, LJ_ctUSB, "1", 1, &lngHandle), __LINE__);

    // Do a reset (just to be safe) and wait before trying anything else
    m_pResetLabJack(lngHandle);
    Sleep(5000);

    // Reset pin assignments are in the factory default condition.
    Call (m_pePut (lngHandle, LJ_ioPIN_CONFIGURATION_RESET, 0, 0, 0),__LINE__);

    status = "Ready for Configuration";
    timer.start(); // kick off the timing - probably not required for stream mode
}

// Configure the LabJack pins, clock and timers
void LabJack::Configure(void)
{
    // Use the 48 MHz timer clock base with divider.
    Call(m_pAddRequest  (lngHandle, LJ_ioPUT_CONFIG, LJ_chTIMER_CLOCK_BASE, LJ_tc48MHZ_DIV, 0, 0), __LINE__);

    // Set the divisor to 48 so the actual timer clock is 1 MHz.
    Call(m_pAddRequest  (lngHandle, LJ_ioPUT_CONFIG, LJ_chTIMER_CLOCK_DIVISOR, 48, 0, 0), __LINE__);

    // Set the timer/counter pin offset to FIO4, which will put the first
    Call(m_pAddRequest (lngHandle,  LJ_ioPUT_CONFIG, LJ_chTIMER_COUNTER_PIN_OFFSET, 4, 0, 0), __LINE__);

    // Enable both timers.// TODO - 1 or 2? this should probably only one until we need the other
    Call(m_pAddRequest  (lngHandle, LJ_ioPUT_CONFIG, LJ_chNUMBER_TIMERS_ENABLED, 1, 0, 0), __LINE__);

    // Enable timer 32-bit rising to rising edge measurement LJ_tmRISINGEDGES32
    Call(m_pAddRequest  (lngHandle, LJ_ioPUT_TIMER_MODE, 0, LJ_tmRISINGEDGES32, 0, 0), __LINE__);

    // Execute the requests.
    Call(m_pGoOne (lngHandle), __LINE__);
}

// trying out streamed mode for faster reads and more accuracy
void LabJack::ConfigureStreamed(void) // move to Configure() when this becomes "the" way rather than "a" way
{
    Configure(); //set up the inputs and clock

    long lngGetNextIteration;
    long lngIOType=0, lngChannel=0;
    double dblValue=0;

    // Set the scan rate.
    Call(m_pAddRequest(lngHandle, LJ_ioPUT_CONFIG, LJ_chSTREAM_SCAN_FREQUENCY, scanRate, 0, 0), __LINE__);

    // Give the driver a 5 second buffer (scanRate * 2 channels * 5 seconds).
    Call(m_pAddRequest(lngHandle, LJ_ioPUT_CONFIG, LJ_chSTREAM_BUFFER_SIZE, scanRate*2*5, 0, 0), __LINE__);

    // Configure reads to retrieve whatever data is available without waiting (wait mode LJ_swNONE).
    Call(m_pAddRequest(lngHandle, LJ_ioPUT_CONFIG, LJ_chSTREAM_WAIT_MODE, LJ_swNONE, 0, 0), __LINE__);

    //Add Timer0 200 LSW (low bit) to the scan
    //Call(m_pAddRequest(lngHandle, LJ_ioADD_STREAM_CHANNEL, 200, 0, 0, 0), __LINE__);
    //above with reset...
    Call(m_pAddRequest(lngHandle, LJ_ioADD_STREAM_CHANNEL, 230, 0, 0, 0), __LINE__);

    //Add TC_Capture 224 MSW - (high bit for the above) to the scan
    Call(m_pAddRequest(lngHandle, LJ_ioADD_STREAM_CHANNEL, 224, 0, 0, 0), __LINE__);

    //Execute the list of requests.
    Call(m_pGoOne(lngHandle), __LINE__);

    //Get all the results just to check for errors.
    lngErrorcode = m_pGetFirstResult(lngHandle, &lngIOType, &lngChannel, &dblValue, 0, 0);
    ErrorHandler(lngErrorcode, __LINE__, 0);
    lngGetNextIteration=0;	//Used by the error handling function.
    while(lngErrorcode < LJE_MIN_GROUP_ERROR)
    {
            lngErrorcode = m_pGetNextResult(lngHandle, &lngIOType, &lngChannel, &dblValue, 0, 0);
            if(lngErrorcode != LJE_NO_MORE_DATA_AVAILABLE)
            {
                    ErrorHandler(lngErrorcode, __LINE__, lngGetNextIteration);
            }
            lngGetNextIteration++;
    }

    //Start the stream.
    Call(m_peGet(lngHandle, LJ_ioSTART_STREAM, 0, &dblValue, 0), __LINE__);

    //The actual scan rate is dependent on how the desired scan rate divides into
    //the LabJack clock.  The actual scan rate is returned in the value parameter
    //from the start stream command.
    qDebug() << "Actual Scan Rate = " << dblValue;
    qDebug() << "Actual Sample Rate = " << 2*dblValue;
}

// get latest info
double LabJack::GetTimer0Value(void)
{
    //timer.start();
    double period_us;

    // get timer value
    Call(m_peGet (lngHandle, LJ_ioGET_TIMER, 0, &period_us, 0),__LINE__);
    status.setNum(period_us / 1000 / 1000); // show period in seconds
    return period_us;
}

void LabJack::StreamUpdate(void)
{
    long k=0;
    double numScans = 100;  //2x the expected some number
    double numScansRequested = numScans * 2;
    double adblData[200] = {0};  //Max buffer size (#channels*numScansRequested)
    double dblCommBacklog=0;

    //Make a long parameter which holds the address of the data array.  We do this
    //so the compiler does not generate a warning in the eGet call that retrieves
    //stream data.  Note that the x1 parameter  in eGet (and AddRequest) is fairly
    //generic, in that sometimes it could just be a write parameter, and sometimes
    //it has the address of an array.  Since x1 is not declared as a pointer, the
    //compiler will complain if you just pass the array pointer without casting
    //it to a long as follows.
    long padblData = (long)&adblData[0];

    //init array so we can easily tell if it has changed
    for(k=0;k<numScans*2;k++)
    {
            adblData[k] = 99999.0;
    }

    //Read the data.  We will request twice the number we expect, to
    //make sure we get everything that is available.
    //Note that the array we pass must be sized to hold enough SAMPLES, and
    //the Value we pass specifies the number of SCANS to read.
    numScansRequested=numScans;
    Call(m_peGet(lngHandle, LJ_ioGET_STREAM_DATA, LJ_chALL_CHANNELS, &numScansRequested, padblData),__LINE__);

    //The displays the number of scans that were actually read.
    //qDebug() << "Number read = " << numScansRequested;
    //This displays just the first scan.
    //qDebug() << "First scan = " << adblData[0] << "," << adblData[1] << "," << adblData[2] << "," << adblData[3];

    for(k=0;k<50;k+=2)
    {
            //adblData[k] = 99999.0;
        if (adblData[k] > 0)
        qDebug() << "V: " << adblData[k+1] << "," << adblData[k];
    }
    //Retrieve the current backlog.  The UD driver retrieves stream data from
    //the U3 in the background, but if the computer is too slow for some reason
    //the driver might not be able to read the data as fast as the U3 is
    //acquiring it, and thus there will be data left over in the U3 buffer.
    Call(m_peGet(lngHandle, LJ_ioGET_CONFIG, LJ_chSTREAM_BACKLOG_COMM, &dblCommBacklog, 0),__LINE__);
    //qDebug() << "Comm Backlog = " << dblCommBacklog;
}

void LabJack::StreamStop(void)
{
    //Stop the stream
    Call(m_peGet(lngHandle, LJ_ioSTOP_STREAM, 0, 0, 0),__LINE__);
}

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
}

// This is a wrapper to ErrorHandler so we can inline calls and reduce clutter
void LabJack::Call (LJ_ERROR lngErrorcode, long lngLineNumber)
{
    ErrorHandler (lngErrorcode, lngLineNumber, 0);
}

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

//********** junk code below here - just for notes **********

void LabJack::StreamTest(void) //remove later - just for reference
{
    //LJ_ERROR lngErrorcode;
    //LJ_HANDLE lngHandle=0;
    long lngGetNextIteration;
    long i=0,k=0;
    long lngIOType=0, lngChannel=0;
    double dblValue=0, dblCommBacklog=0;
    double scanRate = 1000;
    long delayms = 1000;
    double numScans = 2000;  //2x the expected # of scans (2*scanRate*delayms/1000)
    double numScansRequested;
    double adblData[4000] = {0};  //Max buffer size (#channels*numScansRequested)


    //Make a long parameter which holds the address of the data array.  We do this
    //so the compiler does not generate a warning in the eGet call that retrieves
    //stream data.  Note that the x1 parameter  in eGet (and AddRequest) is fairly
    //generic, in that sometimes it could just be a write parameter, and sometimes
    //it has the address of an array.  Since x1 is not declared as a pointer, the
    //compiler will complain if you just pass the array pointer without casting
    //it to a long as follows.
    long padblData = (long)&adblData[0];

    //Read and display the UD version.
    dblValue = m_pGetDriverVersion();
    printf("UD Driver Version = %.3f\n\n",dblValue);

    //Open the first found LabJack U3.
    Call(m_pOpenLabJack (LJ_dtU3, LJ_ctUSB, "1", 1, &lngHandle),__LINE__);

    //Read and display the hardware version of this U3.
    Call(m_peGet (lngHandle, LJ_ioGET_CONFIG, LJ_chHARDWARE_VERSION, &dblValue, 0),__LINE__);
    printf("U3 Hardware Version = %.3f\n\n",dblValue);

    //Read and display the firmware version of this U3.
    Call(m_peGet (lngHandle, LJ_ioGET_CONFIG, LJ_chFIRMWARE_VERSION, &dblValue, 0),__LINE__);
    printf("U3 Firmware Version = %.3f\n\n",dblValue);

    //Start by using the pin_configuration_reset IOType so that all
    //pin assignments are in the factory default condition.
    Call(m_pePut (lngHandle, LJ_ioPIN_CONFIGURATION_RESET, 0, 0, 0),__LINE__);

    //Configure FIO0 and FIO1 as analog, all else as digital.  That means we
    //will start from channel 0 and update all 16 flexible bits.  We will
    //pass a value of b0000000000000011 or d3.
    Call(m_pePut (lngHandle, LJ_ioPUT_ANALOG_ENABLE_PORT, 0, 3, 16),__LINE__);

    //Configure the stream:
    //Set the scan rate.
    Call(m_pAddRequest(lngHandle, LJ_ioPUT_CONFIG, LJ_chSTREAM_SCAN_FREQUENCY, scanRate, 0, 0),__LINE__);

    //Give the driver a 5 second buffer (scanRate * 2 channels * 5 seconds).
    Call(m_pAddRequest(lngHandle, LJ_ioPUT_CONFIG, LJ_chSTREAM_BUFFER_SIZE, scanRate*2*5, 0, 0),__LINE__);

    //Configure reads to retrieve whatever data is available without waiting (wait mode LJ_swNONE).
        //See comments below to change this program to use LJ_swSLEEP mode.
    Call(m_pAddRequest(lngHandle, LJ_ioPUT_CONFIG, LJ_chSTREAM_WAIT_MODE, LJ_swNONE, 0, 0),__LINE__);

    //Define the scan list as AIN0 then AIN1.
    Call(m_pAddRequest(lngHandle, LJ_ioCLEAR_STREAM_CHANNELS, 0, 0, 0, 0),__LINE__);
    Call(m_pAddRequest(lngHandle, LJ_ioADD_STREAM_CHANNEL, 0, 0, 0, 0),__LINE__); // first method for single ended reading - AIN0
    Call(m_pAddRequest(lngHandle, LJ_ioADD_STREAM_CHANNEL_DIFF, 1, 0, 32, 0),__LINE__); // second method for single ended reading - AIN1

    //Execute the list of requests.
    Call(m_pGoOne(lngHandle),__LINE__);

    //Get all the results just to check for errors.
    lngErrorcode = m_pGetFirstResult(lngHandle, &lngIOType, &lngChannel, &dblValue, 0, 0);
    ErrorHandler(lngErrorcode, __LINE__, 0);
    lngGetNextIteration=0;	//Used by the error handling function.
    while(lngErrorcode < LJE_MIN_GROUP_ERROR)
    {
            lngErrorcode = m_pGetNextResult(lngHandle, &lngIOType, &lngChannel, &dblValue, 0, 0);
            if(lngErrorcode != LJE_NO_MORE_DATA_AVAILABLE)
            {
                    ErrorHandler(lngErrorcode, __LINE__, lngGetNextIteration);
            }

            lngGetNextIteration++;
    }

    //Start the stream.
    Call(m_peGet(lngHandle, LJ_ioSTART_STREAM, 0, &dblValue, 0),__LINE__);

    //The actual scan rate is dependent on how the desired scan rate divides into
    //the LabJack clock.  The actual scan rate is returned in the value parameter
    //from the start stream command.
    printf("Actual Scan Rate = %.3f\n",dblValue);
    printf("Actual Sample Rate = %.3f\n",2*dblValue);


    //Read data
    while(1)	//Loop will run until any key is hit
    {
            //Since we are using wait mode LJ_swNONE, we will wait a little, then
            //read however much data is available.  Thus this delay will control how
            //fast the program loops and how much data is read each loop.  An
            //alternative common method is to use wait mode LJ_swSLEEP where the
            //stream read waits for a certain number of scans.  In such a case
            //you would not have a delay here, since the stream read will actually
            //control how fast the program loops.
            //
            //To change this program to use sleep mode,
            //	-change numScans to the actual number of scans desired per read,
            //	-change wait mode addrequest value to LJ_swSLEEP,
            //	-comment out the following Sleep command.

            Sleep(delayms);	//Remove if using LJ_swSLEEP.

            //init array so we can easily tell if it has changed
            for(k=0;k<numScans*2;k++)
            {
                    adblData[k] = 99999.0;
            }

            //Read the data.  We will request twice the number we expect, to
            //make sure we get everything that is available.
            //Note that the array we pass must be sized to hold enough SAMPLES, and
            //the Value we pass specifies the number of SCANS to read.
            numScansRequested=numScans;
            Call(m_peGet(lngHandle, LJ_ioGET_STREAM_DATA, LJ_chALL_CHANNELS, &numScansRequested, padblData),__LINE__);
            //The displays the number of scans that were actually read.
            printf("\nIteration # %d\n",i);
            printf("Number read = %.0f\n",numScansRequested);
            //This displays just the first scan.
            printf("First scan = %.3f, %.3f\n",adblData[0],adblData[1]);
            ErrorHandler(lngErrorcode, __LINE__, 0);
            //Retrieve the current backlog.  The UD driver retrieves stream data from
            //the U3 in the background, but if the computer is too slow for some reason
            //the driver might not be able to read the data as fast as the U3 is
            //acquiring it, and thus there will be data left over in the U3 buffer.
            Call(m_peGet(lngHandle, LJ_ioGET_CONFIG, LJ_chSTREAM_BACKLOG_COMM, &dblCommBacklog, 0),__LINE__);
            printf("Comm Backlog = %.0f\n",dblCommBacklog);
            i++;
    }
}

// from previous code (useful examples) - remove later
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
