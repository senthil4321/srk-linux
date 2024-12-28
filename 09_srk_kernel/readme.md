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

## Trial1_28DEC24

After applying the omap configuration it is needed to run make menu config with `ARCH` and `CROSS_COMPILE`.
Somehow make detects changes in .config file. If there is no change it starts new manual configuration.

### Changes

1. Change Hostname
2. Change kernel Arguments
3. Change default init

### Backup file Location

01_backup/01_trial1/.config

### Procedure

```bash
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- omap2plus_defconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
```
