TotWaterPark
============

This program uses an ATmega 328p to control an inexpensive DC bilge pump to turn an inflatable pool into a
miniature water park!

##Features:
* Timer to turn pump off after two minutes
* Low battery lockout to prevent overdischarge of battery

##Compiling:
The source can be compiled using the make utility.  Enter the source directory and type `make`.

##Flashing:
To flash to the microcontroller edit the following line in the make file to match your programming device, it's path, and it's baud rate.
```
PROGRAMMER = -c arduino -P /dev/ttyUSB0 -b57600
```
In this example the device is using the arduino bootloader and is connected to a USB to serial converter at `/dev/ttyUSB0`.  Once you have updated the make file type `make flash`
