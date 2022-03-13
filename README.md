# netwrk
A collection of useful, yet to be cross platform, networking tools.
The purpose of this project is to learn how to implement networking protocols and tools from scratch.

Each networking tools and protocol implementations resides inside the `tools/` directory.
The core networking routines and other utilities are in the `lib/` directory.

**Tools currrently available**

1. **ftp-cli**: An FTP file downloader tool. A very minimal implementation of the FTP download protocol.
```
Usage:
ftp-cli <ftp-url>

Example:
ftp-cli ftp://134.119.8.75/zoc/zoc.exe
```

2. **packet-sniffer**: A simple packet sniffer tool.
```
Usage(Requires root privilege):
sudo packet-sniffer <interface-name>

Example:
sudo packet-sniffer eth0
```


**Steps to build**

1. Each tool may be individually compiled. For example, ftp-cli is compiled as shown below: 
   ```
   $ cd tools/ftp-cli
   $ make
   $ ./ftp-cli <ftp-url>
   ```

**Cross compiling(For raspberry pi)**

1. First setup sysroot for the target platform. You can create a sysroot using the help of this repo ->
   [irakr/RPi-Cross-Build-Environment](https://github.com/irakr/RPi-Cross-Build-Environment).
2. Set the following environment variables:  
   ```
   export SYSROOT_PATH=<path-to-sysroot>
   ```
3. Compile:
   ```
   $ make PLATFORM=raspi
   ```
4. To deploy the binaries, the TARGET_IP and TARGET_USER environment variables must be set first. Then run:
   ```
   $ make deploy
   ```
   By default, the binaries will be copied to `/home/$(TARGET_USER)/netwrk` in the target system.

