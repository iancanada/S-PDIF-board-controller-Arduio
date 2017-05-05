/**
 * @file IanFIFOII.cpp
 * implementation file of IanFIFO class
 * Copyright (C) Ian Jin 
 * iancanada.mail@gmail.com
 * Twitter: @iancanadaTT
 * 
 * 2015-08-29 First release Ver1.00
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * 
 */

 #include "IanFIFOII.h"

CIanFIFO::CIanFIFO(int fifoPin,int i2s1Pin,int i2s2Pin,int spdifPin,int coxPin,int optPin,int ttlPin)
{
  _fifoPin=fifoPin; pinMode(_fifoPin,OUTPUT); setFifoPin(1);
  _i2s1Pin=i2s1Pin; pinMode(_i2s1Pin,INPUT_PULLUP);
  _i2s2Pin=i2s2Pin; pinMode(_i2s2Pin,INPUT_PULLUP);
  _spdifPin=spdifPin; pinMode(_spdifPin,OUTPUT); digitalWrite(_spdifPin,HIGH);
  _optPin=optPin; pinMode(_optPin,INPUT_PULLUP);
  _coxPin=coxPin; pinMode(_coxPin,INPUT_PULLUP);
  _ttlPin=ttlPin; pinMode(_ttlPin,INPUT_PULLUP);  

  oldfifoSource=FIFOIIDSCONNECT;   //disconnect as default,fifo source status will be updated if it's not disconnect
  oldspdifSource=BKD;              //backdoor as default which has same signal status as disconncet,spdif source status will be updated if not disconnect  
}

void CIanFIFO::scan()
{
  fifoSource=readFifoPins();
  if(fifoSource!=oldfifoSource)    //fifo II source changed 
  {
    oldfifoSource=fifoSource;      //save fifoSource
    fifoChangedFlag=1;             //set fifo source changed flag
  }
  
  spdifSource=readSpdifPins();
  if(spdifSource!=oldspdifSource)  //spdif source changed
  {
    oldspdifSource=spdifSource;    //save spdifSource
    spdifChangedFlag=1;            //set spdif source changed flag
  } 

  if(spdifLaunchTime)              //run negative pulse on spdif pin if set
  {
    digitalWrite(_spdifPin,LOW);
    if(--spdifLaunchTime==0)       //time is up
      digitalWrite(_spdifPin,HIGH);//finish pulse
  }
}

int CIanFIFO::checkFifoFlag()
{
  int flagToReturn=fifoChangedFlag;
  fifoChangedFlag=0;               //clear flag unconditionally
  return flagToReturn;
}

int CIanFIFO::checkSpdifFlag()
{
  int flagToReturn=spdifChangedFlag;
  spdifChangedFlag=0;              //clear flag unconditionally
  return flagToReturn;
}

void CIanFIFO::setFifoPin(int logic)
{
  if(logic==0)
  {
    digitalWrite(_fifoPin,LOW);
    fifoPinStatus=0;
  }
  else
  {
    digitalWrite(_fifoPin,HIGH);
    fifoPinStatus=1;
  }  
}

fifoSource_t CIanFIFO::readFifoPins()
{
  int pin1=digitalRead(_i2s1Pin);
  int pin2=digitalRead(_i2s2Pin);
  
  if((pin1==HIGH)&&(pin2==LOW))
    return FIFOI2S1;
  else if((pin1==LOW)&&(pin2==HIGH))
    return FIFOI2S2;
  else if((pin1==HIGH)&&(pin2==HIGH))
    return FIFOIIDSCONNECT;
  else
    return FIFOUNKNOWN;  
}

spdifSource_t CIanFIFO::readSpdifPins()
{
  int pin1=digitalRead(_optPin);
  int pin2=digitalRead(_coxPin);
  int pin3=digitalRead(_ttlPin);
  
  if((pin1==HIGH)&&(pin2==LOW)&&(pin3==LOW))
    return OPT;
  else if((pin1==LOW)&&(pin2==HIGH)&&(pin3==LOW))
    return COX;
  else if((pin1==LOW)&&(pin2==LOW)&&(pin3==HIGH))
    return TTL;
  else if((pin1==HIGH)&&(pin2==HIGH)&&(pin3==HIGH))
    return BKD;  
  else
    return SPDIFUNKNOWN;  
}




