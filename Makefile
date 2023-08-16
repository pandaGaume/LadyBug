VERSION = 0.0.1
COMPILER = g++
# Options passed to COMPILER
CFLAGS = -Wall 
ifdef DEBUG
    CFLAGS += -g
endif

HEADER_DIR := include
SRC_DIR := src

PLATFORM = win
PLATFORM_DIR= platforms
PLATFORMS_HEADER_DIR := $(PLATFORM_DIR)/$(PLATFORM)/$(HEADER_DIR)
PLATFORMS_SRC_DIR := $(PLATFORM_DIR)/$(PLATFORM)/$(SRC_DIR)
SAMPLES_DIR := samples

ifdef DEBUG
    TARGET := debug
else
    TARGET := release
endif

BUILD_DIR :=  obj/$(TARGET)
BIN_DIR := bin/$(TARGET)

# Recursive wildcard function
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2)$(filter $(subst *,%,$2),$d))

# List of source files (using recursive wildcard)
ifeq ($(PLATFORM),win)
    SRC_FILES = $(foreach d,$(SRC_DIR),$(call rwildcard,$d/,*.cpp))
    PLATFORMS_SRC_FILES = $(foreach d,$(PLATFORMS_SRC_DIR),$(call rwildcard,$d/,*.cpp))
    SAMPLES_FILES = $(foreach d,$(SAMPLES_DIR),$(call rwildcard,$d/,*.cpp))
else
    SRC_FILES = $(call rwildcard,$(SRC_DIR)/,*.cpp)
    PLATFORMS_SRC_FILES = $(call rwildcard,$(PLATFORMS_SRC_DIR)/,*.cpp)
    SAMPLES_FILES = $(call rwildcard,$(SAMPLES_DIR)/,*.cpp)
endif


# Generate a list of object file names by replacing .cpp with .o or .exe
PLATFORMS_OBJ_FILES = $(patsubst $(PLATFORMS_SRC_DIR)/%.cpp, $(BUILD_DIR)/$(PLATFORMS_SRC_DIR)/%.o, $(PLATFORMS_SRC_FILES))
SAMPLES_OBJ_FILES = $(patsubst $(SAMPLES_DIR)/%.cpp, $(BUILD_DIR)/$(SAMPLES_DIR)/%.o, $(SAMPLES_FILES))
SAMPLES_EXE_FILES = $(patsubst $(SAMPLES_DIR)/%.cpp, $(BIN_DIR)/$(SAMPLES_DIR)/%.exe, $(SAMPLES_FILES))

OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/$(SRC_DIR)/%.o, $(SRC_FILES))
OBJ_FILES += $(PLATFORMS_OBJ_FILES)
OBJ_FILES += $(SAMPLES_OBJ_FILES)

# Platform-specific settings
ifeq ($(PLATFORM),win)
    RM := rmdir /Q /S
    MKDIR := mkdir
else
    RM := rm -f
    MKDIR := mkdir -p
endif

# PHONY targets to avoid conflicts with file names
.PHONY: all clean link

# Default target
compile: $(OBJ_FILES)

# Rule to build object files from CPP source files
$(BUILD_DIR)/%.o: %.cpp
	-$(MKDIR) $(subst /,\,$(@D))
	-$(COMPILER) -I$(HEADER_DIR) -I$(PLATFORMS_HEADER_DIR) $(CFLAGS) -c $< -o $@

# build all samples executables
link: $(SAMPLES_EXE_FILES)

$(SAMPLES_EXE_FILES): $(OBJ_FILES)
	-$(MKDIR) $(subst /,\,$(@D))
	-$(COMPILER) -I$(HEADER_DIR) -I$(PLATFORMS_HEADER_DIR) $(CFLAGS) $(OBJ_FILES) -o $@

# Clean rule to remove object files and the executable
clean:
	-$(RM) $(subst /,\,$(BIN_DIR))
	-$(RM) $(subst /,\,$(BUILD_DIR))



