# trinity

## git
### create your working branch
```sh
git checkout -b feature/<your_branch_name>
```

### merge with origin/main
```sh
git checkout feature/<your_branch_name> # switch to your branch
git merge origin/main
```

## include drivers
```c
#include "driviers/gpio.h"
#include "driviers/pwm.h"
#include "driviers/i2c.h"
```

## build
### all
```sh
make
```
### specific targets only
```sh
make rpi-<your_device>
```
ex) `make rpi-amp`

## execute and debug
```sh
./build/rpi-<your_device>
gdb ./build/rpi-<your_device>
```
ex) `./build/rpi-amp`, `gdb ./build/rpi-amp`