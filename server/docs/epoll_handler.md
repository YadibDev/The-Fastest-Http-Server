Below is a **single, polished `README.md` section** for the `clsEpollHandler` class, written in the **same style as your `clsServerSock` documentation**, but with **clearer explanations** and slightly improved wording where it matters.

You can copy-paste this directly into your GitHub README.

---

# clsEpollHandler Class Documentation

## Overview

`clsEpollHandler` is a C++ utility class that wraps Linux **epoll** functionality.
It manages the epoll instance, registers server and client sockets, modifies event
interests, and waits for I/O readiness events.

This class is designed to work alongside `clsServerSock` to efficiently handle
multiple connections using **non-blocking I/O**.

---

## Class Definition

```cpp
class clsEpollHandler
```

Handles:

- Creation and destruction of an epoll instance
- Registration of server and client file descriptors
- Modification of epoll event interests
- Polling for ready file descriptors

---

## Constructors and Destructor

```cpp
clsEpollHandler();
```

Creates an epoll instance and initializes internal state.

```cpp
~clsEpollHandler();
```

Closes the epoll file descriptor and releases resources.

---

## Public Methods

```cpp
bool addServerSockets(clsServerSock &SocketsServer,
                      int ability = EPOLLIN);
```

Registers all server sockets managed by `clsServerSock` into the epoll instance.

- `SocketsServer` : Reference to a `clsServerSock` object
- `ability` : Epoll events to clsLinker (default: `EPOLLIN`)

Returns `true` if at least one server socket was successfully added.

---

```cpp
bool addClient(int fd, int ability = EPOLLIN);
```

Adds a client or server file descriptor to the epoll watch list.

- `fd` : File descriptor to clsLinker
- `ability` : Epoll events to clsLinker (default: `EPOLLIN`)

Returns `true` on success, `false` on failure.

---

```cpp
void changeAbility(int fd, int newAbility);
```

Modifies the epoll events associated with an existing file descriptor.

- `fd` : File descriptor to modify
- `newAbility` : New epoll event mask

Used to switch between read/write readiness (e.g. `EPOLLIN ↔ EPOLLOUT`).

---

```cpp
int tryPollNewClients(struct epoll_event *ClientBuffer,
                      size_t sizeBuffer,
                      int timeout);
```

Waits for events on registered file descriptors.

- `ClientBuffer` : Output array of `epoll_event` structures
- `sizeBuffer` : Maximum number of events to return
- `timeout` : Wait timeout in milliseconds (`-1` blocks indefinitely)

Returns the number of ready file descriptors, or `-1` on error.

---

## Private Members

```cpp
int _EpollFd;
```

File descriptor representing the epoll instance.

```cpp
struct epoll_event tempEvent;
```

Temporary structure used for `epoll_ctl` operations.

---

## Design Notes

- Uses `epoll_create` to initialize the epoll instance
- Works with **non-blocking sockets**
- Designed for high-performance I/O multiplexing
- Clean separation between socket management (`clsServerSock`) and event handling

---

## Typical Flow

1. Create `clsServerSock` and build server sockets
2. Create `clsEpollHandler`
3. Register server sockets using `addServerSockets()`
4. Accept clients and register them with `addClient()`
5. Use `tryPollNewClients()` to process events
6. Modify interests with `changeAbility()` when needed

---

If you want next:

- A **combined architecture section** (`clsServerSock + clsEpollHandler`)
- A **minimal epoll server example**
- A **best-practices section** (EPOLLET vs level-triggered, edge cases)

Just say the word.
