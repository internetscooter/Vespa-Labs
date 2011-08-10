#ifndef LJSTREAM_H
#define LJSTREAM_H

#include "LabJackUD.h"
#include <list>
using namespace std;

class LJStream
{
public:
    LJStream();
    double scanData[];
    long *pScanData;
private:
    double scanRate_Hz;             // for streaming speed at samples per second
    //double channels;              // number of slots or channels of 16bit data
    double bufferSize;              // size of buffer for labjack driver to use
    list<int> channelList;          // List used for LJ_ioADD_STREAM_CHANNEL
    long mode;                      // LJ Stream wait mode


};

#endif // LJSTREAM_H
