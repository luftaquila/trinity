#ifndef __DRIVER_GPIO_H
#define __DRIVER_GPIO_H


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

#endif /* __DRIVER_GPIO_H */
