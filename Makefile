FIRMWARE_NAME := iot_firmware
FIRMWARE      := $(FIRMWARE_NAME)
FIRMWARE_HEX  := $(FIRMWARE_NAME).hex

TOOLCHAIN := avr-
CC        := $(TOOLCHAIN)gcc
MCU       := atmega2560

FREE_RTOS_SRC_DIR := submodules/freeRTOS/src
DRIVERS_DIR       := submodules/drivers
USER_CODE_INC_DIR := inc
USER_CODE_SRC_DIR := src
BUILD_DIR         := build
TESTS_DIR         := tests

IOT_LIB   := IotLibrary
CPU_SPEED := 16000000L

CFLAGS  := -c -mmcu=$(MCU) -std=gnu11 -O2 -DF_CPU=$(CPU_SPEED) -I$(FREE_RTOS_SRC_DIR) -I$(DRIVERS_DIR) -I$(USER_CODE_INC_DIR)
LDFLAGS := -mmcu=$(MCU) -L$(DRIVERS_DIR) -lm -l$(IOT_LIB)

SOURCES := $(wildcard $(FREE_RTOS_SRC_DIR)/*.c)
SOURCES += $(wildcard $(USER_CODE_SRC_DIR)/*.c)
SOURCES += $(wildcard $(USER_CODE_SRC_DIR)/**/*.c)
OBJECTS := $(addprefix $(BUILD_DIR)/, $(addsuffix .c.o, $(basename $(notdir $(SOURCES)))))

BEAR_OUTPUT_FILES := .cache/ compile_commands.json

.PHONY = all flash test clean

all:
	@echo "Building firmware"
	@mkdir -p $(BUILD_DIR)
	@make --no-print-directory $(BUILD_DIR)/$(FIRMWARE)
	@$(TOOLCHAIN)objcopy -O ihex -j .text -j .data $(BUILD_DIR)/$(FIRMWARE) $(BUILD_DIR)/$(FIRMWARE_HEX)

$(BUILD_DIR)/$(FIRMWARE): $(OBJECTS)
	$(CC) $^ $(LDFLAGS) -o $@

$(BUILD_DIR)/%.c.o: $(FREE_RTOS_SRC_DIR)/%.c
	$(CC) $^ $(CFLAGS) -o $@

$(BUILD_DIR)/%.c.o: $(USER_CODE_SRC_DIR)/%.c
	$(CC) $^ $(CFLAGS) -o $@

flash: $(BUILD_DIR)/$(FIRMWARE_HEX)
	avrdude -C /etc/avrdude.conf -v -p m2560 -c atmelice_isp -P usb -U flash:w:$^:i

test:
	@$(MAKE) -C $(TESTS_DIR) --no-print-directory

clean:
	@echo "Cleaning top level directory"
	rm -rf $(BUILD_DIR) $(BEAR_OUTPUT_FILES)

clean_test:
	@echo "Cleaning tests directory"
	@$(MAKE) -C $(TESTS_DIR) clean --no-print-directory

clean_all:
	@$(MAKE) clean --no-print-directory
	@$(MAKE) clean_test --no-print-directory