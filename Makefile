TARGET = \
	rpi-sensor-buttons \
	rpi-sensor-ultrasonic \
	rpi-sensor-waterlevel \
	server-mockup
	# rpi-amp

OUTPUT_DIR = build

RGBLED_DIR = drivers/rpi-rgb-led-matrix
RGBLED_LIB = $(RGBLED_DIR)/lib/librgbmatrix.a

C_INCLUDES = \
	-Iincludes \
	-I$(RGBLED_DIR)/include

C_DRIVERS = $(shell find drivers -maxdepth 1 -name "*.c")

CC = g++
CFLAGS = -g -Wall -pthread $(C_INCLUDES)
LDFLAGS =

.PHONY: $(TARGET) MKDIR clean

all: $(TARGET)

MKDIR:
	@ mkdir -p $(OUTPUT_DIR)

$(RGBLED_LIB):
	$(MAKE) -C $(RGBLED_DIR)/lib

$(TARGET):%: $(C_DRIVERS) ./%/main.c $(RGBLED_LIB) | MKDIR
	$(CC) $(CFLAGS) $^ -o $(OUTPUT_DIR)/$@

clean:
	rm -rf $(OUTPUT_DIR)
