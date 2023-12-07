#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/ip.h>

// #define SERVER_MOCK 1
#include "types.h"
#include "drivers/gpio.h"

payload_t result;

/* Global variables */
int volumeUpButtonPin = 18;   // GPIO 핀 번호, 볼륨 증가
int volumeDownButtonPin = 23; // GPIO 핀 번호, 볼륨 감소
int soundButtons[] = {24, 25, 12, 16, 20, 21};
int numSoundButtons = sizeof(soundButtons) / sizeof(soundButtons[0]);
int maxVolume = 100;

/* Function prototypes */
void *thread_job_socket(void *arg);
void *thread_job_button(void *arg);

/* Function to adjust volume based on button press */
void adjustVolume(int buttonPin);

/* Function to play sound based on button press */
void playSound(int buttonPin);

/**************************************************************************
 * Main function
 */
int main(void) {
  pthread_t thread_socket;
  pthread_t thread_button[numSoundButtons + 2];

  result.id = ID_BUTTONS;

  for (int i = 0; i < numSoundButtons; ++i) {
    if (GPIOExport(soundButtons[i]) < 0) {
      printf("[ERR] GPIO export failed for button %d.\n", i);
      return -1;
    }
  }

  GPIOExport(volumeUpButtonPin);
  GPIOExport(volumeDownButtonPin);

  if (pthread_create(&thread_socket, NULL, thread_job_socket, NULL) < 0) {
    goto thread_fail;
  }

  for (int i = 0; i < numSoundButtons; ++i) {
    if (pthread_create(&thread_button[i], NULL, thread_job_button, &soundButtons[i]) < 0) {
      goto thread_fail;
    }
  }

  // Create threads for volume buttons
  if (pthread_create(&thread_button[numSoundButtons], NULL, thread_job_button, &volumeUpButtonPin) < 0) {
    goto thread_fail;
  }

  if (pthread_create(&thread_button[numSoundButtons + 1], NULL, thread_job_button, &volumeDownButtonPin) < 0) {
    goto thread_fail;
  }

  pthread_join(thread_button[0], NULL);

  return 0;

thread_fail:
  printf("Thread creation failed. Terminating...\n");

  // Unexport GPIO before exiting
  for (int i = 0; i < numSoundButtons; ++i) {
    GPIOUnexport(soundButtons[i]);
  }

  return -1;
}

/**************************************************************************
 * Read button state and transmit via socket
 */
void *thread_job_button(void *arg) {
  int buttonPin = *((int *)arg);
  printf("[BUTTON] setting up a button on GPIO pin %d...\n", buttonPin);

  usleep(1000000);

  if (GPIODirection(buttonPin, IN) < 0) {
    printf("[ERR] Failed to set GPIO direction for the button.\n");
    pthread_exit((void *) -1);
  }

  int lastButtonState = 1;

  while (1) {
    int buttonState = GPIORead(buttonPin);

    if (buttonState == 0 && lastButtonState == 1) {
      printf("[BUTTON] Button on GPIO pin %d pressed!\n", buttonPin);

      if (buttonPin == volumeUpButtonPin || buttonPin == volumeDownButtonPin) {
        adjustVolume(buttonPin);
      } else {
        playSound(buttonPin);
      }

      usleep(30000); // Debouncing delay to prevent multiple presses
    }

    lastButtonState = buttonState;
    usleep(30000);
  }
}

/* Function to adjust volume based on button press */
void adjustVolume(int buttonPin) {
  if (buttonPin == volumeUpButtonPin) {
    // Increase volume
    if (result.volume < maxVolume) {
      result.volume += 10;
    }
  } else if (buttonPin == volumeDownButtonPin) {
    // Decrease volume
    if (result.volume > 0) {
      result.volume -= 10;
    }
  }

  // Print the adjusted volume
  printf("[VOLUME] Adjusted volume: %d\n", result.volume);
}

/* Function to play sound based on button press */
void playSound(int buttonPin) {
  switch (buttonPin){
    case 24:
      result.note = 48;
      break;
    
    case 25:
      result.note = 50;
      break;

    case 12:
      result.note = 52;
      break;

    case 16:
      result.note = 53;
      break;

    case 20:
      result.note = 55;
      break;

    case 21:
      result.note = 57;
      break;
  }
  
}

/**************************************************************************
 * Transmit sensor reading via socket
 */
void *thread_job_socket(void *arg)
{
  printf("[SOCKET] server: %s\n", SERVER_IP);
  printf("[SOCKET] initiating socket...\n");

  int ret;
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0)
  {
    goto socket_fail;
  }

  struct sockaddr_in server;
  init_socket_server(&server, SERVER_IP, SERVER_PORT);

  printf("[SOCKET] waiting for server...\n");
  ret = connect(sock, (struct sockaddr *)&server, sizeof(server));

  if (ret < 0)
  {
    goto socket_fail;
  }

  while (1)
  {
    if(result.note < 30){
      continue;
    }
    ret = write(sock, &result, sizeof(result));

    if (ret < 0)
    {
      printf("[SOCKET] write failed: %s\n", strerror(errno));
    }

    printf("[SOCKET] write: %d %d %d(%d)\n", result.id, result.note, result.volume, ret);
    result.note = 0;
    usleep(100000);
  }

socket_fail:
  printf("[SOCKET] socket connection failed\n");
  pthread_exit((void *)-1);
}