# trinity

## 1. git
### recommendation: [Fork](https://git-fork.com/), a GUI git client

### create your working branch
```sh
git checkout -b feature/<your_branch_name>
```

### rebase with origin/main
```sh
git rebase origin/main <your_branch_name>
```

## 2. programming
### including drivers
```c
#include "driviers/<driver_name>.h"
```

### setting up a socket

```c
#include <sys/socket.h>
#include <netinet/ip.h>

#include "types.h"

...
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in server;
  init_socket_server(&server, SERVER_IP, SERVER_PORT_#); // < replace SERVER_PORT_# to your port

  int ret = connect(sock, (struct sockaddr*)&server, sizeof(server));
...
```

## 3. build
### setup
```sh
git submodule update --init --recursive
```

### all
```sh
make
```
### only specific target
```sh
make rpi-<your_device>
```
ex) `make rpi-amp`

## 4. execute and debug
```sh
./build/rpi-<your_device> # execute
gdb ./build/rpi-<your_device> # debug
```

## 5. mockup socket server
### run server
```sh
make server-mockup
build/server-mockup
```

#### run in background
```sh
build/server-mockup &
```

### set mockup server in your device
```c
#define SERVER_MOCK 1 // must declared before including types.h

#include "types.h"
```
will set `SERVER_IP` to `127.0.0.1`.
