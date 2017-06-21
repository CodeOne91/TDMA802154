packet EthernetFrame {
    string DestAddr;
    string SrcAddr;
    short EtherType;
    byteLength = 24;
}

packet DataPacket {
    simtime_t genTime;
    //simtime_t deadline;
    int n;
}
