# netwrk
A collection of useful cross platform networking tools.

**Steps to build**

1. Each tool may be individually compiled. For example, ftp-cli is compiled as shown below: 
   ```
   $ cd tools/ftp-cli
   $ make
   ```

**Cross compiling(Example: For raspberry pi)**

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
   By default, the binaries will be under to `/opt/netwrk` in the target system.

