package testwireless;

import inet.common.LayeredProtocolBase;
import inet.linklayer.base.MACProtocolBase;


simple Mac extends MACProtocolBase
{

        parameters:
            string Address;
            //string SlotConfig;  // Assigned slot
            double SlotTime=default(2.1ms) @unit(s);
            //double ACKTimeout=default(12ms) @unit(s);
            int    MaxRetransmissions;
            int    numNodes;
            int    myId;
            int    totSlot;

            //string NodeConfig;
            string radioModule=default("^.radio");

            @class(Mac);
        gates:
}
