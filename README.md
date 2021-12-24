# netwrk
A collection of useful cross platform networking tools.

**Steps to build**

1. Each tool may be individually compiled. For example, ftp-cli is compiled as shown below: 
```
  $ cd tools/ftp-cli
  $ make
```

**Cross compiling for ARM based systems**

1. First setup the repo [irakr/RPi-Cross-Build-Environment](https://github.com/irakr/RPi-Cross-Build-Environment) in a separate directory.
2. Before compiling, set the TOOLCHAIN_PREFIX and SYSROOT_DIR appropriately in the Makefile(s).  
   Example:  
   TOOLCHAIN_PREFIX=arm-linux-gnueabihf-
   SYSROOT_DIR=/home/abc/workspace/raspi/sys
   
3. Compile:
```
  $ make
```
4. To deploy the binaries, the TARGET_IP and TARGET_USER environment variables must be set first. Then run:
```
  $ make deploy
```
