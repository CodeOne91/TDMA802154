package testwireless;

simple PeriodicApp
{
    parameters:
        double Period @unit(s);
        double StartTime @unit(s);
        int DataSize @unit(byte) = default(500B);
        string DestAddr;

        @display("i=block/source");

        @signal[E2E](record=vector);
        
    gates:
        input lowerLayerIn;
        output lowerLayerOut;
}
