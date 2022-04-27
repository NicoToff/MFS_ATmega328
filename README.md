[mfs.png](https://i.postimg.cc/VLrQ106B/mfs.png)
# MFS_ATmega328
Lightweight library for the use of a Multifunction Shield with an Arduino Uno

## Port Register Manipulation
This library is *lightweigth* because it makes extensive use of port register manipulations. In a nutshell, port manipulation is the closest thing there is to working directly with Assembly code. A few methods in this library even use inline Assembly code!

## Why use this library?
If you're running out of RAM or Flash space in your Arduino project, this library might help you shave off precious bytes and continue coding.

## Why should I avoid this library?
Port manipulation is done directly on the registers on the ATmega328 chip of the Arduino Uno. For this reason, this library **is not compatible** with any other microcontrollers. It's also incomplete when it comes to using the MFS to its full extent. I wholeheartedly recommend [Antonio Vanegas's "MultiFuncShield" library](https://github.com/hpsaturn/MultiFuncShield-Library/tree/master/src) which provides everything you need.

### List of compatible boards
- All boards with ATmega328 or variants (ATmega328P, ATmega328PU...)
