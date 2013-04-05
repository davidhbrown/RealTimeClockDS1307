My goal in creating yet another DS1307 library was to provide
easy access to some of the other functions I needed from the chip,
specifically its square wave output and its battery-backed RAM.

## Documentation
@todo Mostly comments in `RealTimeClockDS1307.h`

## Examples (in /examples folder)

- `RealTimeClockDS1307_Test.pde` allow you to turn the clock on/off,
set date/time, set 12/24h, [de]activate the square wave, and
read/write memory from the Serial Monitor.

- `RealTimeClockDS1307.fz` is a Fritzing breadboard layout showing
the basic hookup of the Sparkfun RTC module to an Arduino. Included
is an optional resistor+LED to show the square wave (note that it's
an open drain, so you hook up to it rather differently than, say, 
pin 13).

## Changelog

##### Version 0.95
* Reverse renaming of getDate() and setDate(), now getDay() is calling getDate() and setDay() is calling setDate()
* Readme improvements

##### Version 0.94
* changed getDate() to getDay() and setDate() to setDay()
* updated keywords.txt
* updated example

##### Version 0.93
* added keywords.txt for syntax highlighting

##### Version 0.92 RC
* Updated for Arduino 1.00; testing with Andreas Giemza (hurik)

##### Version 0.91
* added multi-byte read/write

##### Version 0.9 RC
* initial release

## Future
 - web page documentation

## Credits

Much thanks to John Waters and Maurice Ribble for their
earlier and very helpful work (even if I didn't wind up
using any of their code):

- [http://combustory.com/wiki/index.php/RTC1307_-_Real_Time_Clock](http://combustory.com/wiki/index.php/RTC1307_-_Real_Time_Clock)
- [http://www.glacialwanderer.com/hobbyrobotics/?p=12](http://www.glacialwanderer.com/hobbyrobotics/?p=12)

## Copyright

RealTimeClockDS1307 - library to control a DS1307 RTC module
Copyright (c) 2011 David H. Brown. All rights reserved

## License

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
