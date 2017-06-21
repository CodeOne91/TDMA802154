package testwireless.simulations;

import inet.linklayer.ieee80211.mac.Ieee80211Mac;
import inet.node.inet.WirelessHost;
import inet.physicallayer.ieee802154.packetlevel.Ieee802154NarrowbandScalarRadioMedium;
import inet.physicallayer.propagation.ConstantTimePropagation;
import testwireless.Node;


network NetworkTDMA
{
    @display("bgb=500,500");
    int numNodes;
	node[*].nic.mac.numNodes = numNodes;
    types:
    submodules:
        radioMedium: Ieee802154NarrowbandScalarRadioMedium {
            @display("p=57.120003,21.76");
        }
        node[numNodes]: Node {
            @display("p=157,172,r,50");
        }
	
    connections allowunconnected:
}
