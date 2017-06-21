
package testwireless;

import inet.linklayer.contract.IWirelessNic;
import inet.physicallayer.contract.packetlevel.IRadio;

module Nic like IWirelessNic
{
    parameters:
        string interfaceTableModule;
        string radioType = default("Ieee802154NarrowbandScalarRadio");
        *.interfaceTableModule = default(absPath(interfaceTableModule));
        @display("i=block/ifcard");
    gates:
        input upperLayerIn;
        output upperLayerOut;
        input radioIn @labels(IRadioFrame);
    submodules:
        mac: Mac {
            parameters:
                @display("p=100,150");
        }
        radio: <radioType> like IRadio {
            parameters:
                @display("p=100,250");
        }
    connections:
        upperLayerIn --> mac.upperLayerIn;
        mac.lowerLayerOut --> radio.upperLayerIn;
        mac.upperLayerOut --> { @display("m=n"); } --> upperLayerOut;
        radioIn --> { @display("m=s"); } --> radio.radioIn;
        radio.upperLayerOut --> mac.lowerLayerIn;
}

