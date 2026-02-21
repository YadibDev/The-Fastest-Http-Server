# Server Directory Documentation

This directory contains the core components for building a high-performance HTTP server using **epoll** for efficient handling of multiple concurrent client connections.

---

## 📁 Files Overview

### 1. **clsServerSock.hpp / clsServerSock.cpp**

**Purpose:** Manages server socket creation and configuration

**Key Features:**

- Creates server sockets bound to specific IP addresses and ports
- Supports multiple listen interfaces (can bind to different IPs/ports)
- Handles socket initialization with IPv4 TCP sockets in non-blocking mode
- Manages socket cleanup and connection acceptance
- Uses a `set<int>` to track all active server socket file descriptors

**Main Methods:**

- `buildSockets()` - Creates server sockets for given IP addresses and ports
- `tryAcceptNewClient()` - Accepts incoming client connections
- `removeSocket()` - Closes and removes a socket from tracking
- `getServerSockets()` - Returns all currently managed server sockets
- `isServerIp()` - Verifies if an IP/port pair is managed by this server

**Configuration:**

- `MAX_QUEUE = 100` - Maximum pending connections in listen queue (backlog)
- Uses `SOCK_NONBLOCK` flag for non-blocking socket operations

---

### 2. **clsEpollHandler.hpp / clsEpollHandler.cpp**

**Purpose:** Manages Linux epoll for efficient event-driven I/O multiplexing

**Key Features:**

- Wraps the Linux `epoll_create` system call for creating an event notification system
- Registers file descriptors (both server sockets and client connections) for monitoring
- Efficiently polls for events (incoming connections or data) on all registered descriptors
- Supports event mask configuration (e.g., `EPOLLIN` for read events)

**Main Methods:**

- `addServerSockets()` - Registers all server sockets with epoll for monitoring
- `addClient()` - Registers a client connection for event monitoring
- `tryPollNewClients()` - Waits for and returns events on monitored file descriptors
- `changeAbility()` - Modifies event masks for existing descriptors (e.g., switch from read to write)

**How it Works:**

1. Creates an epoll instance in the constructor
2. Server sockets are added to listen for incoming connections (`EPOLLIN`)
3. When clients connect, their file descriptors are also added for monitoring
4. `tryPollNewClients()` blocks until events occur, efficiently handling thousands of connections

---

### 3. **testing.cpp**

**Purpose:** Example/test file demonstrating basic server setup

**What it does:**

- Creates two server instances on different ports and IPs:
  - Server 1: Port 8080, IP 0.0.0.0 (all interfaces)
  - Server 2: Port 9090, IP 127.0.0.1 (localhost)
- Initializes an `clsEpollHandler` to manage both servers
- Registers both server sockets with epoll
- Verifies socket creation with `getsockname()` and `getpeername()`

**Usage:** This is a template for understanding how to initialize the server components

---

### 4. **small-documetation.md**

**Purpose:** Learning resources and references used during development

Contains links to educational materials about:

- HTTP protocol and web servers
- Socket programming concepts
- Nginx server architecture
- Epoll and multiplexing concepts

---

## 🔄 Architecture Flow

```
┌─────────────────────────────────────────────────┐
│         Application Layer (main)                │
└──────────────┬──────────────────────────────────┘
               │
        ┌──────┴──────┐
        │             │
   ┌────▼────┐   ┌────▼────┐
   │clsServerSock│   │clsServerSock│  (Multiple server instances)
   │(Port    │   │(Port     │
   │ 8080)   │   │ 9090)    │
   └────┬────┘   └────┬─────┘
        │             │
        └──────┬──────┘
               │
        ┌──────▼──────────┐
        │  clsEpollHandler   │
        │                 │
        │ Monitors all    │
        │ server sockets  │
        │ for events      │
        └──────┬──────────┘
               │
        ┌──────▼──────────┐
        │  epoll_wait()   │
        │                 │
        │ Returns when    │
        │ events occur    │
        └─────────────────┘
```

---

## 🚀 Quick Start Example

```cpp
// Create server instances
clsEpollHandler eventHandler;
clsServerSock server1;
clsServerSock server2;

// Build sockets on specific ports/IPs
vector<unsigned short> ports1 = {8080};
vector<unsigned int> ips1 = {0};  // All interfaces
server1.buildSockets(ports1, ips1);

vector<unsigned short> ports2 = {9090};
vector<unsigned int> ips2 = {INADDR_LOOPBACK};  // 127.0.0.1
server2.buildSockets(ports2, ips2);

// Register with epoll
eventHandler.addServerSockets(server1);
eventHandler.addServerSockets(server2);

// Wait for incoming connections
struct epoll_event events[100];
int numEvents = eventHandler.tryPollNewClients(events, 100, -1);
```

---

## 🔑 Key Concepts

**Non-Blocking Sockets:** All sockets are created with `SOCK_NONBLOCK` flag, allowing the server to handle multiple clients without blocking

**Epoll vs Traditional Approaches:**

- `select()`/`poll()`: Check all file descriptors every iteration (O(n))
- `epoll`: Kernel maintains ready list, returns only active descriptors (O(1))
- Efficient for thousands of concurrent connections

**Event Masks:**

- `EPOLLIN` - Socket ready for reading (new connection or data available)
- `EPOLLOUT` - Socket ready for writing
- Can be modified during runtime with `changeAbility()`

---

## ⚙️ Error Handling

- Uses try-catch for critical operations
- Returns `false` on epoll registration failures
- Removes failed sockets from tracking set
- Socket creation failures throw `std::runtime_error`

---

## 📝 Notes

- All operations use IPv4 (`AF_INET`) with TCP (`SOCK_STREAM`)
- File descriptors are managed in `set<int>` for efficient tracking
- Maximum backlog for pending connections is 100
- The architecture supports horizontal scaling to multiple server instances
