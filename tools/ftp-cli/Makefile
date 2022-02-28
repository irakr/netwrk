SHELL = /bin/bash

PROJECT_ROOT := $(PWD)/../..

# Source files located in lib/netwrk
LIB_SOURCES = $(shell find ${PROJECT_ROOT}/lib/netwrk -name '*.c')

# Source files in current directory.
SOURCES = $(shell find . -name '*.c')

# All source files
SOURCES += $(LIB_SOURCES)

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Assign your project's output binary name
OUT = ftp-cli

# SYSROOT_PATH is the path to sysroot of the target platform and
# is set as an environment variable.
SYSROOT = --sysroot=$(SYSROOT_PATH)

#
# PLATFORM is the part of the filename of a platform specific
# makefile before .mk
#
# By default, PLATFORM is set to linux-x86_64 if:
# - PLATFORM == ""
# - PLATFORM == "linux"
#
ifeq ("$(PLATFORM)","")
override PLATFORM=linux-x86_64
else ifeq ("$(PLATFORM)","linux")
override PLATFORM=linux-x86_64
endif

include $(PLATFORM).mk

# TOOLCHAIN_PREFIX is set by platform specific mk files.
# By default it uses this system's toolchain, i.e., TOOLCHAIN_PREFIX is empty.
CC = $(TOOLCHAIN_PREFIX)gcc
CXX = $(TOOLCHAIN_PREFIX)g++
AR = $(TOOLCHAIN_PREFIX)ar

LIBS = -L$(SYSROOT_DIR)/usr/local/lib \
	-Wl,--start-group \
	-lpthread -lglib-2.0 \
	-Wl,--end-group \
	$(PLATFORM_LIBS)

LFLAGS = $(SYSROOT) \
	$(PLATFORM_LFLAGS)

# Compiler flags
CFLAGS = -std=gnu99 -Wall -fPIC \
	$(SYSROOT)

# Custom macros
DEFINES =

# Custom macro definitions
CONFIGS = 

# Directories for required header files
INCLUDES += -I$(PROJECT_ROOT)/lib \
	-I$(SYSROOT_PATH)/usr/include/glib-2.0 \
	$(PLATFORM_INCLUDES)

CPPFLAGS = $(DEFINES) $(INCLUDES)

# Set debug mode
ifeq ($(debug), y)
	CFLAGS += -g -O0
endif

# Generate intermediate object files
%.o: %.c
	@echo ""
	@echo "-----------------------------------------"
	@echo Compiling: $<
	@echo "-------------------------------------------"
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -c -o $@

$(OUT): $(OBJECTS)
	@echo ""
	@echo "-----------------------------------------"
	@echo "       Linking everything                  "
	@echo "-------------------------------------------"
	$(CC) $(LFLAGS) $(CFLAGS) $^ -o $@ $(LIBS)

.PHONY: build
build: $(OUT)

.PHONY: clean
clean:
	-rm -rf $(OBJECTS) $(OUT) *.o

DEPLOY_FILES = $(OUT)
TARGET_DEPLOY_PATH = /home/$(TARGET_USER)/netwrk/ftp-cli

.PHONY: deploy
deploy:
	@ if [[ "${TARGET_IP}" == "" || "${TARGET_USER}" == "" ]];\
	then\
		echo "ERROR: Cannot find environment variables TARGET_IP and TARGET_USER.";\
		exit 1;\
	fi
	ssh $(TARGET_USER)@$(TARGET_IP) mkdir -p $(TARGET_DEPLOY_PATH)
	scp $(DEPLOY_FILES) $(TARGET_USER)@$(TARGET_IP):$(TARGET_DEPLOY_PATH)
