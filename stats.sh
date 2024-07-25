#!/bin/bash

current_clock=$(vcgencmd measure_clock arm)
current_voltage=$(vcgencmd measure_volts core)
current_temp=$(vcgencmd measure_temp)
uptime=$(uptime)

printf "Clock: ${current_clock#*=}\n"
printf "Volts: ${current_voltage#*=}\n"
printf "Temp: ${current_temp#*=}\n"
printf "Uptime: ${uptime}\n"
