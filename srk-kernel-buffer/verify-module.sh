#!/bin/bash

MODULE_NAME="srk-kernel-buffer.ko"

# Verify the kernel module signature
modinfo -F signer $MODULE_NAME
modinfo -F sig_key $MODULE_NAME
modinfo -F sig_hashalgo $MODULE_NAME
