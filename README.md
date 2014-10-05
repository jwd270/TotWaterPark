TotWaterPark
============

This program uses an ATmega 328p to control an inexpensive DC bilge pump to turn an inflatable pool into a
miniature water park!

The circuit diagram and PCB layout files are comming.

###Features:
* Timer to turn pump off after two minutes
* Low battery lockout to prevent over discharge of battery

###Compiling:
The source can be compiled using the make utility.  Enter the source directory and use the command `make` to build the program.

###Programming:
To flash to the micro-controller edit the following line in the make file to match your programming device, it's path, and it's baud rate.
```
PROGRAMMER = -c arduino -P /dev/ttyUSB0 -b57600
```
In this example, the device is using the arduino boot loader and is connected to a USB to serial converter at `/dev/ttyUSB0`.  Once you have updated the make file use the command `make flash` to flash the micro-controller.

