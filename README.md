# MFS_Arduino_Uno
Lightweight library for the use of a Multifunction Shield with an Arduino Uno

## Port Register Manipulation
This library is *lightweigth* because it makes extensive use of port register manipulation. In a nutshell, port manipulation is the closest thing there is to working directly with Assembly code.

## Why use this library?
If you're running out of RAM or Flash space in your Arduino project, this library might help you shave off precious bytes and continue coding.

## Why should I avoid this library?
Port manipulation is done directly on the registers on the ATmega328P chip of the Arduino Uno. For this reason, this library **is not compatible** with any other microcontrollers.
