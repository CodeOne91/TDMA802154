#ifndef __TESTWIRELESS_MAC_H_
#define __TESTWIRELESS_MAC_H_

#include <omnetpp.h>

#include "inet/physicallayer/contract/packetlevel/IRadio.h"
#include "inet/linklayer/contract/IMACProtocol.h"
#include "inet/linklayer/common/MACAddress.h"
#include "inet/linklayer/base/MACProtocolBase.h"
#include "inet/linklayer/csma/CSMAFrame_m.h"
#include "Frames_m.h"

using namespace omnetpp;
using namespace inet;
using namespace physicallayer;

class Mac : public  MACProtocolBase{
  protected:

    typedef enum
    {
      STATE_TX,
      STATE_RX,
    } Macstates;

    virtual void initialize(int stage);

    /** @brief Handle messages from lower layer */
    virtual void handleLowerPacket(cPacket *) override;

    /** @brief Handle messages from upper layer */
    virtual void handleUpperPacket(cPacket *) override;

    /** @brief Handle self messages such as timers */
    virtual void handleSelfMessage(cMessage *) override;

    virtual InterfaceEntry *createInterfaceEntry() override;

    virtual void receiveSignal(cComponent *source, simsignal_t signalID, long value, cObject* obj) override;

    virtual void finish() override;

    const char *address;
    simtime_t tPeriod;
    simtime_t slotTime;
    simtime_t ACKTimeout;
    cPacketQueue txQueue;
    cPacketQueue retrQueue;
    int totSlot;
    int SlotIndex;



private:
    void transmitFrame();
    int ackCnt;
    int maxRetransmissions;
    int numNodes;
    int myId;
    cArray acksList;

    Macstates state;
    IRadio *radio;
    //std::vector<std::string> nodeConfig;
    std::vector<int> slotConfig;
    long pktSent;
    long ackRcvd;
    cArray e2eDelay;
    cArray toAck;
    cOutVector e2eDelayStat;
    cOutVector packetLoss;
    cOutVector packetsSent;
    cOutVector acksReceivd;
};

#endif
