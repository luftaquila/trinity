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

$(TARGET):%: $(C_DRIVERS) ./%/main.c | MKDIR
	$(CC) $(CFLAGS) $^ -o $(OUTPUT_DIR)/$@

clean:
	rm -rf $(OUTPUT_DIR)
