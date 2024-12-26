#!/bin/bash

# Variables
IMAGE_FILE="filesystem.squashfs"
MOUNT_POINT="/mnt/squashfs"

# Create mount point if it doesn't exist
[ ! -d $MOUNT_POINT ] && sudo mkdir -p $MOUNT_POINT

# Mount the SquashFS filesystem
sudo mount -o loop $IMAGE_FILE $MOUNT_POINT

echo "SquashFS filesystem mounted at $MOUNT_POINT"
