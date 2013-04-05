/*
  RealTimeClockDS1307 - library to control a DS1307 RTC module
  Copyright (c) 2011 David H. Brown. All rights reserved

  v0.92 Updated for Arduino 1.00; not re-tested on earlier versions
  
  Much thanks to John Waters and Maurice Ribble for their
  earlier and very helpful work (even if I didn't wind up
  using any of their code):
   - http://combustory.com/wiki/index.php/RTC1307_-_Real_Time_Clock
   - http://www.glacialwanderer.com/hobbyrobotics/?p=12

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
*/

#ifndef RealTimeClockDS1307_h
#define RealTimeClockDS1307_h

  #if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #else
  #include "WProgram.h"
  #endif

//#include <HardwareSerial.h>
//#include <WConstants.h> //need/want 'boolean' and 'byte' types used by Arduino
//#undef round is required to avoid a compile-time
//"expected unqualified-id before 'double'" error in math.h
//see: http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1247924528/3
#undef round 
#include <Wire.h>

#define ARDUINO_PIN_T uint8_t

class RealTimeClockDS1307
{
  private:
    byte _reg0_sec;
    byte _reg1_min;
    byte _reg2_hour;
    byte _reg3_day;
    byte _reg4_date;
    byte _reg5_month;
    byte _reg6_year;
    byte _reg7_sqw;
    byte decToBcd(byte);
    byte bcdToDec(byte);
    char lowNybbleToASCII(byte);
    char highNybbleToASCII(byte);
  public:
    RealTimeClockDS1307();
    void readClock();//read registers (incl sqw) to local store
    void setClock();//update clock registers from local store
    void stop();//immediate; does not require setClock();
    void start();//immediate; does not require setClock();
    void sqwEnable(byte);//enable the square wave with the specified frequency
    void sqwDisable(boolean);//disable the square wave, setting output either high or low
    void writeData(byte, byte);//write a single value to a register
    void writeData(byte, void *, int);//write several values consecutively
    byte readData(byte);//read a single value from a register
    void readData(byte, void *, int);//read several values into a buffer

    int getHours();
    int getMinutes();
    int getSeconds();
    int getYear();
    int getMonth();
    int getDate();
    int getDay();
    int getDayOfWeek();
    boolean is12hour();
    boolean isPM();
    boolean isStopped();
    //getFormatted writes into a char array provided by you. Format is:
    // YY-MM-DD HH:II:SS   ... plus "A" or "P" if in 12-hour mode
    //and of course a NULL terminator. So, [18] for 24h or [19] for 12h
    void getFormatted(char *);//see comment above
    void getFormatted2k(char *);//as getFormatted, but with "20" prepended

    //must also call setClock() after any of these
    //before next readClock(). Note that invalid dates are not
    //corrected by the clock. All the clock knows is when it should 
    //roll over to the next month rather than the next date in the same month.
    void setSeconds(int);
    void setMinutes(int);
    //setHours rejects values out of range for the current 12/24 mode
    void setHours(int);
    void setAM();//does not consider hours; see switchTo24()
    void setPM();//does not consider hours; see switchTo24()
    void set24h();//does not consider hours; see switchTo24()
    void switchTo24h();//returns immediately if already 24h
    void switchTo12h();//returns immediately if already 12h
    void setDayOfWeek(int);//incremented at midnight; not set by date (no fixed meaning)
    void setDate(int);//allows 1-31 for *all* months.
    void setDay(int);
    void setMonth(int);
    void setYear(int);

    //squarewave frequencies:
    static const byte SQW_1Hz=0x00;
    static const byte SQW_4kHz=0x01;//actually 4.096kHz
    static const byte SQW_8kHz=0x02;//actually 8.192kHz
    static const byte SQW_32kHz=0x03;//actually 32.768kHz
};

extern RealTimeClockDS1307 RTC;

#endif
