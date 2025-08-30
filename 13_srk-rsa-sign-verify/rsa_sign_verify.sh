#!/bin/bash
# Copyright (c) 2025 Senthil Kumar
# Author: Senthil Kumar
# Permission required for commercial use. Please acknowledge the author.
# This program is free for personal and educational use.
# Contact: senthil4321 (GitHub)
# Version: 1.0.0
# RSA Sign and Verify Script
VERSION="1.0.0"
LOGFILE="rsa_sign_verify.log"

echo_log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a "$LOGFILE"
}

usage() {
    echo "Usage: $0 sign|verify <key> <input> <output>"
    exit 1
}

if [ "$#" -lt 4 ]; then
    usage
fi

MODE="$1"
KEY="$2"
INPUT="$3"
OUTPUT="$4"

case "$MODE" in
    sign)
        openssl dgst -sha256 -sign "$KEY" -out "$OUTPUT" "$INPUT"
        RET=$?
        if [ $RET -eq 0 ]; then
            echo_log "Signed $INPUT with $KEY to $OUTPUT (Version: $VERSION)"
        else
            echo_log "Signing failed for $INPUT with $KEY"
        fi
        exit $RET
        ;;
    verify)
        openssl dgst -sha256 -verify "$KEY" -signature "$OUTPUT" "$INPUT"
        RET=$?
        if [ $RET -eq 0 ]; then
            echo_log "Verified $INPUT with $KEY and $OUTPUT (Version: $VERSION)"
        else
            echo_log "Verification failed for $INPUT with $KEY and $OUTPUT"
        fi
        exit $RET
        ;;
    *)
        usage
        ;;
esac
