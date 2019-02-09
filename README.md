# Chatz
Chatz is a simple client/server chat room written in C. This wouldn't have been possible at all without [Beej's Guid to Network Programming](https://beej.us/guide/bgnet/). I tried to do this without this resource for a solid three weeks and I wasn't getting anywhere. This book has shown me so much, not just about network programming, but also error checking and writing better C code. 

## Chatz-Client
Chatz-Client is the client program that enables a user to connect to Chatz-Server, the server program. I implemented threading in this program, mainly just to see how threding works and get a little familiar with it.
This program takes two command line arguments:
  1. Remote IP Address to connect to.
  2. The port that the server is running on.

## Chatz-Server
Chatz-Server is the server program that can be hosted on a machine.
The server acts as a hub for the clients by forwarding each message it recieves on to every client.
This program takes one command line argument:
  1. Port to bind the server to.
  
### Why?
I wanted to learn more about Network programming in C. This project is a precursor to a bigger project that I would like to work on. Eventually I would like to write my own little Command and Control server and a RAT that connects back the server and allows me to run commands on the machine that the RAT is running on.

### Things I tried right away that failed.
*All of my struggles had to do with the server program*
  * I initially tried storing the socket file descriptors in an array. This proved to be really hard to work with and made my code disgusting.
  * Right away I was trying to spin each new client off on their own thread that would be able to listen for incoming data for that one client. This proved to also be really hard to keep track of not only the sockets, but also the different threads. It also made my code break a lot.
  * I tried to implement a linked list that kept track of the clients. This served to be a better solution than the previous but I still wasn't getting the results I wanted.
  
### Things I learned
  * Using the function `select()` to monitor sockets is way eaiser than creating a thread for each new client :)
  * I learned how to properly create sockets and connect sockets.
  * I learned how to manage connections.
  * I learned more about error checking and making sure my code doesn't blow up.
  * I learned that it is alright to fail over and over again. All that matters is that you stick with it.
  * I have gained a new appreciation for the C programming language.
