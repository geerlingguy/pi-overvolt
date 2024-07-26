# Pi Overvolt - Remove voltage limits from Pi 5

> [!CAUTION]
> **USE AT YOUR OWN RISK**: This software removes voltage limits set in the Raspberry Pi firmware, presumably to prevent the SoC from frying itself. Use of this software can and most probably _will_ cause damage to your Raspberry Pi.
> 
> _You have been warned._

## History

This project was initially conceived by [@jonatron](https://github.com/jonatron), and the first tests were written up in this blog post: [Beating Jeff's 3.14 GHz Raspberry Pi 5](https://jonatron.github.io/randomstuff/pivolt/).

The Raspberry Pi 5's bootloader / firmware contains a hard-coded 1.000V limit, and because the firmware is signed by Broadcom/Raspberry Pi, you can't patch the bootloader itself. But you _can_ patch the system memory once booted, and that's exactly what the code in this project does.

That does create a winkle, however: the `arm_freq` setting, which controls the CPU clocks, has to be set to the overclock value we're aiming for, but most Pi 5's are unstable at clocks above 3.0-3.2 GHz with the default 1.000V limits. (Some Raspberry Pi 5's can't even hit 2.6 or 2.8 GHz reliably!)

Part of the setup process involves _downclocking_ the Pi during boot as early as possible, so it will boot with a stable clock. Then, once the voltage limit is raised, we can lift the clock limit.

## Setup

### Clone and build `removelimit`

Clone this repository and compile the app:

```
git clone https://github.com/geerlingguy/pi-overvolt.git && cd pi-overvolt
./compile.sh
```

This will generate a `removelimit` binary in the pi-overvolt directory.

### Create `slowcpu` systemd service

We need to slow down the Pi's clock as early in the boot process as possible, since the voltage limit is in place. Do that with the `slowcpu` service that runs on boot.

Copy the `slowcpu.service` file into place:

```
sudo cp slowcpu.service /lib/systemd/system/slowcpu.service
```

Then copy the `slowcpu` script into place:

```
sudo cp slowcpu /slowcpu
```

Enable the new systemd unit so it will run on boot:

```
sudo systemctl enable slowcpu.service
```

## Usage

Try rebooting your Pi with different `over_voltage_delta` and `arm_freq` values, increasing each beyond `50000` and `3000` respectively (the default `arm_freq` is `2400`—not all Pi 5's can even overclock beyond 2.8 GHz!):

```
over_voltage_delta=55000
arm_freq=3300
```

> [!CAUTION]
> **THERE IS NO TURNING BACK**! You can easily fry your Pi. Do not proceed unless that's a sacrifice you're willing to make!

When you're ready to run a benchmark, remove the voltage limit, flush the GPU's cache, set the frequency limit to your maximum frequency, and enable the `performance` governor to ensure the Pi doesn't throttle its clock at idle:

```
cd pi-overvolt && sudo ./removelimit && vcgencmd cache_flush
echo 3500000 | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_max_freq
echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
```

## Tips / Debugging

  - Check core voltage: `vcgencmd measure_volts core`
  - Check core frequency: `vcgencmd measure_clock arm`
  - Check CPU temp: `vcgencmd measure_temp`
  - Check max frequency: `/sys/devices/system/cpu/cpu*/cpufreq/scaling_max_freq`

While running these tests, I run `watch -n 1 ./stats.sh` using the file included in this repository to monitor clocks, voltage, temps, and uptime while running benchmarks.

## Records

### Geekbench

| Date | User | Single Core | Multicore | Settings | Link |
| --- | --- | --- | --- | --- | --- |
| 2024-07-25 | geerlingguy | 1121 | 2219 | `over_voltage_delta=70000`, `arm_freq=3400`, `gpu_freq=1100` | [result](https://browser.geekbench.com/v6/cpu/7058700) |

Sorted list of Pi 5 Geekbench 6 results: [Single core](https://browser.geekbench.com/v6/cpu/search?dir=desc&q=Raspberry+Pi+5+Model+B&sort=score) | [Multi core](https://browser.geekbench.com/v6/cpu/search?dir=desc&q=Raspberry+Pi+5+Model+B&sort=multicore_score)
