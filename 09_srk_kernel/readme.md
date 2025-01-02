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

Get the kernel command line


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

1. Change Hostname - ?
2. Change Kernel Arguments - ?
3. Change default init /sbin/hello - OK

### Backup file Location

01_backup/01_trial1/.config

### Procedure

```bash
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- omap2plus_defconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
```

## Trial2_28DEC24

Change the build output directory and use cache to speed up build.

### Changes

1. Change default init to /sbin/hello2

### Backup file Location

01_backup/01_trial1/.config

### Procedure

Mechanism to speed up rebuilds

```bash
#to clean the old build Config files, backup, output directory
make ARCH=arm mrproper
make ARCH=arm CROSS_COMPILE="ccache arm-linux-gnueabihf-" O=~/project/srk-1-linux-build/ omap2plus_defconfig
make ARCH=arm CROSS_COMPILE="ccache arm-linux-gnueabihf-" O=~/project/srk-1-linux-build/ menuconfig
make ARCH=arm CROSS_COMPILE="ccache arm-linux-gnueabihf-" O=~/project/srk-1-linux-build/ -j$(nproc)
```

#### Export

```bash
export PATH="/usr/lib/ccache:$PATH"
#not used
export CROSS_COMPILE="ccache arm-linux-gnueabihf-" O=/path/to/output-directory -j$(nproc)
```

## Trial3_29DEC24

### Changes

1. CONFIG_LOCALVERSION - srk-release-3
    - Linux 192.168.1.200 6.6.67srk-release-3 #2 SMP Sun Dec 29 17:20:22 CET 2024 armv7l GNU/Linux - OK
2. CONFIG_DEFAULT_INIT - /sbin/hello3
    - OK
3. CONFIG_DEFAULT_HOSTNAME - srk3

### Procedure

```bash
make ARCH=arm CROSS_COMPILE="ccache arm-linux-gnueabihf-" O=~/project/srk-1-linux-build/ xconfig
make ARCH=arm CROSS_COMPILE="ccache arm-linux-gnueabihf-" O=~/project/srk-1-linux-build/ -j$(nproc)
```

## Trial4_02JAN25

### Changes

1. Disable IP6 support

### Procedure

```bash
make ARCH=arm CROSS_COMPILE="ccache arm-linux-gnueabihf-" O=~/project/srk-1-linux-build/ xconfig
export PATH="/usr/lib/ccache:$PATH"
make ARCH=arm CROSS_COMPILE="ccache arm-linux-gnueabihf-" O=~/project/srk-1-linux-build/ -j$(nproc)
```

Backup Commands

```bash
scp srk2cob@192.168.0.216:~/project/srk-1-linux-build/.config ./
scp srk2cob@192.168.0.216:~/project/srk-1-linux-build/arch/arm/boot/zImage ./output/

```

---

## Linux target commands

### How to check the current linux command line in the running system?

```bash
mount -t proc proc /proc
cat /proc/cmdline
dmesg | grep "Command line"
sysctl -a 

```

### What is systctl?

Utility to view and modify linux parameters at runtime.

```bash
#command to view current kernel parameters
sysctl -a 
```

### How to get the current config from running system?

```bash
zcat /proc/config.gz  | grep '=y$' | wc -l 
```

### TODO

-[ ] How to disable sysctl ?
-[ ] How to disable viewing linux command line ?
-[ ] Script to mount `proc` automatically at boot
    use /etc/fstab
