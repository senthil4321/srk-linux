#!/bin/bash

# Variables
BINARY_FILE="print_time"
IMAGE_FILE="filesystem.squashfs"

# Check if binary file exists
if [ ! -f "$BINARY_FILE" ]; then
    echo "Binary file $BINARY_FILE not found!"
    exit 1
fi

# Create a temporary directory
TEMP_DIR=$(mktemp -d)

# Copy the binary file to the temporary directory
cp $BINARY_FILE $TEMP_DIR/

# Create SquashFS filesystem
mksquashfs $TEMP_DIR $IMAGE_FILE -comp gzip

# Clean up
rm -rf $TEMP_DIR

echo "SquashFS filesystem created as $IMAGE_FILE"
