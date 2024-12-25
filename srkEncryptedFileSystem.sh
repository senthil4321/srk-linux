#!/bin/bash

# Variables
IMAGE_FILE="encrypted.img"
MAPPER_NAME="encrypted_device"
MOUNT_POINT="/mnt/encrypted"
IMAGE_SIZE_MB=100

# Create a local image file
dd if=/dev/zero of=$IMAGE_FILE bs=1M count=$IMAGE_SIZE_MB

# Format the image file with LUKS using CBC mode
sudo cryptsetup luksFormat --cipher aes-cbc-essiv:sha256 $IMAGE_FILE

# Open the encrypted image file
sudo cryptsetup luksOpen $IMAGE_FILE $MAPPER_NAME

# Create a filesystem on the encrypted device
sudo mkfs.ext4 /dev/mapper/$MAPPER_NAME

# Create mount point if it doesn't exist
[ ! -d $MOUNT_POINT ] && sudo mkdir -p $MOUNT_POINT

# Mount the encrypted device
sudo mount /dev/mapper/$MAPPER_NAME $MOUNT_POINT

echo "Encrypted file system mounted at $MOUNT_POINT"

# To unmount and close the encrypted device, use:
# sudo umount $MOUNT_POINT
# sudo cryptsetup luksClose $MAPPER_NAME
