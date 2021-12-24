PROJECT_ROOT := $(PWD)

# Assuming your project source files are in the current directory.
SOURCES = $(shell find . -name '*.c')

OBJECTS = $(SOURCES:.c=.o)

# Assign your project's output binary name
OUT = ftp-cli

# SYSROOT_PATH is the path to sysroot of the target platform and
# is set as an environment variable.
ifdef PLATFORM
SYSROOT = --sysroot=$(SYSROOT_PATH)
endif

ifeq ("$(PLATFORM)","raspi")
include raspi.mk
endif

# TOOLCHAIN_PREFIX is set by platform specific mk file.
# By default it uses this system's toolchain, i.e., TOOLCHAIN_PREFIX is empty.
CC = $(TOOLCHAIN_PREFIX)gcc
CXX = $(TOOLCHAIN_PREFIX)g++
AR = $(TOOLCHAIN_PREFIX)ar

LIBS = $(LIB_DIRS)	\
	$(PLATFORM_LIBS)

LFLAGS = $(SYSROOT) \
	$(PLATFORM_LFLAGS)

# Compiler flags
CFLAGS = -std=c11 -Wall -fPIC \
	$(SYSROOT)

# Custom macros
DEFINES =

# Custom macro definitions
CONFIGS = 

# Directories for required header files
INCLUDES += $(PLATFORM_INCLUDES)

CPPFLAGS = $(DEFINES) $(INCLUDES)

# Set debug mode
ifeq ($(debug), y)
	CFLAGS += -g -O0 -ggdb
	DEFINES += -DDEBUG_ON
	OUT := $(OUT)-dbg
endif

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
TARGET_DEPLOY_DIR = /home/$(TARGET_USER)/netwrk/ftp-cli

.PHONY: deploy
deploy:
	@ if [[ "${TARGET_IP}" == "" || "${TARGET_USER}" == "" ]];\
	then\
		echo "ERROR: Cannot find environment variables TARGET_IP and TARGET_USER.";\
		exit 1;\
	fi
	ssh $(TARGET_USER)@$(TARGET_IP) mkdir -p $(TARGET_DEPLOY_DIR)
	scp $(DEPLOY_FILES) $(TARGET_USER)@$(TARGET_IP):$(TARGET_DEPLOY_DIR)
