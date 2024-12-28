# Kernel Compiling

## Command

To open the graphical configuration menu for the Linux kernel, use the following command:

```bash
make menuconfig
```

To open the graphical configuration menu with a GUI, use the following command:

```bash
make xconfig
```

To compile kernel with omap2plus_defconfig

```bash
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-

make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- help
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- kernelversion
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- omap2plus_defconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j$(nproc)
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- dtbs -j$(nproc)
```
