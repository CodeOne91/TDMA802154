#include "PeriodicApp.h"
#include "Frames_m.h"
#include "inet/linklayer/common/Ieee802Ctrl.h"
#include "inet/networklayer/contract/IInterfaceTable.h"

Define_Module(PeriodicApp);

using namespace inet;

void PeriodicApp::initialize(int stage) 
{
  if(stage == INITSTAGE_LOCAL)
  {
    period    = par("Period");
    startTime = par("StartTime");
    dataSize  = par("DataSize");

  }
  else if(stage == INITSTAGE_APPLICATION_LAYER) 
  {
    const char *dAddr = par("DestAddr").stringValue();
    destAddr = resolveMACAddress(dAddr);
    if(startTime.dbl() >= 0) 
    {
      cMessage *msg = new cMessage("StartTimer");
      scheduleAt(startTime, msg);
    }
  }
}

void PeriodicApp::handleMessage(cMessage *msg) 
{
  if(msg->isSelfMessage()) 
  {
    if(strcmp(msg->getName(), "StartTimer") == 0) 
    {
      transmitFrame();
      scheduleAt(simTime() + period, msg);
      return;
    }
    error("Unknown self-message.");
  }

  DataPacket *pkt = dynamic_cast<DataPacket *>(msg);

  simsignal_t sig = registerSignal("E2E");

  sig = registerSignal("E2E");
  emit(sig, simTime() - pkt -> getGenTime());
  EV << "AppLayer Size" << dataSize << endl;
}

void PeriodicApp::transmitFrame() 
{
  IMACProtocolControlInfo *ctrl = new Ieee802Ctrl();
  ctrl->setDestinationAddress(destAddr);

  DataPacket *pkt = new DataPacket();
  pkt->setByteLength(dataSize);
  pkt->setGenTime(simTime());
  pkt->setN(rand() % 10);

  send(pkt,"lowerLayerOut");
}

MACAddress PeriodicApp::resolveMACAddress(const char *addr) 
{
  cTopology topo("topo");
  topo.extractByProperty("networkNode");
  for (int i = 0; i < topo.getNumNodes(); i++) 
  {
    cModule *dhost = topo.getNode(i) -> getModule();
    if(strcmp(dhost->getFullName(), addr) == 0)
    {
      IInterfaceTable *it = dynamic_cast<IInterfaceTable *>(dhost->getSubmodule("interfaceTable"));
      return (it->getInterface(0))->getMacAddress();
    }
  }
  return MACAddress::UNSPECIFIED_ADDRESS;
}
