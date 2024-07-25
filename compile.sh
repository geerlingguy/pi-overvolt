#!/bin/bash
gcc -I`echo /usr/src/linux-headers-*common-rpi/include/linux/broadcom/` main.c -o removelimit
