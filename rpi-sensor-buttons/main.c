#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/ip.h>

#include "types.h"
#include "drivers/gpio.h"

/* global variables */
int result = 0;
int volumeUpButtonPin = 18;   // GPIO 핀 번호, 볼륨 증가
int volumeDownButtonPin = 23; // GPIO 핀 번호, 볼륨 감소
int buttonPins[] = {volumeUpButtonPin, volumeDownButtonPin, 24, 25, 12, 16, 20, 21};
int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

/* function prototypes */
void *thread_job_socket(void *arg);
void *thread_job_button(void *arg);

/**************************************************************************
 * main function
 */
int main(void) {
  pthread_t thread_socket;
  pthread_t thread_button[numButtons];

  for (int i = 0; i < numButtons; ++i) {
    if (GPIOExport(buttonPins[i]) < 0) {
      printf("[ERR] GPIO export failed for button %d.\n", i);
      return -1;
    }
  }

  if (pthread_create(&thread_socket, NULL, thread_job_socket, NULL) < 0) {
    goto thread_fail;
  }

  for (int i = 0; i < numButtons; ++i) {
    if (pthread_create(&thread_button[i], NULL, thread_job_button, &buttonPins[i]) < 0) {
      goto thread_fail;
    }
  }

  pthread_join(thread_button[0], NULL);

  return 0;

thread_fail:
  printf("Thread creation failed. Terminating...\n");

  // Unexport GPIO before exiting
  for (int i = 0; i < numButtons; ++i) {
    GPIOUnexport(buttonPins[i]);
  }

  return -1;
}

/**************************************************************************
 * Read button state and transmit via socket
 */
void *thread_job_button(void *arg) {
  int buttonPin = *((int *)arg);
  printf("[BUTTON] setting up a button on GPIO pin %d...\n", buttonPin);

  if (GPIODirection(buttonPin, IN) < 0) {
    printf("[ERR] Failed to set GPIO direction for the button.\n");
    pthread_exit((void *) -1);
  }

  int lastbuttonState = 1;

  while (1) {
    int buttonState = GPIORead(buttonPin);

    if (buttonState == 0 && lastbuttonState == 1) {
      printf("[BUTTON] Button on GPIO pin %d pressed!\n", buttonPin);

      if (buttonPin == volumeUpButtonPin) {
        result = 1; // Set result to 1 for volume up
      } else if (buttonPin == volumeDownButtonPin) {
        result = -1; // Set result to -1 for volume down
      } else {
        result = buttonPin; // Set result to the pressed button
      }

      usleep(1000000); // Debouncing delay to prevent multiple presses
    }

    lastbuttonState = buttonState;
    usleep(100000);
  }
}

/**************************************************************************
 * Transmit sensor reading via socket
 */
void *thread_job_socket(void *arg) {
  printf("[SOCKET] server: %s\n", SERVER_IP);
  printf("[SOCKET] initiating socket...\n");

  int ret;
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0) {
    goto socket_fail;
  }

  struct sockaddr_in server;
  init_socket_server(&server, SERVER_IP, SERVER_PORT_2);

  printf("[SOCKET] waiting for server...\n");
  ret = connect(sock, (struct sockaddr*)&server, sizeof(server));

  if (ret < 0) {
    goto socket_fail;
  }

  while (1) {
    ret = write(sock, &result, sizeof(result));

    if (ret < 0) {
      printf("[SOCKET] write failed: %s\n", strerror(errno));
    }

    printf("[SOCKET] write: %d(%d)\n", result, ret);
    usleep(100000);
  }

socket_fail:
  printf("[SOCKET] socket connection failed\n");
  pthread_exit((void *) -1);
}
