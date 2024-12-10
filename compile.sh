#!/bin/bash

# Find the latest linux-headers directory with "+rpt-common-rpi"
TARGET_DIR=$(ls -d /usr/src/linux-headers-*+rpt-common-rpi 2>/dev/null | sort -V | tail -n 1)

# Check if a matching directory was found
if [ -n "$TARGET_DIR" ] && [ -d "$TARGET_DIR/include/linux/broadcom" ]; then
    echo "Using Broadcom headers from: $TARGET_DIR/include/linux/broadcom"
    gcc -I"$TARGET_DIR/include/linux/broadcom" main.c -o removelimit
else
    echo "Error: No suitable Broadcom headers directory found in /usr/src/"
    exit 1
fi
