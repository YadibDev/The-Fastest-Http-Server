i used many ressource to understand what is http and how the web server should behave , and i will make a list bellow of the ressources that i used :
  youtube:
    http
    https://youtu.be/wW2A5SZ3GkI?si=M8wnSkrGNyc8SBNA
    https://youtu.be/pi4gaFDBtMc?si=Qyxv1WCeoCibg40b
    
    nginx
    https://youtu.be/I6dpN0geIb4?si=-gGnzCrGz5hLEsxN
    how to write config file nginx
    https://youtu.be/9t9Mp0BGnyI?si=nov_Neiw22bRQrxT
    what is a webserver
    https://youtu.be/9J1nJOivdyw?si=XBVs4UPLhFE4TkJf
    how socket work
    https://youtu.be/-utm73RxNo4?si=dEDMeKdnUFgHwSKu
  articles:
    what is socket
    https://geeksforgeeks.org/computer-networks/socket-in-computer-network/
    what is http
    https://www.cloudflare.com/learning/ddos/glossary/hypertext-transfer-protocol-http/
    what is epoll:
    https://medium.com/@m-ibrahim.research/mastering-epoll-the-engine-behind-high-performance-linux-networking-85a15e6bde90

and also i use ai to deep dive in the concept of socket and why we need epoll and pool to manage multiple client etc ...


what is socket 
socket is an endpoint to make two devices connect and exchange data

to create a socket you must use some system call:

how to use socket system call:

// Create a non-blocking TCP socket using IPv4.
// AF_INET        -> Address family: IPv4
// SOCK_STREAM    -> Socket type: stream-oriented (TCP)
// SOCK_NONBLOCK  -> Make the socket non-blocking (I/O calls return immediately)
// 0              -> Let the OS choose the default protocol for TCP
int fdSock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);


