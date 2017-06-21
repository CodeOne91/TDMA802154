#ifndef __TESTWIRELESS_PERIODICAPP_H_
#define __TESTWIRELESS_PERIODICAPP_H_

#include <omnetpp.h>
#include "inet/linklayer/common/MACAddress.h"

using namespace omnetpp;

using namespace inet;

class PeriodicApp : public cSimpleModule 
{
  protected:
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

    virtual void transmitFrame();
    virtual MACAddress resolveMACAddress(const char *addr);

    unsigned int dataSize;
    simtime_t period;
    simtime_t startTime;
    MACAddress destAddr;
};

#endif
