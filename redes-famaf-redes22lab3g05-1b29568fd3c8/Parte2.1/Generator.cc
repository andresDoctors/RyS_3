#ifndef GENERATOR
#define GENERATOR

#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Generator : public cSimpleModule {
private:
    cMessage *sendMsgEvent;
    cMessage *resendMsgEvent;
    cStdDev transmissionStats;
    cStdDev totalPacketSend;
    cStdDev cargaUtil;
    simtime_t departureTime;
    simtime_t waitTime;
    short expectedAck;
    cPacket *pkt;
public:
    Generator();
    virtual ~Generator();
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};
Define_Module(Generator);

Generator::Generator() {
    sendMsgEvent = NULL;
    resendMsgEvent = NULL;
    pkt = NULL;
}

Generator::~Generator() {
    cancelAndDelete(sendMsgEvent);
    cancelAndDelete(resendMsgEvent);
}

void Generator::initialize() {
    transmissionStats.setName("TotalTransmissions");
    totalPacketSend.setName("PackagesSent");
    // create the send packet
    sendMsgEvent = new cMessage("sendEvent");
    // schedule the first event at random time
    scheduleAt(par("generationInterval"), sendMsgEvent);

    waitTime = simtime_t(1);
    expectedAck = 0;
    resendMsgEvent = new cMessage("resendEvent");

}

void Generator::finish() {
    recordScalar("Number of packets sent", totalPacketSend.getCount());
    recordScalar("carga util", (float)totalPacketSend.getCount()/(float)200);
}

void Generator::handleMessage(cMessage *msg) {

    if (msg == sendMsgEvent) {
        // create new packet
        pkt = new cPacket("packet");
        pkt->setByteLength(par("packetByteSize"));
        pkt->setKind(expectedAck);

        // send to the output
        send(pkt->dup(), "out");
        totalPacketSend.collect(1);

        // compute the new departure time
        departureTime = simTime() + par("generationInterval");
        scheduleAt(simTime() + waitTime, resendMsgEvent);
    }

    else if (msg == resendMsgEvent) {
		send(pkt->dup(), "out");
		scheduleAt(simTime() + 2 * waitTime, resendMsgEvent);
    }

	else if (msg->getKind() == expectedAck) {

		delete(msg);
		delete(pkt);

		cancelEvent(resendMsgEvent);
        expectedAck = !expectedAck;

        if (simTime() > departureTime)
            handleMessage(sendMsgEvent);
        else
            scheduleAt(departureTime, sendMsgEvent);
	}
}

#endif /* GENERATOR */
