#!/bin/bash

current_clock=$(vcgencmd measure_clock arm)
requested_voltage=$(vcgencmd measure_volts core)
# On the Pi 5, measure_volts reports the voltage the SoC requests. The PMIC caps
# its VDD_CORE output, so read the delivered voltage from the PMIC ADC when available.
pmic_voltage=$(vcgencmd pmic_read_adc VDD_CORE_V 2>/dev/null | grep -m1 'VDD_CORE_V')
current_temp=$(vcgencmd measure_temp)
uptime=$(uptime)

printf "Clock: ${current_clock#*=}\n"
if [ -n "$pmic_voltage" ]; then
  printf "Volts (PMIC VDD_CORE): ${pmic_voltage#*=}\n"
  printf "Volts (SoC requested): ${requested_voltage#*=}\n"
else
  printf "Volts: ${requested_voltage#*=}\n"
fi
printf "Temp: ${current_temp#*=}\n"
printf "Uptime: ${uptime}\n"
