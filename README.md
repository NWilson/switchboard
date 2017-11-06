# Switchboard
_A C++ experiment in internet-scale peer-to-peer communication_

Switchboard is a server-client application which will enable peers to communicate, without requiring each peer to have a public IP address.

It is an exploration in using modern C++ features to implement a large-scale low-latency system, able to handle many millions of concurrent clients by scaling the number of server nodes linearly to match the number of clients, using a fine-grained sharded architecture.

Security shall be based around API keys, allowing each user access to a private namespace of peers which can communicate.

## Grandiose goals

1. Implement a C++ server which listens for HTTP requests and passes messages to locally-rendezvoused peers (first demo stage)
   1. Implement a networking library using C++20 coroutines, with lock-free concurrent message queueing
   1. Add the Actor pattern, and a timer scheduler
   1. Add an HTTP server
   1. Add a JSON parser
   1. Add a service layer and implement two methods, send-message and receive-messages
1. Add mesh message-passing capability (diameter 1)
   Enables clients to send messages to peers rendezvoused on another node. Uses distributed hash ring.
1. Add security
   1. Implement coroutine-based TLS (using BearTLS or mbedTLS)
   1. Add authentication to message-passing routines
   1. Add TLS offload to low-priority worker threads
   1. Add OCSP stapling protocol support
   1. Add OCSP stapling server support
1. Implement SOCKS clients
   The goal is to enable the following use-case:
    * Run `switchboard-client-listen LONDON` on the machine to be connected-to
    * Run `ssh -o ProxyCommand="switchboard-client-connect %h" LONDON.switchboard` to connect via Switchboard, where `switchboard-client-connect` is a local SOCKS server that enables any of your applications (such as `ssh`) to connect to any of your other Switchboard-listening machines.
    Work:
    1. Implement SOCKS server
    1. Add port-forwarding protocol
    1. Add application-level TCP layer for reliability
1. Add publish-subscribe pattern
   This adds new primitives, set-status and receive-status for dynamic multicast messages, such as node status or other streaming information.
1. Add mesh message-passing capability (diameter 2)
   Enables scale-out to >1000 server nodes.
