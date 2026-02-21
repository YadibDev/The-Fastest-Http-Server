this part ensure the server runing coreclty

what is socket :
A socket is a low-level communication interface that enables a web server and a client (such as a web browser) to exchange data over a network. The server listens on a socket for incoming connections, and once a client connects, the socket becomes the channel used to receive requests and send responses. In simple terms, a socket acts as the communication doorway between the server and the outside world.
sf
i will design here this classes

clsServerSock:
here i will create all the socket that we will use in our server and each socket listen
to specific port and ip
and also it will have three or more method

isServerSocket:
like is the fd exist in the object
getServerSockets:
a simple getter
buildSockets:
this method build all sockets one by one
