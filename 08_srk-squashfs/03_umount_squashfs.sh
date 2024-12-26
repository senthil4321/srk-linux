#!/bin/bash

# Variables
MOUNT_POINT="/mnt/squashfs"

# Unmount the SquashFS filesystem
sudo umount $MOUNT_POINT

echo "SquashFS filesystem unmounted from $MOUNT_POINT"
