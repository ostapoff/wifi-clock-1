# wifi-clock-1
Redevelopment of the original DIY clock kit [bought at Aliexpress](https://aliexpress.com/item/32972008419.html) on ESP-8266 MC.

![photo](./assets/clock_photo.jpeg?raw=true)

Components:
- original acrylic box and indicators
- [ESP-8266 D1 Mini board](https://aliexpress.com/item/32958591238.html)
- [MAX7221 7 segments 8 digits display driver](https://aliexpress.com/item/32965053259.html)
- [2 right angle 6x6x9mm push buttons](https://aliexpress.com/item/32901481925.html)

Original led 7 segments indicators have common anode which is a bit tricky to program the MAX7221 driver for. Top lid was removed and SMD leds were resoldered with a common cathode.

ESP-8266 supports WiFi allowing the clock to syncronize via NTP.

## electronic circuit

![scheme](./assets/clock_schematic.png?raw=true)

## PCB layout

![pcb](./assets/clock_pcb.png?raw=true)

