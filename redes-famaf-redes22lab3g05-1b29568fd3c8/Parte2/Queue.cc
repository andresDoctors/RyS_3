#ifndef QUEUE
#define QUEUE

#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Queue: public cSimpleModule {
private:
    cQueue buffer;
    cMessage *endServiceEvent;
    simtime_t serviceTime;
    simtime_t packetServiceTime;
    cStdDev countOfPackets;
    cStdDev packetsDroped;
    cStdDev packetsSended;
    cOutVector bufferSizeVector;
    cOutVector packetDropVector;
    cOutVector packetSendVector;
public:
    Queue();
    virtual ~Queue();
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Queue);

Queue::Queue() {
    endServiceEvent = NULL;
}

Queue::~Queue() {
    cancelAndDelete(endServiceEvent);
}

void Queue::initialize() {
    buffer.setName("buffer");
    countOfPackets.setName("totalPackets");
    bufferSizeVector.setName("bufferSize");
    packetDropVector.setName("packetDrop");
    packetsDroped.setName("totalPacketsDroped");
    endServiceEvent = new cMessage("endService");
    packetSendVector.setName("packetsSend");
}

void Queue::finish() {
    recordScalar("Number of packets collected", countOfPackets.getCount());
    recordScalar("Number of packets droped", packetDropVector.getValuesReceived());
    recordScalar("number of packets sended",packetSendVector.getValuesReceived());

}
void Queue::handleMessage(cMessage *msg) {

    // if msg is signaling an endServiceEvent
    if (msg == endServiceEvent) {
        // if packet in buffer, send next one
        if (!buffer.isEmpty()) {
            // dequeue packet
            cPacket *pkt = (cPacket*) buffer.pop();
            // send packet
            send(pkt, "out");
            packetSendVector.record(packetSendVector.getValuesReceived());
            // start new service
            serviceTime = pkt->getDuration();
            scheduleAt(simTime() + serviceTime, endServiceEvent);
        }
    } else { // if msg is a data packet
        countOfPackets.collect(1);
        if(buffer.getLength() >= par("bufferSize").intValue()){
            // drop the packet
            delete msg;
            this->bubble("packet dropped");
            packetDropVector.record(packetDropVector.getValuesReceived());
        } else {
            // enqueue the packet
            buffer.insert(msg);
            bufferSizeVector.record(buffer.getLength());
            // if the server is idle
            if (!endServiceEvent->isScheduled()) {
                // start the service
                scheduleAt(simTime(), endServiceEvent);
            }
        }
    }
}

#endif /* QUEUE */
