#include "labjack.h"
#include <QObject>
#include <QMessageBox>
#include <QtDebug>
#include <QDateTime>
#include <qapplication.h>
#include <QLibrary>
#include <QWaitCondition>

#define U6 // this is how we choose the type of LabJack for the moment

LabJack::LabJack():
    lngHandle(0),
    scanRate_Hz(1000),
    totalTime_ms(0),
    scanNumber(0),
    pulseCount(0),
    ljInternalTemp(0),
    localWheelspeed(0,1.360), // Sava MC31 10inch
    status("Initialising...")
{
    // Load the DLL
    LoadLabJackUD();

#ifdef U6
    // Open the first found U6 LabJack - we are only expecting one (for now)
    Call (m_pOpenLabJack (LJ_dtU6, LJ_ctUSB, "1", 1, &lngHandle), __LINE__);
#else
    // Use this instead of the above if using a U3
    // Open the first found *U3* LabJack - we are only expecting one (for now)
    Call (m_pOpenLabJack (LJ_dtU3, LJ_ctUSB, "1", 1, &lngHandle), __LINE__);
#endif
    // Reset pin assignments are in the factory default condition.
    Call (m_pePut (lngHandle, LJ_ioPIN_CONFIGURATION_RESET, 0, 0, 0),__LINE__);

    status = "Ready for Configuration";
    timer.start(); // kick off the timing - probably not required for stream mode

    // Quick and dirty Temp file logging to capture some data
    // no error checking! Comment out when not being used
     logfile = new QFile( "vespa.log" );
     logfile->open(QIODevice::WriteOnly | QIODevice::Text);
}

// Configure the LabJack pins, clock, timers and stream
void LabJack::ConfigureStreamed(void)
{
    long lngGetNextIteration;
    long lngIOType=0, lngChannel=0;
    double dblValue=0;
    long ainResolution = 0;	//Configure resolution of the analog inputs (pass a non-zero value for quick sampling).
                                                    //See section 2.6 / 3.1 for more information.
    // Use the 48 MHz timer clock base with divider.
    Call(m_pAddRequest (lngHandle, LJ_ioPUT_CONFIG, LJ_chTIMER_CLOCK_BASE, LJ_tc48MHZ_DIV, 0, 0), __LINE__);

    // Set the divisor to 48 so the actual timer clock is 1 MHz.
    Call(m_pAddRequest (lngHandle, LJ_ioPUT_CONFIG, LJ_chTIMER_CLOCK_DIVISOR, 48, 0, 0), __LINE__);

    //Configure the desired resolution.  See section 2.6 / 3.1
    Call(m_pAddRequest (lngHandle, LJ_ioPUT_CONFIG, LJ_chAIN_RESOLUTION, ainResolution, 0, 0), __LINE__);

#ifdef U6
    // U6 Set the first timer/counter pin offset to FIO0 (others will automatically be set to FI00+)
    Call(m_pAddRequest (lngHandle,  LJ_ioPUT_CONFIG, LJ_chTIMER_COUNTER_PIN_OFFSET, 0, 0, 0), __LINE__);
#else
    // U3 Set the first timer/counter pin offset to FIO4 (others will automatically be set to FI04+)
     Call(m_pAddRequest (lngHandle,  LJ_ioPUT_CONFIG, LJ_chTIMER_COUNTER_PIN_OFFSET, 4, 0, 0), __LINE__);
#endif
    //Make sure Counter0 and 1 are disabled.
    Call(m_pAddRequest (lngHandle, LJ_ioPUT_COUNTER_ENABLE, 0, 0, 0, 0), __LINE__);
    Call(m_pAddRequest (lngHandle, LJ_ioPUT_COUNTER_ENABLE, 1, 0, 0, 0), __LINE__);

    // Enable 0, 1 or 2 timers.
    Call(m_pAddRequest  (lngHandle, LJ_ioPUT_CONFIG, LJ_chNUMBER_TIMERS_ENABLED, 1, 0, 0), __LINE__);

    // Enable timer 32-bit falling to falling edge measurement
    Call(m_pAddRequest  (lngHandle, LJ_ioPUT_TIMER_MODE, 0, LJ_tmFALLINGEDGES32, 0, 0), __LINE__);

    // Execute the requests.
    Call(m_pGoOne (lngHandle), __LINE__);

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

    status = "Configured";

    // Streaming Configuration...

    // Start with afresh
    Call(m_pAddRequest(lngHandle, LJ_ioCLEAR_STREAM_CHANNELS, 0, 0, 0, 0), __LINE__);

    // Set the scan rate.
    Call(m_pAddRequest(lngHandle, LJ_ioPUT_CONFIG, LJ_chSTREAM_SCAN_FREQUENCY, scanRate_Hz, 0, 0), __LINE__);

    // Give the driver a 5 second buffer (scanRate * 2 channels * 5 seconds).
    Call(m_pAddRequest(lngHandle, LJ_ioPUT_CONFIG, LJ_chSTREAM_BUFFER_SIZE, scanRate_Hz*2*5, 0, 0), __LINE__);

    // Configure reads to retrieve whatever data is available without waiting (wait mode LJ_swNONE).
    Call(m_pAddRequest(lngHandle, LJ_ioPUT_CONFIG, LJ_chSTREAM_WAIT_MODE, LJ_swNONE, 0, 0), __LINE__);
    // Call(m_pAddRequest(lngHandle, LJ_ioPUT_CONFIG, LJ_chSTREAM_WAIT_MODE, LJ_swSLEEP, 0, 0), __LINE__);

    // Add Timer0 200 LSW (low bit) to the scan
    // No reset... Call(m_pAddRequest(lngHandle, LJ_ioADD_STREAM_CHANNEL, 200, 0, 0, 0), __LINE__);
    // With reset...
    Call(m_pAddRequest(lngHandle, LJ_ioADD_STREAM_CHANNEL, 230, 0, 0, 0), __LINE__);

    //Add TC_Capture 224 MSW - (high bit for the above) to the scan
    Call(m_pAddRequest(lngHandle, LJ_ioADD_STREAM_CHANNEL, 224, 0, 0, 0), __LINE__);

    //Add Internal Temperature U6 Channel 14
    // doesn't work Call(m_pAddRequest(lngHandle, LJ_ioADD_STREAM_CHANNEL, 14, 0, 0, 0), __LINE__);

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
//    qDebug() << "Actual Scan Rate = " << dblValue;
//    qDebug() << "Actual Sample Rate = " << 2*dblValue;

    double period_us;

    // reset timer value
    Call(m_peGet (lngHandle, LJ_ioGET_TIMER, 0, &period_us, 0),__LINE__);

    status = "Streamed Configured";

}

void LabJack::ConfigurePolled(void)
{
    long lngGetNextIteration;
    long lngIOType=0, lngChannel=0;
    double dblValue=0;
    long ainResolution = 0;	//Configure resolution of the analog inputs (pass a non-zero value for quick sampling).
                                                    //See section 2.6 / 3.1 for more information.
    // Use the 48 MHz timer clock base with divider.
    Call(m_pAddRequest (lngHandle, LJ_ioPUT_CONFIG, LJ_chTIMER_CLOCK_BASE, LJ_tc48MHZ_DIV, 0, 0), __LINE__);

    // Set the divisor to 48 so the actual timer clock is 1 MHz.
    Call(m_pAddRequest (lngHandle, LJ_ioPUT_CONFIG, LJ_chTIMER_CLOCK_DIVISOR, 48, 0, 0), __LINE__);

    //Configure the desired resolution.  See section 2.6 / 3.1
    Call(m_pAddRequest (lngHandle, LJ_ioPUT_CONFIG, LJ_chAIN_RESOLUTION, ainResolution, 0, 0), __LINE__);

#ifdef U6
    // U6 Set the first timer/counter pin offset to FIO0 (others will automatically be set to FI00+)
    Call(m_pAddRequest (lngHandle,  LJ_ioPUT_CONFIG, LJ_chTIMER_COUNTER_PIN_OFFSET, 0, 0, 0), __LINE__);
#else
    // U3 Set the first timer/counter pin offset to FIO4 (others will automatically be set to FI04+)
     Call(m_pAddRequest (lngHandle,  LJ_ioPUT_CONFIG, LJ_chTIMER_COUNTER_PIN_OFFSET, 4, 0, 0), __LINE__);
#endif
    //Make sure Counter0 and 1 are disabled.
    Call(m_pAddRequest (lngHandle, LJ_ioPUT_COUNTER_ENABLE, 0, 0, 0, 0), __LINE__);
    Call(m_pAddRequest (lngHandle, LJ_ioPUT_COUNTER_ENABLE, 1, 0, 0, 0), __LINE__);

    // Enable 0, 1 or 2 timers.
    Call(m_pAddRequest  (lngHandle, LJ_ioPUT_CONFIG, LJ_chNUMBER_TIMERS_ENABLED, 1, 0, 0), __LINE__);

    // Enable timer 32-bit falling to falling edge measurement
    Call(m_pAddRequest  (lngHandle, LJ_ioPUT_TIMER_MODE, 0, LJ_tmFALLINGEDGES32, 0, 0), __LINE__);

    // Execute the requests.
    Call(m_pGoOne (lngHandle), __LINE__);

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

    status = "Configured Polled";

}

void LabJack::CreateTestPulse(int milliseconds)
{
    //Set FIO5 to output-high.
    //AddRequest (lngHandle, LJ_ioPUT_DIGITAL_BIT, 3, 1, 0, 0);
    Call(m_pePut (lngHandle, LJ_ioPUT_DIGITAL_BIT, 5, 1, 0),__LINE__);
    //Sleep(milliseconds);
    //Set FIO5 to output-low.
    Call(m_pePut (lngHandle, LJ_ioPUT_DIGITAL_BIT, 5, 0, 0),__LINE__);
    pulseCount++;
}

void LabJack::StreamUpdate(void)
{
    // qDebug() << "Scan?";
    // qDebug() << "Start:" << timer.elapsed();
    double dblValue=0;
    long k=0;
    double ms=0;
    double numScans = scanRate_Hz * 2;                              // the expected number rate x 2 channels
    double numScansRequested = numScans * 2;                    // we request twice as much as expected
    double scanData[(int)numScansRequested];// = {0};               // Max buffer size (#channels*numScansRequested)
    memset(scanData,0,numScansRequested*sizeof(double));            // dynamically create array size
    for (int i=0; i < numScansRequested; i++) scanData[i] = 0.0;    // initialize double array to zero
    // double scanData[4000] = {0};

    double dblCommBacklog=0;

    //Make a long parameter which holds the address of the data array.  We do this
    //so the compiler does not generate a warning in the eGet call that retrieves
    //stream data.  Note that the x1 parameter  in eGet (and AddRequest) is fairly
    //generic, in that sometimes it could just be a write parameter, and sometimes
    //it has the address of an array.  Since x1 is not declared as a pointer, the
    //compiler will complain if you just pass the array pointer without casting
    //it to a long as follows.
    long pScanData = (long)&scanData[0];

    //init array so we can easily tell if it has changed
    for(k=0;k<numScansRequested;k++)
    {
            scanData[k] = 0.0;
    }

    //Read the data.  We will request twice the number we expect, to
    //make sure we get everything that is available.
    //Note that the array we pass must be sized to hold enough SAMPLES, and
    //the Value we pass specifies the number of SCANS to read.
    //numScansRequested=numScans;
    Call(m_peGet(lngHandle, LJ_ioGET_STREAM_DATA, LJ_chALL_CHANNELS, &numScansRequested, pScanData),__LINE__);

    //The displays the number of scans that were actually read.
    //qDebug() << "Number read = " << numScansRequested;
    //This displays just the first scan.
    //qDebug() << "First scan = " << adblData[0] << "," << adblData[1] << "," << adblData[2] << "," << adblData[3];

    QDateTime now = QDateTime::currentDateTime();
    QTextStream out(logfile);

    for(k=0;k<numScans;k+=2)
    {
            //adblData[k] = 99999.0;
        if (scanData[k] > 0)
        {

            // Sanity check here...
            // The LabJack can throw some wild values at you, however here is pseudo code for a sanity check
            //
            // keep last sane period measurement
            // keep last_k
            // keep last_scanNumber
            //
            // if (k - last_k)/2 * scanRate ~= ms
            // use new value
            // else use old value
            //
            // since we are getting a value of time between last trigger and now, then logically the
            // time between slots in the scan should also roughly be the same.

            ms = ((scanData[k+1] * 65536) + scanData[k])/1000;
            //ms = scanData[k]/50;
            localWheelspeed.set_period_s(ms/1000);
            totalTime_ms += ms;
            // qDebug()
            out
                << "," << dblCommBacklog // should always be zero
                << "," << scanNumber // we are tracking each scan by number
                << "," << k // we are tracking each scan by number
                << "," << scanData[k+1]    //MSW
                << "," << scanData[k]    //LSW
                << "," << totalTime_ms/1000
                << "," << timer.elapsed()/1000
                << "," << (totalTime_ms/1000) - (timer.elapsed()/1000)
                << "," << localWheelspeed.kmph()
                << "," << localWheelspeed.rpm()
                << "," << localWheelspeed.hp()
                << "\n";

            //qDebug() << now.toString()<< "," << totalTime_ms << "," << ms << "," << scanData[k+1] << "," << scanData[k];
            //qDebug() << "V: " << scanData[k+1] << "," << scanData[k];
            // qDebug() << numScansRequested << "," << x << "," << pulseCount << "," << now.toString()<< "," << totalTime_ms << "," << ms << "," << scanData[k+1] << "," << scanData[k];
            //out << numScansRequested << "," << x << "," << pulseCount << "," << now.toString()<< "," << totalTime_ms << "," << ms << "," << scanData[k+1] << "," << scanData[k] << "\n";
        }
        status = QString("%1 km/h").arg(localWheelspeed.kmph(),0,'f',2);
    //qDebug() << numScansRequested << "," << x << "," << pulseCount << "," << now.toString()<< "," << totalTime_ms << "," << ms << "," << scanData[k+1] << "," << scanData[k];


    }
    //Retrieve the current backlog.  The UD driver retrieves stream data from
    //the LabJack in the background, but if the computer is too slow for some reason
    //the driver might not be able to read the data as fast as the LabJack is
    //acquiring it, and thus there will be data left over in the LabJack buffer.

    Call(m_peGet(lngHandle, LJ_ioGET_CONFIG, LJ_chSTREAM_BACKLOG_COMM, &dblCommBacklog, 0),__LINE__);
    // qDebug() << "Comm Backlog = " << dblCommBacklog;
    // qDebug() << "Stop:" << timer.elapsed();
    scanNumber++; // update scan reference number

    Call(m_peGet(lngHandle, LJ_ioSTOP_STREAM, 0, &dblValue, 0), __LINE__);
    // Get LabJack's Internal Temperature
    Call(m_peGet(lngHandle, LJ_ioGET_AIN, 14, &ljInternalTemp, 0),__LINE__);
    qDebug() << "Internal Temp = " << ljInternalTemp;

    Call(m_peGet(lngHandle, LJ_ioSTART_STREAM, 0, &dblValue, 0), __LINE__);
}

void LabJack::StreamStop(void)
{
    //Stop the stream
    Call(m_peGet(lngHandle, LJ_ioSTOP_STREAM, 0, 0, 0),__LINE__);
}

//This is the function used to dynamically load the DLL.
void LabJack::LoadLabJackUD (void)
{
    // Now try and load the DLL.
    // won't work for Linux unfortunately
    QLibrary ljsharedlib("labjackud");
    ljsharedlib.load();

    // If successfully loaded, get the address of the functions.
    if (ljsharedlib.isLoaded())
    {
        m_pListAll = (tListAll)ljsharedlib.resolve("ListAll");
        m_pOpenLabJack = (tOpenLabJack)ljsharedlib.resolve("OpenLabJack");
        m_pAddRequest = (tAddRequest)ljsharedlib.resolve("AddRequest");
        m_pGo = (tGo)ljsharedlib.resolve("Go");
        m_pGoOne = (tGoOne)ljsharedlib.resolve("GoOne");
        m_peGet = (teGet)ljsharedlib.resolve("eGet");
        m_pePut = (tePut)ljsharedlib.resolve("ePut");
        m_pGetResult = (tGetResult)ljsharedlib.resolve("GetResult");
        m_pGetFirstResult = (tGetFirstResult)ljsharedlib.resolve("GetFirstResult");
        m_pGetNextResult = (tGetNextResult)ljsharedlib.resolve("GetNextResult");
        m_peAIN = (teAIN)ljsharedlib.resolve("eAIN");
        m_peDAC = (teDAC)ljsharedlib.resolve("eDAC");
        m_peDI = (teDI)ljsharedlib.resolve("eDI");
        m_peDO = (teDO)ljsharedlib.resolve("eDO");
        m_peAddGoGet = (teAddGoGet)ljsharedlib.resolve("eAddGoGet");
        m_peTCConfig = (teTCConfig)ljsharedlib.resolve("eTCConfig");
        m_peTCValues = (teTCValues)ljsharedlib.resolve("eTCValues");
        m_pResetLabJack = (tResetLabJack)ljsharedlib.resolve("ResetLabJack");
        m_pDoubleToStringAddress = (tDoubleToStringAddress)ljsharedlib.resolve("DoubleToStringAddress");
        m_pStringToDoubleAddress = (tStringToDoubleAddress)ljsharedlib.resolve("StringToDoubleAddress");
        m_pStringToConstant = (tStringToConstant)ljsharedlib.resolve("StringToConstant");
        m_pErrorToString = (tErrorToString)ljsharedlib.resolve("ErrorToString");
        m_pGetDriverVersion = (tGetDriverVersion)ljsharedlib.resolve("GetDriverVersion");
        m_pTCVoltsToTemp = (tTCVoltsToTemp)ljsharedlib.resolve("TCVoltsToTemp");
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Load fail");
        msgBox.setInformativeText("Failed to load DLL!");
        msgBox.setStandardButtons(QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        msgBox.exec();
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
    }
}




