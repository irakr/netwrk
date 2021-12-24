SHELL = /bin/bash

PROJECT_ROOT := $(PWD)

# Assuming your project source files are in the current directory.
SOURCES = $(shell find . -name '*.c')

OBJECTS = $(SOURCES:.c=.o)

# Assign your project's output binary name
OUT = ftp-cli

# Root directory of the build environment.
# Hint: Where your sysroot is.
# Another hint: Where you cloned this repo.
DEVENV_DIR = /home/irak101/workspace/raspi/sys

SYSROOT_DIR = $(DEVENV_DIR)/sysroot
SYSROOT = --sysroot=$(SYSROOT_DIR)

# Toolchain prefix. You can assign whatever you're using.
# Hardcoded one for now.
TOOLCHAIN_PREFIX = arm-linux-gnueabihf-

CC = $(TOOLCHAIN_PREFIX)gcc
AR = $(TOOLCHAIN_PREFIX)ar

# Library linkages
LIB_DIRS = -L$(MISC_DIR)/lib -L$(SYSROOT_DIR)/usr/lib/arm-linux-gnueabihf \
	-L$(SYSROOT_DIR)/opt/vc/lib -L$(SYSROOT_DIR)/usr/local/lib

LIBS = $(LIB_DIRS)	\
	-Wl,--start-group \
	-lpthread -ldl	\
	-Wl,--end-group

LFLAGS = --sysroot=$(SYSROOT_DIR) \
	-Wl,-rpath-link,$(SYSROOT_DIR)/opt/vc/lib \
	-Wl,-rpath-link,$(SYSROOT_DIR)/usr/lib/arm-linux-gnueabihf \
	-Wl,-rpath-link,$(SYSROOT_DIR)/lib/arm-linux-gnueabihf

# Compiler flags
CFLAGS = -std=c11 -Wall -fPIC \
	$(SYSROOT) $(EXTRA_CFLAGS)

# Custom macros
DEFINES =

# Custom macro definitions
# XXX... Unused as for now.
CONFIGS = 

# Command line arg parse
ifeq ($(debug), y)
	CFLAGS += -g -O0 -ggdb
	DEFINES += -DDEBUG_ON
	OUT := $(OUT)-dbg
endif

# Directories for required header files
INCLUDES = -I$(SYSROOT_DIR)/usr/include/arm-linux-gnueabihf

CPPFLAGS = $(DEFINES) $(INCLUDES)

# Generate intermediate object files
%.o: %.c
	@echo "\n-----------------------------------------"
	@echo $<
	@echo "-------------------------------------------"
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -c -o $@ $(LIBS)

$(OUT): $(OBJECTS)
	@echo "\n-----------------------------------------"
	@echo "              Linking                   "
	@echo "-------------------------------------------"
	$(CC) $(LFLAGS) $(CFLAGS) $^ -o $@ $(LIBS)

.PHONY: build
build: $(OUT)

.PHONY: clean
clean:
	-rm -rf $(OBJECTS) $(OUT) *.o

DEPLOY_FILES = $(OUT)
TARGET_DEPLOY_DIR = /home/$(TARGET_USER)/my-apps/ftp-cli

.PHONY: deploy
deploy:
	@ if [[ "${TARGET_IP}" == "" || "${TARGET_USER}" == "" ]];\
	then\
		echo "ERROR: Cannot find environment variables TARGET_IP and TARGET_USER.";\
		exit 1;\
	fi
	ssh $(TARGET_USER)@$(TARGET_IP) mkdir -p $(TARGET_DEPLOY_DIR)
	scp $(DEPLOY_FILES) $(TARGET_USER)@$(TARGET_IP):$(TARGET_DEPLOY_DIR)

.PHONY: check_env
check_env:
	if [[ "${TARGET_IP}" == "" || "${TARGET_USER}" == "" ]]; then\
		echo "ERROR: Environment variables TARGET_IP and TARGET_USER not set.";\
		exit 1;\
	fi
