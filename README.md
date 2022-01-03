Demo code for the Sabertooth 2x12 V1.00 serial motor controller

Cobbled together from some old code, nothing terrably interesting to see here

To enable on a RasPi 3 (not tested on other models recently):
1. Put `enable_uart=1` in `/boot/config.txt`
2. Put `dtoverlay=pi3-disable-bt` in `/boot/config.txt`
3. Reboot

More info [here](https://raspberrypi.stackexchange.com/a/54766).
