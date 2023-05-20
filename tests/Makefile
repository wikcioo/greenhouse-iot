OUTPUT_FILENAME := gtest_tests

CC  := gcc
CXX := g++

DRIVERS_INC_DIR   := ../drivers/include
USER_CODE_INC_DIR := ../include
USER_CODE_SRC_DIR := ../src
MOCKS_DIR         := mocks
TEST_SRC_DIR      := src
FFF_DIR           := vendor
BUILD_DIR         := build

CGCOV :=
LGCOV :=

ifeq ($(GEN_COV),true)
	CGCOV    := -fprofile-arcs -ftest-coverage
	LGCOV    := -lgcov
endif

CFLAGS   := -c -DTEST_ENV $(CGCOV) -I$(DRIVERS_INC_DIR) -I$(USER_CODE_INC_DIR) -I$(MOCKS_DIR)
CXXFLAGS := -c -I$(DRIVERS_INC_DIR) -I$(USER_CODE_INC_DIR) -I$(MOCKS_DIR) -I$(FFF_DIR)

LDFLAGS  := $(LGCOV) -lgtest -lpthread

# Add user source files you want to test to C_FILES variable
C_FILES   := payload.c time_point.c co2.c humidity_temperature.c lorawan.c water_controller.c scheduler.c hardware_controller.c
C_SOURCES := $(addprefix $(USER_CODE_SRC_DIR)/, $(C_FILES))

CPP_SOURCES += $(wildcard $(MOCKS_DIR)/*.cpp)
CPP_SOURCES += $(wildcard $(TEST_SRC_DIR)/*.cpp)

OBJECTS := $(addprefix $(BUILD_DIR)/, $(addsuffix .c.o, $(basename $(notdir $(C_SOURCES)))))
OBJECTS += $(addprefix $(BUILD_DIR)/, $(addsuffix .cpp.o, $(basename $(notdir $(CPP_SOURCES)))))

BEAR_OUTPUT_FILES := .cache/ compile_commands.json

.PHONY = all clean

all:
	@echo "Building tests"
	@mkdir -p $(BUILD_DIR)
	@make --no-print-directory $(BUILD_DIR)/$(OUTPUT_FILENAME)

$(BUILD_DIR)/$(OUTPUT_FILENAME): $(OBJECTS)
	$(CXX) $^ $(LDFLAGS) -o $@

$(BUILD_DIR)/%.c.o: $(USER_CODE_SRC_DIR)/%.c
	$(CC) $^ $(CFLAGS) -o $@

$(BUILD_DIR)/%.cpp.o: $(MOCKS_DIR)/%.cpp
	$(CXX) $^ $(CXXFLAGS) -o $@

$(BUILD_DIR)/%.cpp.o: $(TEST_SRC_DIR)/%.cpp
	$(CXX) $^ $(CXXFLAGS) -o $@

clean:
	rm -rf $(BUILD_DIR) $(BEAR_OUTPUT_FILES)