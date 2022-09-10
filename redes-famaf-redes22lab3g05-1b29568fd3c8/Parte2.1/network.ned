simple Generator
{
    parameters:
        volatile int packetByteSize;
        volatile double generationInterval; // sec
        @display("i=block/source");
    gates:
        input in;
        output out;
}

simple Queue
{
    parameters:
        volatile int bufferSize;
        volatile double serviceTime; // sec
        @display("i=block/queue;q=buffer");
    gates:
        input in;
        output out;
}

simple Sink
{
    parameters:
        @display("i=block/sink");
    gates:
        input in;
        output out;
}

module NodeTx{
    parameters:
        @display("i=block/routing");
    gates:
    	input in;
        output out;
    submodules:
        gen: Generator{
            parameters:
                @display("p=75,50");
        }
        queue: Queue{
            parameters:
                @display("p=75,190");
        }
    connections:
    	in --> gen.in;
        gen.out --> queue.in;
        queue.out --> out;
}

module NodeRx{
    parameters:
        @display("i=block/routing");
    gates:
        input in;
        output out;
    submodules:
        sink: Sink{
            parameters:
                @display("p=75,50");
        }
        queue: Queue{
            parameters:
                @display("p=75,190");
        }
    connections:
        in --> queue.in;
        //case study 1:
        //queue.out --> { datarate = 0.5Mbps; } --> sink.in;
        //case study 2:
        queue.out --> { datarate = 1Mbps; } --> sink.in;
        sink.out --> out;     
}

simple UnreliableQueue
{
    parameters:
        volatile int bufferSize;
        volatile double serviceTime; // sec
        @display("i=block/queue;q=buffer");
    gates:
        input in;
        output out;
}

network Network
{
    @display("bgl=2");
    submodules:
        nodeTx: NodeTx {
            @display("p=30,30");
        }
        queue: UnreliableQueue {
            @display("p=130,30");
        }
        confirmQueue: UnreliableQueue {
        	@display("p=130,130");
        }
        nodeRx: NodeRx {
            @display("p=230,30");
        }
    connections:
        nodeTx.out --> {  datarate = 1Mbps; delay = 100us; } --> queue.in;
        //case study 1
        //queue.out --> {  datarate = 1Mbps; delay = 100us; } --> nodeRx.in;
        //case study 2
        queue.out --> { datarate = 0.5Mbps; delay = 100us; } --> nodeRx.in;

        nodeRx.out --> {  datarate = 1Mbps; delay = 100us; } --> confirmQueue.in;
        confirmQueue.out --> {  datarate = 1Mbps; delay = 100us; } --> nodeTx.in;
}