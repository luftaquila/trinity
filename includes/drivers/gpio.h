#ifndef __DRIVER_GPIO_H
#define __DRIVER_GPIO_H

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_MAX 3
#define DIRECTION_MAX 45
#define VALUE_MAX 30
#define IN 0
#define OUT 1
#define LOW 0
#define HIGH 1

int GPIOExport(int pin);
int GPIOUnexport(int pin);
int GPIODirection(int pin, int dir);
int GPIORead(int pin);
int GPIOWrite(int pin, int value);

/**
 * Initialize GPIO.
 * @return 0 on success, -1 on failure.
 */
int gpio_setup(void);

/**
 * Export a GPIO pin.
 * @param pin GPIO pin number.
 * @return 0 on success, -1 on failure.
 */
int gpio_export(int pin);

/**
 * Unexport a GPIO pin.
 * @param pin GPIO pin number.
 * @return 0 on success, -1 on failure.
 */
int gpio_unexport(int pin);

/**
 * Set the direction of a GPIO pin.
 * @param pin GPIO pin number.
 * @param direction Direction to set (1 for output, 0 for input).
 * @return 0 on success, -1 on failure.
 */
int gpio_set_direction(int pin, int direction);

/**
 * Write a value to a GPIO pin.
 * @param pin GPIO pin number.
 * @param value Value to write (1 for HIGH, 0 for LOW).
 * @return 0 on success, -1 on failure.
 */
int gpio_write(int pin, int value);

/**
 * Read the value from a GPIO pin.
 * @param pin GPIO pin number.
 * @return Value read (1 for HIGH, 0 for LOW), -1 on failure.
 */
int gpio_read(int pin);


#endif /* __DRIVER_GPIO_H */