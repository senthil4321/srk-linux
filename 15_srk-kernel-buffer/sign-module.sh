#!/bin/bash

MODULE_NAME="srk-kernel-buffer.ko"
PRIVATE_KEY="signing_key.priv"
PUBLIC_KEY="signing_key.x509"

# Generate a private key and self-signed public key
openssl req -new -x509 -newkey rsa:2048 -keyout $PRIVATE_KEY -out $PUBLIC_KEY -days 365 -nodes -subj "/CN=Kernel Module Signing"

# Sign the kernel module
/usr/src/linux-headers-$(uname -r)/scripts/sign-file sha256 $PRIVATE_KEY $PUBLIC_KEY $MODULE_NAME

echo "Module $MODULE_NAME signed with $PRIVATE_KEY and $PUBLIC_KEY"
