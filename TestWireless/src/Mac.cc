#include <cassert>

#include "inet/common/INETUtils.h"
#include "inet/common/INETMath.h"
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/common/InterfaceEntry.h"
#include "inet/linklayer/contract/IMACProtocolControlInfo.h"
#include "inet/common/FindModule.h"
#include "inet/linklayer/common/SimpleLinkLayerControlInfo.h"
#include "inet/linklayer/csma/CSMAFrame_m.h"

#include "Mac.h"
#include "Frames_m.h"
#include "TDMAFrame_m.h"
#include "cAckDelay.h"
#include <string.h>

Define_Module(Mac);

using namespace inet;
using namespace physicallayer;
using namespace std;

void Mac::initialize(int stage)
{
  MACProtocolBase::initialize(stage);
  if (stage == INITSTAGE_LOCAL)
  {
    slotTime   = par("SlotTime");
    address    = par("Address").stringValue();


    totSlot    = par("totSlot");
    SlotIndex  = 0;
    ackCnt     = 0;
    pktSent    = 0;
    ackRcvd    = 0;
    maxRetransmissions = par("MaxRetransmissions");
    ACKTimeout = slotTime * totSlot; //par("ACKTimeout");
    myId       = par("myId");
    numNodes   = par("numNodes");
    string s = "";
    for (int i = 0; i <= (int)(totSlot / numNodes); i++)
    {
    	int newSlot = myId + i * numNodes;
    	if (newSlot <= totSlot)
    		s = s + " " + std::to_string(newSlot);
    }
    slotConfig = cStringTokenizer(s.c_str(), " ").asIntVector();
    packetLoss.setName("PacketLoss");
    packetsSent.setName("Sent packets");
    acksReceivd.setName("Received Acks");
    e2eDelayStat.setName("End to end delay");
    cMessage *txTimer = new cMessage("TDMATimer");
    cModule *radioModule = getModuleFromPar<cModule>(par("radioModule"), this);
    radioModule -> subscribe(IRadio::radioModeChangedSignal, this);
    radioModule -> subscribe(IRadio::transmissionStateChangedSignal, this);
    radio = check_and_cast<IRadio *>(radioModule);

    scheduleAt(0, txTimer);

  }
  else if (stage == INITSTAGE_LINK_LAYER)
  {
    radio -> setRadioMode(IRadio::RADIO_MODE_RECEIVER);
  }
}

void Mac::transmitFrame()
{
  radio -> setRadioMode(IRadio:: RADIO_MODE_TRANSMITTER);
  // Priority to Retransmission
  TDMAFrame* tframe;
  if (retrQueue.getLength() > 0)
  {
	  // Resending packets
	  tframe =  dynamic_cast<TDMAFrame*>(retrQueue.pop());
	  EV << "Retransmission " << tframe->getAckId() << " for " << tframe->getDestAddr() << endl;
  }
  else
  {
	  // Sending data packets
	  tframe =  dynamic_cast<TDMAFrame*>(txQueue.pop());
  }
  EV << "SIZE: " << tframe->getBitLength() << endl;
  cMessage *ackTimeoutMessage = new cMessage("ACKTimeout");
  ackTimeoutMessage -> addObject(tframe -> dup());

  toAck.add(new cNamedObject(std::to_string(tframe->getAckId()).c_str()));
  scheduleAt(simTime() + (ACKTimeout), ackTimeoutMessage);

  sendDown(tframe);
}

void Mac::handleSelfMessage(cMessage *msg)
{

  if(strcmp(msg -> getName(), "TDMATimer") == 0)
  {
    if(std::find(slotConfig.begin(), slotConfig.end(), SlotIndex)!= slotConfig.end())
    {
      EV<<"TRANSMITTING (" << SlotIndex <<  ")" << endl;
      if(!txQueue.isEmpty() || !retrQueue.isEmpty())
        transmitFrame();
      else
        EV << "EMPTY QUEUES" << endl;
    }

    	scheduleAt(simTime() + slotTime, msg);
    	SlotIndex = (SlotIndex + 1) % totSlot;
    if (pktSent > 0)
    	packetLoss.record((1 - ((float)ackRcvd / pktSent)) * 100);
    return;
  }
  else if (strcmp(msg -> getName(), "ACKTimeout") == 0)
  {
      EV << "[ACK TIMEOUT]: verifying if is needed a packet re-transmission" << endl;
      TDMAFrame* tframe = dynamic_cast<TDMAFrame*>(msg -> getParList().remove(0));
      char ack[16] = "";
      sprintf(ack, "%d", tframe -> getAckId());
      tframe -> setRetransmissionCnt (tframe -> getRetransmissionCnt() + 1);
      if (acksList.remove(ack) == nullptr) // The ack was not provided
      {
    	if (tframe -> getRetransmissionCnt() < maxRetransmissions)
    	{
    		if (toAck.get(ack) != nullptr)
    		{
				retrQueue.insert(dynamic_cast<cPacket*>(tframe));
				EV << "[ACK TIMEOUT] ACK for " << ack <<
				   " not received. Re-transmission" << endl;
    		}
    		else
    		{
    			EV << "[ACK TIMEOUT] ACK for " << ack << " not found into toAck list. Ignoring." << endl;
    		}

    	} else
    	{
        	EV << "[ACK TIMEOUT] ACK for " << ack <<
        	  " not received. MaxRetransmission limit reached. Denying retransmission." << endl;
        	toAck.remove(ack);
    	}
      } else
      {
          EV << "[ACK TIMEOUT] ACK for " << ack << " received. Re-transmission not needed" << endl;
          toAck.remove(ack);
      }
      return;
  }
  error("Unknown (self) message!");
}


void Mac::handleUpperPacket(cPacket *pkt){
  // Create the TDMA Frame
  TDMAFrame* tframe = new TDMAFrame("TDFrame");
  // Set a random destination address
  int destId = rand() % numNodes;
  if (destId == myId)
	  destId = (destId + 1) % numNodes; // next node if random destId is the same as this node's id

  tframe -> setDestAddr(((string)"node" + std::to_string(destId)).c_str());
  tframe -> setSrcAddr(address);
  tframe -> setAckId(ackCnt++);
  tframe -> setIsAck(false);
  tframe -> setRetransmissionCnt(0);
  pktSent++;
  packetsSent.record(pktSent);
  e2eDelay.add(new cAckDelay( std::to_string(tframe->getAckId()).c_str(),simTime()));
  tframe -> encapsulate(pkt);
  // Insert the TDMA Frame in the queue for sending
  txQueue.insert(tframe);
}

void Mac::handleLowerPacket(cPacket *pkt){
  EV << "LOWER-LAYER PACKET RECEIVED" << endl;
  TDMAFrame* tframe = dynamic_cast<TDMAFrame*>(pkt);
  if(strcmp(tframe -> getDestAddr(),this->address) == 0)
  {
      EV << "LOWER-LAYER PACKET " << tframe -> getAckId() << "FOR " << this -> address << endl;
      if (!(tframe -> getIsAck()))
      {
        // Data packet
        cPacket *p     = dynamic_cast<DataPacket*>(tframe -> decapsulate());

        // Create the Ack to send
        radio -> setRadioMode(IRadio:: RADIO_MODE_TRANSMITTER);
        TDMAFrame* ack = new TDMAFrame();
        ack -> setDestAddr(tframe -> getSrcAddr());
        ack -> setSrcAddr(this -> address);
        ack -> setIsAck(true);
        ack -> setAckId(tframe -> getAckId());
        EV << "[ACK] Sending ACK " << ack -> getAckId() << " for " << ack->getDestAddr() << " " << ack->getBitLength() << endl;
        // Ack to lower layer
        sendDown(ack);
        // Data packet to upper-layer
        sendUp(p);
      }
      else 
      {
        // Ack packet
        char ack[16] = "";
        sprintf(ack, "%d", tframe -> getAckId());
        acksList.add(new cNamedObject(ack));
        ackRcvd++;
        acksReceivd.record(ackRcvd);
        EV << "[ACK] Received ACK for " << ack << endl;
        cAckDelay* t = dynamic_cast<cAckDelay*>(e2eDelay.remove(ack));
        if (t != nullptr)
        {
        	t -> setArrival(simTime());
        	e2eDelayStat.record(t -> getE2EDelay());
        }
      }
  }
  delete tframe;
  return;
}

InterfaceEntry *Mac::createInterfaceEntry()
{
  InterfaceEntry *e = new InterfaceEntry(this);
  e -> setMulticast(false);
  e -> setBroadcast(true);
  return e;
}


// Inspired to CSMA
void Mac::receiveSignal(cComponent *source, simsignal_t signalID, long value, cObject* obj) 
{
  Enter_Method_Silent();
  if (signalID == IRadio::transmissionStateChangedSignal) 
  {
    IRadio::TransmissionState newRadioTransmissionState = (IRadio::TransmissionState) value;
    if(newRadioTransmissionState == IRadio::TRANSMISSION_STATE_IDLE && state==STATE_TX)
    {
      this -> state = STATE_RX;
      radio -> setRadioMode(IRadio::RADIO_MODE_RECEIVER);
    }
    if(newRadioTransmissionState == IRadio::TRANSMISSION_STATE_TRANSMITTING && state==STATE_RX)
    {
      this -> state=STATE_TX;
    }
  }
}

void Mac::finish()
{
	EV << "Data report for " << this -> address << endl;
    EV << "Sent: " << pktSent << endl;
    EV << "Acks: " << ackRcvd << endl;
    recordScalar("#sent", pktSent);
    recordScalar("#acks", ackRcvd);
}
