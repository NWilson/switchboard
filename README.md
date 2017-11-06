# Switchboard
_A C++ experiment in internet-scale peer-to-peer communication_

Switchboard is a server-client application which will help me learn to use modern C++ for implementing message-passing systems.

It is an exploration in using modern C++ features to implement a large-scale low-latency system, able to handle many millions of concurrent clients by scaling the number of server nodes linearly to match the number of clients, using a fine-grained sharded architecture.
