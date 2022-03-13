TOOLCHAIN_PREFIX = arm-linux-gnueabihf-

PLATFORM_INCLUDES = -I$(SYSROOT_PATH)/usr/include/arm-linux-gnueabihf

# Library linkages
PLATFORM_LIBS = -L$(SYSROOT_PATH)/usr/lib/arm-linux-gnueabihf 	\
	-L$(SYSROOT_PATH)/opt/vc/lib					\
	
PLATFORM_LFLAGS = -Wl,-rpath-link,$(SYSROOT_PATH)/opt/vc/lib \
	-Wl,-rpath-link,$(SYSROOT_PATH)/usr/lib/arm-linux-gnueabihf \
	-Wl,-rpath-link,$(SYSROOT_PATH)/lib/arm-linux-gnueabihf

