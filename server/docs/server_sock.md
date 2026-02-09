
# ServerSock Class Documentation

## Overview

`ServerSock` is a C++ class responsible for managing **server-side TCP sockets**.
It supports binding and listening on **multiple IP addresses and ports**, tracking server sockets, and accepting incoming client connections.

The class is designed to work with **non-blocking sockets** and integrates well with `epoll`.

---

## Class Definition

```cpp
class ServerSock
```

Manages the lifecycle of server sockets:

* Socket creation
* Address binding
* Listening for connections
* Accepting clients
* Cleanup and removal

---

## Constructors and Destructor

```cpp
ServerSock();
```

Creates a `ServerSock` instance and initializes internal data structures.

```cpp
~ServerSock();
```

Closes all server sockets and releases allocated resources.

---

## Public Methods

```cpp
bool isServerIp(unsigned int ip, unsigned int port);
```

Checks whether the given IP and port belong to an existing server socket.

* `ip`   : IPv4 address to check
* `port` : Port number to check

---

```cpp
void buildSockets(const vector<unsigned short> &ports,
                  const vector<unsigned int> &ip);
```

Creates and binds server sockets for all provided IP and port combinations.

* `ports` : List of ports to listen on
* `ip`    : List of IPv4 addresses to bind

---

```cpp
void removeSocket(int fd);
```

Closes and removes a server socket.

* `fd` : Server socket file descriptor

---

```cpp
int tryAcceptNewClient(int sockServer, sockaddr_in *addr);
```

Attempts to accept a new client connection from a server socket.

* `sockServer` : Server socket file descriptor
* `addr`       : Output parameter containing the client address

Returns the client socket file descriptor on success, or `-1` on failure.

---

```cpp
const set<int> &getServerSockets();
```

Returns a set containing all active server socket file descriptors.

---

## Private Methods

```cpp
int _buildSingleSocket(unsigned short port, unsigned int ip);
```

Creates, binds, and starts listening on a single server socket.

* `port` : Port number to bind
* `ip`   : IPv4 address (network byte order)

---

```cpp
void _initializeSockaffr(unsigned short port, unsigned int ip);
```

Initializes a `sockaddr_in` structure used for binding.

* `port` : Port number
* `ip`   : IPv4 address (network byte order)

---

```cpp
bool _isServerSocket(int fd);
```

Checks whether a file descriptor corresponds to a valid server socket.

* `fd` : Socket file descriptor

---

## Notes

* Uses IPv4 (`AF_INET`) and TCP (`SOCK_STREAM`)
* Designed for non-blocking I/O
* Suitable for use with `epoll`
* Supports multiple listening interfaces

---
