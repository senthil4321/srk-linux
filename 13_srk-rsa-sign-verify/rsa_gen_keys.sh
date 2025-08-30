#!/bin/bash
# Copyright (c) 2025 Senthil Kumar
# Author: Senthil Kumar
# Permission required for commercial use. Please acknowledge the author.
# This program is free for personal and educational use.
# Contact: senthil4321 (GitHub)
# Version: 1.0.0
# RSA Key Pair Generation Script
VERSION="1.0.0"
LOGFILE="rsa_gen_keys.log"

echo_log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a "$LOGFILE"
}

usage() {
    echo "Usage: $0 <keysize> <private_key> <public_key>"
    exit 1
}

if [ "$#" -lt 3 ]; then
    usage
fi

KEYSIZE="$1"
PRIVKEY="$2"
PUBKEY="$3"

openssl genpkey -algorithm RSA -out "$PRIVKEY" -pkeyopt rsa_keygen_bits:$KEYSIZE
RET=$?
if [ $RET -eq 0 ]; then
    openssl rsa -pubout -in "$PRIVKEY" -out "$PUBKEY"
    RET=$?
    if [ $RET -eq 0 ]; then
        echo_log "Generated RSA key pair ($KEYSIZE bits): $PRIVKEY, $PUBKEY (Version: $VERSION)"
    else
        echo_log "Failed to extract public key from $PRIVKEY"
    fi
else
    echo_log "Failed to generate private key $PRIVKEY"
fi
exit $RET
