#!/bin/sh
cd simulations
../src/TestWireless -m -n .:../src:../../../../../../Downloads/omnetpp-5.1.1/samples/inet/src:../../../../../../Downloads/omnetpp-5.1.1/samples/inet/examples:../../../../../../Downloads/omnetpp-5.1.1/samples/inet/tutorials:../../../../../../Downloads/omnetpp-5.1.1/samples/inet/showcases --image-path=../../../../../../Downloads/omnetpp-5.1.1/samples/inet/images -l ../../../../../../Downloads/omnetpp-5.1.1/samples/inet/src/INET omnetpp.ini
