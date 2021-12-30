TOOLCHAIN_PREFIX = arm-linux-gnueabihf-

PLATFORM_INCLUDES = -I$(SYSROOT_DIR)/usr/include/arm-linux-gnueabihf

# Library linkages
PLATFORM_LIBS = -L$(SYSROOT_DIR)/usr/lib/arm-linux-gnueabihf 	\
	-L$(SYSROOT_DIR)/opt/vc/lib					\
	-Wl,--start-group \
	-lpthread -ldl	\
	-Wl,--end-group

PLATFORM_LFLAGS = -Wl,-rpath-link,$(SYSROOT_DIR)/opt/vc/lib \
	-Wl,-rpath-link,$(SYSROOT_DIR)/usr/lib/arm-linux-gnueabihf \
	-Wl,-rpath-link,$(SYSROOT_DIR)/lib/arm-linux-gnueabihf

