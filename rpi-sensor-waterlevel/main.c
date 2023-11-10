#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>

#include "types.h"
#include "drivers/ledmatrix.h"

int main(void) {
  int ret = ledmatrix_setup();

  if (ret < 0) {
    return -1;
  }

  int arr[4] = { 0, };

  int cnt = 0;
  while (1) {
    arr[0] = (cnt * 1) % 160;
    arr[1] = (cnt * 2) % 160;
    arr[2] = ((cnt * 1) + 10) % 160;
    arr[3] = ((cnt * 3) + 10) % 160;
    ledmatrix_drawgraph(arr, sizeof(arr) / sizeof(arr[0]));
    usleep(10000);

    cnt++;
  }

  usleep(10000000);

  return 0;
}
