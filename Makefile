FIRMWARE_NAME = iot_firmware
FIRMWARE = $(FIRMWARE_NAME)
FIRMWARE_HEX = $(FIRMWARE_NAME).hex
TOOLCHAIN = avr-
CC = $(TOOLCHAIN)gcc
MCU = atmega2560
FREE_RTOS_SRC_DIR = freeRTOS/src
DRIVERS_DIR = drivers
FIRMWARE_INC_DIR = inc
FIRMWARE_SRC_DIR = src
TESTS_DIR = tests
BUILD_DIR = build
IOT_LIB = IotLibrary
CPU_SPEED = 16000000L
CFLAGS = -c -mmcu=$(MCU) -std=gnu11 -O2 -DF_CPU=$(CPU_SPEED) -I$(FREE_RTOS_SRC_DIR) -I$(DRIVERS_DIR) -I$(FIRMWARE_INC_DIR)
LDFLAGS = -mmcu=$(MCU) -L$(DRIVERS_DIR) -lm -l$(IOT_LIB)
SOURCES  = $(wildcard $(FREE_RTOS_SRC_DIR)/*.c)
SOURCES += $(wildcard $(FIRMWARE_SRC_DIR)/*.c)
SOURCES += $(wildcard $(FIRMWARE_SRC_DIR)/**/*.c)
OBJECTS  = $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))

.PHONY = all test clean

all:
	@echo $(SOURCES)
	@mkdir -p $(BUILD_DIR)
	@make --no-print-directory $(BUILD_DIR)/$(FIRMWARE)
	@$(TOOLCHAIN)objcopy -O ihex -j .text -j .data $(BUILD_DIR)/$(FIRMWARE) $(BUILD_DIR)/$(FIRMWARE_HEX)

$(BUILD_DIR)/$(FIRMWARE): $(OBJECTS)
	$(CC) $^ $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: $(FREE_RTOS_SRC_DIR)/%.c
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(FIRMWARE_SRC_DIR)/%.c
	$(CC) $(CFLAGS) $^ -o $@

flash: $(BUILD_DIR)/$(FIRMWARE_HEX)
	avrdude -C /etc/avrdude.conf -v -p m2560 -c atmelice_isp -P usb -U flash:w:$^:i

test:
	$(CXX) $(TESTS_DIR)/*.cpp -lgtest -lpthread -o $(TESTS_DIR)/tests
	./$(TESTS_DIR)/tests
	rm -rf $(TESTS_DIR)/tests

clean:
	rm -rf $(BUILD_DIR) .cache/ compile_commands.json
