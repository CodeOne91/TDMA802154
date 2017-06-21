package testwireless;

import inet.linklayer.ieee802154.Ieee802154NarrowbandNic;
import inet.mobility.contract.IMobility;
import inet.linklayer.contract.IWirelessNic;
import inet.networklayer.common.InterfaceTable;


module Node
{
    parameters:
        @networkNode;
        string mobilityType = default("StationaryMobility");
        string nicType = default("Nic");
        *.mobilityModule = default(mobilityType != "" ? absPath(".mobility") : "");
        *.interfaceTableModule = default(absPath(".interfaceTable"));

       // @statistic[Throughput](record=vector);
       // @statistic[Workload](record=vector);
       @statistic[E2E](record=vector);
       // @statistic[DMR](record=vector);
       // @statistic[DM](record=vector);
        @display("bgb=317,280");
    gates:
        input channelIn @loose;

    submodules:
        interfaceTable: InterfaceTable {
            @display("p=55,107");
        }
        nic: <nicType> like IWirelessNic {
            @display("p=166,162");
            radio.transmitter.preambleDuration = 192us;
            radio.bandwidth = 2.8MHz;
        }
        app: PeriodicApp {
            @display("p=166,32");
        }
        mobility: <mobilityType> like IMobility {
            @display("p=63,32");
        }
    connections:
        channelIn --> nic.radioIn;
        app.lowerLayerOut --> nic.upperLayerIn;
        nic.upperLayerOut --> app.lowerLayerIn;
}
