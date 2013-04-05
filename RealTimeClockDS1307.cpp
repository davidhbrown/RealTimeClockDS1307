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
 
/******************************************************************************
 * Includes
 ******************************************************************************/

#include "RealTimeClockDS1307.h"
#include <Wire.h>

/******************************************************************************
 * Definitions
 ******************************************************************************/

#define DS1307_I2C_ADDRESS 0x68  // This is the I2C address

/******************************************************************************
 * Constructors
 ******************************************************************************/

RealTimeClockDS1307::RealTimeClockDS1307()
{
  Wire.begin();
  //must NOT attempt to read the clock before
  //Wire.begin() has not been called; readClock() will hang.
  //Fortunately, it seems that you can call Wire.begin()
  //multiple times with no adverse effect).
} 

/******************************************************************************
 * User API
 ******************************************************************************/

/***** CHIP READ/WRITE ******/

void RealTimeClockDS1307::readClock()
{
  // Reset the register pointer
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write((uint8_t) 0x00);
  Wire.endTransmission();
 
  Wire.requestFrom(DS1307_I2C_ADDRESS, 8);
  _reg0_sec = Wire.read();
  _reg1_min = Wire.read();
  _reg2_hour = Wire.read();
  _reg3_day = Wire.read();
  _reg4_date = Wire.read();
  _reg5_month = Wire.read();
  _reg6_year = Wire.read();
  _reg7_sqw = Wire.read();
}

void RealTimeClockDS1307::setClock()
{
  //to be paranoid, we're going to first stop the clock
  //to ensure we don't have rollovers while we're
  //writing:
  writeData(0,0x80);
  //now, we'll write everything *except* the second
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write((uint8_t) 0x01);
  Wire.write(_reg1_min);
  Wire.write(_reg2_hour);
  Wire.write(_reg3_day);
  Wire.write(_reg4_date);
  Wire.write(_reg5_month);
  Wire.write(_reg6_year);
  Wire.endTransmission();
  //now, we'll write the seconds; we didn't have to keep
  //track of whether the clock was already running, because
  //_reg0_sec already knows what we want it to be. This
  //will restart the clock as it writes the new seconds value.
  writeData(0,_reg0_sec); 
}

void RealTimeClockDS1307::stop()
{
  //"Bit 7 of register 0 is the clock halt (CH) bit. 
  //When this bit is set to a 1, the oscillator is disabled."
  _reg0_sec = _reg0_sec | 0x80;
  writeData(0,_reg0_sec);
}
void RealTimeClockDS1307::start()
{
  //"Bit 7 of register 0 is the clock halt (CH) bit. 
  //When this bit is set to a 1, the oscillator is disabled."
  _reg0_sec = _reg0_sec & ~0x80;
  writeData(0,_reg0_sec);
}

void RealTimeClockDS1307::writeData(byte regNo, byte value)
{
  if(regNo > 0x3F) { return; }
   Wire.beginTransmission(DS1307_I2C_ADDRESS);
   Wire.write(regNo);
   Wire.write(value);
   Wire.endTransmission();
}

void RealTimeClockDS1307::writeData(byte regNo, void * source, int length)
{
  char * p = (char*) source;
  if(regNo > 0x3F || length > 0x3F) { return; }
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(regNo);
  for(int i=0; i<length; i++) {
    Wire.write(*p);
    p++;
  }
  Wire.endTransmission();
}

byte RealTimeClockDS1307::readData(byte regNo)
{
  if(regNo > 0x3F) { return 0xff; }
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(regNo);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS, 1);
  return Wire.read();
}

void RealTimeClockDS1307::readData(byte regNo, void * dest, int length)
{
  char * p = (char*) dest;
  if(regNo > 0x3F || length > 0x3F) { return; }
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(regNo);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS, length);
  for(int i=0; i<length; i++) {
    *p=Wire.read();
    p++;
  }
}


void RealTimeClockDS1307::sqwEnable(byte frequency)
{
  if(frequency > 3) { return; }
  //bit 4 is enable (0x10);
  //bit 7 is current output state if disabled
  _reg7_sqw = _reg7_sqw & 0x80 | 0x10 | frequency;
  writeData(0x07, _reg7_sqw);
}

void RealTimeClockDS1307::sqwDisable(boolean outputLevel)
{
  //bit 7 0x80 output + bit 4 0x10 enable both to zero, 
  //the OR with the boolean shifted up to bit 7
  _reg7_sqw = _reg7_sqw & ~0x90 | (outputLevel << 7);
  writeData(0x07, _reg7_sqw);
  //note: per the data sheet, "OUT (Output control): This bit controls 
  //the output level of the SQW/OUT pin when the square wave
  //output is disabled. If SQWE = 0, the logic level on the 
  //SQW/OUT pin is 1 if OUT = 1 and is 0 if OUT = 0."
  //"The SQW/OUT pin is open drain and requires an external 
  //pull-up resistor."
  //It is worth mentioning that on the Sparkfun breakout board,
  //BOB-00099, a LED connected to the SQW pin through a resistor to
  //Vcc+5V illuminated when OUT=0 and was dark when OUT=1, the 
  //opposite of what I expected until I remembered that it is 
  //an open drain (google it if you need to). Basically, they don't
  //so much mean a logic level (e.g., +3.3V rel Gnd) as they mean 
  //high or low *impeadance* to ground (drain). So High is basically
  //an open switch. Low connects to ground.
}


/***** GETTERS ******/

boolean RealTimeClockDS1307::is12hour() 
{
  //12-hour mode has bit 6 of the hour register set high
  return ((_reg2_hour & 0x40) == 0x40);
}
boolean RealTimeClockDS1307::isPM()
{
  //if in 12-hour mode, but 5 of the hour register indicates PM
  if(is12hour()) {
    return ((_reg2_hour & 0x20) == 0x20);
  }
  //otherwise, let's consider any time with the hour >11 to be PM:
  return (getHours() > 11);
}
boolean RealTimeClockDS1307::isStopped()
{
  //bit 7 of the seconds register stopps the clock when high
  return ((_reg0_sec & 0x80) == 0x80);
}

int RealTimeClockDS1307::getHours()
{
  if(is12hour()) { 
    //do not include bit 5, the am/pm indicator
    return bcdToDec(_reg2_hour & 0x1f);
  }
  //bits 4-5 are tens of hours
  return bcdToDec(_reg2_hour & 0x3f);
}
int RealTimeClockDS1307::getMinutes()
{
  //could mask with 0x7f but shouldn't need to
  return bcdToDec(_reg1_min);
}
int RealTimeClockDS1307::getSeconds()
{
  //need to mask oscillator start/stop bit 7
  return bcdToDec(_reg0_sec & 0x7f);
}
int RealTimeClockDS1307::getYear()
{
  return bcdToDec(_reg6_year);
}
int RealTimeClockDS1307::getMonth()
{
  //could mask with 0x1f but shouldn't need to
  return bcdToDec(_reg5_month);
}
int RealTimeClockDS1307::getDate()
{
  //could mask with 0x3f but shouldn't need to
  return bcdToDec(_reg4_date);
}
int RealTimeClockDS1307::getDay()
{
  return getDate();
}
int RealTimeClockDS1307::getDayOfWeek()
{
  //could mask with 0x07 but shouldn't need to
  return bcdToDec(_reg3_day);
}

void RealTimeClockDS1307::getFormatted(char * buffer)
{
  int i=0;
  //target string format: YY-MM-DD HH:II:SS
  buffer[i++]=highNybbleToASCII(_reg6_year);
  buffer[i++]=lowNybbleToASCII(_reg6_year);
  buffer[i++]='-';
  buffer[i++]=highNybbleToASCII(_reg5_month & 0x1f);
  buffer[i++]=lowNybbleToASCII(_reg5_month);
  buffer[i++]='-';
  buffer[i++]=highNybbleToASCII(_reg4_date & 0x3f);
  buffer[i++]=lowNybbleToASCII(_reg4_date);
  buffer[i++]=' ';
  if(is12hour()) {
    buffer[i++]=highNybbleToASCII(_reg2_hour & 0x1f);
  } else {
    buffer[i++]=highNybbleToASCII(_reg2_hour & 0x3f);
  }
  buffer[i++]=lowNybbleToASCII(_reg2_hour);
  buffer[i++]=':';
  buffer[i++]=highNybbleToASCII(_reg1_min & 0x7f);
  buffer[i++]=lowNybbleToASCII(_reg1_min);
  buffer[i++]=':';
  buffer[i++]=highNybbleToASCII(_reg0_sec & 0x7f);
  buffer[i++]=lowNybbleToASCII(_reg0_sec);
  if(is12hour()) {
    if(isPM()) {
      buffer[i++]='P';
    } else {
      buffer[i++]='A';
    }
  }
  buffer[i++]=0x00;
}

void RealTimeClockDS1307::getFormatted2k(char * buffer)
{
  buffer[0]='2';
  buffer[1]='0';
  getFormatted(&buffer[2]);
}

/**** SETTERS *****/

void RealTimeClockDS1307::setSeconds(int s)
{
  if (s < 60 && s >=0)
  {
    //need to preserve oscillator bit
    _reg0_sec = decToBcd(s) | (_reg0_sec & 0x80);
  }
}
void RealTimeClockDS1307::setMinutes(int m)
{
  if (m < 60 && m >=0)
  {
    _reg1_min = decToBcd(m);
  }
}
void RealTimeClockDS1307::setHours(int h)
{
  if (is12hour())
  {
    if (h >= 1 && h <=12)
    {
      //preserve 12/24 and AM/PM bits
      _reg2_hour = decToBcd(h) | (_reg2_hour & 0x60);
    }
  } else {
    if (h >= 0 && h <=24)
   {
      //preserve 12/24 bit
      _reg2_hour = decToBcd(h) | (_reg2_hour & 0x40);
   }
  }//else
}//setHours

void RealTimeClockDS1307::set24h()
{
  //"Bit 6 of the hours register is defined as the
  //"12- or 24-hour mode select bit.
  //"When high, the 12-hour mode is selected"
  //So, mask the curent value with the complement turn off that bit:
  _reg2_hour = _reg2_hour & ~0x40; 
}
void RealTimeClockDS1307::setAM()
{
  //"In the 12-hour mode, bit 5 is the AM/PM bit with logic high being PM"
  //so we need to OR with 0x40 to set 12-hour mode and also
  //turn off the PM bit by masking with the complement
  _reg2_hour = _reg2_hour & ~0x20 | 0x40;
}
void RealTimeClockDS1307::setPM()
{
  //"In the 12-hour mode, bit 5 is the AM/PM bit with logic high being PM"
  //so we need to OR with 0x40 and 0x20 to set 12-hour mode and also
  //turn on the PM bit:
  _reg2_hour = _reg2_hour | 0x60;
}

void RealTimeClockDS1307::switchTo12h()
{
  if(is12hour()) { return; }
  int h = getHours();
  if (h < 12) {
    setAM();
  } else {
    h = h-12;
    setPM();
  }
  if (h==0)
  {
    h=12;
  }
  setHours(h);
}
void RealTimeClockDS1307::switchTo24h()
{
  if(!is12hour()) { return ; }
  int h = getHours();
  if(h==12) {//12 PM is just 12; 12 AM is 0 hours.
    h = 0;
  }
  if (isPM())
  {//if it was 12 PM, then h=0 above and so we're back to 12:
    h = h+12;
  }
  set24h();
  setHours(h);
}

void RealTimeClockDS1307::setDayOfWeek(int d)
{
  if (d > 0 && d < 8)
  {
    _reg3_day = decToBcd(d);
  }
}

void RealTimeClockDS1307::setDate(int d)
{
  if (d > 0 && d < 32)
  {
    _reg4_date = decToBcd(d);
  }
}
void RealTimeClockDS1307::setDay(int d)
{
  setDate(d);
}

void RealTimeClockDS1307::setMonth(int m)
{
  if (m > 0 && m < 13)
  {
    _reg5_month = decToBcd(m);
  }
}
void RealTimeClockDS1307::setYear(int y)
{
  if (y >= 0 && y <100)
  {
    _reg6_year = decToBcd(y);
  }
}



/*****************************************
 * Private methods
 *****************************************/
byte RealTimeClockDS1307::decToBcd(byte b)
{
  return ( ((b/10) << 4) + (b%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte RealTimeClockDS1307::bcdToDec(byte b)
{
  return ( ((b >> 4)*10) + (b%16) );
}

char RealTimeClockDS1307::lowNybbleToASCII(byte b) 
{
  b = b & 0x0f;
  if(b < 10) {
    //0 is ASCII 48
    return 48+b;
  }
  //A is ASCII 55
  return 55+b;
}
char RealTimeClockDS1307::highNybbleToASCII(byte b)
{
  return lowNybbleToASCII(b >> 4);
}

/***** INSTANCE *******/

RealTimeClockDS1307 RTC = RealTimeClockDS1307();
