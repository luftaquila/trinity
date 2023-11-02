TARGET = \
	rpi-amp \
	rpi-sensor-buttons \
	rpi-sensor-ultrasonic \
	rpi-sensor-waterlevel

OUTPUT_DIR = build

C_INCLUDES = \
	-Iincludes

C_DRIVERS = $(shell find drivers -name "*.c")

CC = gcc
CFLAGS = -g -Wall $(C_INCLUDES)
LDFLAGS =

.PHONY: $(TARGET) MKDIR clean

all: $(TARGET)

MKDIR:
	@ mkdir -p $(OUTPUT_DIR)

rpi-amp: $(C_DRIVERS) ./rpi-amp/main.c | MKDIR
	$(CC) $(CFLAGS) $^ -o $(OUTPUT_DIR)/$@

rpi-sensor-buttons: $(C_DRIVERS) ./rpi-sensor-buttons/main.c | MKDIR
	$(CC) $(CFLAGS) $^ -o $(OUTPUT_DIR)/$@

rpi-sensor-ultrasonic: $(C_DRIVERS) ./rpi-sensor-ultrasonic/main.c | MKDIR
	$(CC) $(CFLAGS) $^ -o $(OUTPUT_DIR)/$@

rpi-sensor-waterlevel: $(C_DRIVERS) ./rpi-sensor-waterlevel/main.c | MKDIR
	$(CC) $(CFLAGS) $^ -o $(OUTPUT_DIR)/$@

clean:
	rm -rf $(OUTPUT_DIR)
