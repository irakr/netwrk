TOOLCHAIN_PREFIX = 

PLATFORM_INCLUDES = 

# Library linkages
PLATFORM_LIBS = \
	-Wl,--start-group \
	-lpthread -ldl \
	-Wl,--end-group

PLATFORM_LFLAGS = 