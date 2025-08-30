#!/bin/bash
# Copyright (c) 2025 Senthil Kumar
# Author: Senthil Kumar
# Permission required for commercial use. Please acknowledge the author.
# This program is free for personal and educational use.
# Contact: senthil4321 (GitHub)
# Unit tests for rsa_sign_verify.sh
set -e

TEST_LOG="test_rsa_sign_verify.log"
TEST_DIR="test_rsa"
KEYSIZE=2048
PRIVKEY="$TEST_DIR/test_private.pem"
PUBKEY="$TEST_DIR/test_public.pem"
DATAFILE="$TEST_DIR/test_data.txt"
SIGFILE="$TEST_DIR/test_data.sig"

mkdir -p "$TEST_DIR"
rm -f "$TEST_LOG" "$PRIVKEY" "$PUBKEY" "$DATAFILE" "$SIGFILE"

echo "Hello, RSA Test!" > "$DATAFILE"

# Generate key pair
SCRIPT_DIR="$(dirname "$0")"
echo "Generating RSA key pair..." | tee -a "$TEST_LOG"
"$SCRIPT_DIR/rsa_gen_keys.sh" $KEYSIZE "$PRIVKEY" "$PUBKEY"
if [ $? -eq 0 ]; then
    echo "Key generation: PASS" | tee -a "$TEST_LOG"
else
    echo "Key generation: FAIL" | tee -a "$TEST_LOG"
    exit 1
fi

# Sign the file
echo "Signing data file..." | tee -a "$TEST_LOG"
"$SCRIPT_DIR/rsa_sign_verify.sh" sign "$PRIVKEY" "$DATAFILE" "$SIGFILE"
if [ $? -eq 0 ]; then
    echo "Signing: PASS" | tee -a "$TEST_LOG"
else
    echo "Signing: FAIL" | tee -a "$TEST_LOG"
    exit 1
fi

# Verify the signature
echo "Verifying signature..." | tee -a "$TEST_LOG"
"$SCRIPT_DIR/rsa_sign_verify.sh" verify "$PUBKEY" "$DATAFILE" "$SIGFILE"
if [ $? -eq 0 ]; then
    echo "Verification: PASS" | tee -a "$TEST_LOG"
else
    echo "Verification: FAIL" | tee -a "$TEST_LOG"
    exit 1
fi

echo "All tests passed." | tee -a "$TEST_LOG"
exit 0
