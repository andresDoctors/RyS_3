#ifndef SINK
#define SINK

#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Sink : public cSimpleModule {
private:
    cStdDev delayStats;
    cOutVector delayVector;
    short expectedAck;
    cStdDev cargaOfrecida;
    cPacket *pkt;
public:
    Sink();
    virtual ~Sink();
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Sink);

Sink::Sink() {
    pkt = NULL;
}

Sink::~Sink() {
    //cancelAndDelete(pkt);
}

void Sink::initialize(){
    // stats and vector names
    delayStats.setName("TotalDelay");
    delayVector.setName("Delay");
    expectedAck = 0;
}

void Sink::finish(){
    // stats record at the end of simulation
    recordScalar("Avg delay", delayStats.getMean());
    recordScalar("Number of packets", delayStats.getCount());
    recordScalar("cargaOfrecida", (float)delayStats.getCount()/(float)200);
}

void Sink::handleMessage(cMessage * msg) {

    if(msg->getKind() == expectedAck) {

        // compute queuing delay
        simtime_t delay = simTime() - msg->getCreationTime();
        // update stats
        delayStats.collect(delay);
        delayVector.record(delay);

        // create new packet
        pkt = new cPacket("confirm");
        pkt->setByteLength(1);
        pkt->setKind(expectedAck);
        // send to the output
        send(pkt, "out");

        expectedAck = !expectedAck;
    }

    else {
        pkt = new cPacket("confirm");
        pkt->setByteLength(1);
        pkt->setKind(!expectedAck);
        // send to the output
        send(pkt, "out");
    }

    delete(msg);
}

#endif /* SINK */
