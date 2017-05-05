/**
 * @file IanFIFOII.h
 * head file of IanFIFO class
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
 */

 /**
  * conect pins to FIFO II (FIFO I doesn't need pins)
  * _fifoPin: output
  *    1: to select I2S1, default
  *    0: to select I2S2
  * _i2s1Pin,_i2s2Pin: input, will be programmed pullup internally in Arduino  
  *     1        0   :i2s1 is selected
  *     0        1   :i2s2 is selected
  *     1        1   :signal disconnected or is working with FIFO I
  *     0        0   :wrong signal
  *     
  * Connect pins to S/PDIF  board (no connection is needed if there is no spdif board)
  * _spdifPin: output
  *    200ms or longer negative pulse switch to next spdif source
  * _coxPin,_optPin,_ttlPin: input, will be programmed pullup internally in Arduino    
  *    1       0       0   : BNC     input is currentlly selected
  *    0       1       0   : optical input is currentlly selected (default)
  *    0       0       1   : ttl     input is currentlly selected 
  *    1       1       1   : i2s backdoor input is currentlly selected, or disconnection
  *    other status        : wrong signals  
  */

#ifndef IanFIFO_H_
#define IanFIFO_H_

#include <arduino.h>


typedef enum
{
  FIFOUNKNOWN=0,   //unknown status
  FIFOI2S1,        //FIFO II i2s1 is selected
  FIFOI2S2,        //FIFO II i2s2 or S/PDIF board is selected
  FIFOIIDSCONNECT, //FIFO II is disconnected,also migh be FIFO I      
} fifoSource_t;

typedef enum
{
  SPDIFUNKNOWN=0,  //unknown status
  OPT,             //spdif opt is selected 
  COX,             //spdif cox is selected 
  TTL,             //spdif ttl is disconnected 
  BKD,             //spdif backdoor i2s is selected  
} spdifSource_t;

class CIanFIFO
{
  int _fifoPin,_i2s1Pin,_i2s2Pin,_spdifPin,_coxPin,_optPin,_ttlPin;
  int fifoPinStatus;                                   //to save _fifiPin status each time it is updated, this is done because Arduino DUE cannot read back pins
  fifoSource_t fifoSource,oldfifoSource;
  spdifSource_t spdifSource,oldspdifSource;
  int fifoChangedFlag=0;                               //indicating source status has been changed if set,cleared as default
  int spdifChangedFlag=0;                              //indicating source status has been changed if set,cleared as default
  int spdifLaunchTime=0;                               //if set, to launch negative pulse on _spdifPin, disable as default
  fifoSource_t readFifoPins();                         //read fifo source from _i2s1Pin and _i2s2Pin
  spdifSource_t readSpdifPins();                       //read spdif source from _optPin,_coxPin and _ttlPin
public:
  CIanFIFO(int fifoPin,int i2s1Pin,int i2s2Pin,int spdifPin,int coxPin,int optPin,int ttlPin); //constructor
  void scan();                                         //scan fifo and spdif, flags will be set if there is any change
  int checkFifoFlag();                                 //check if source status has been changed, flag will be cleared automactically after check
  int checkSpdifFlag();                                //check if source status has beenchanged, flag will be cleared automactically after check
  void launchSpdifPin(int time){spdifLaunchTime=time;};//time in 10ms
  void setFifoPin(int logic);                          //to set _fifoPin to 0 or 1
  int getFifoPin(){return fifoPinStatus;};             //return _fifoPin status, 1 for I2S1, 0 for I2S2
  fifoSource_t getFifoSource(){return oldfifoSource;};
  spdifSource_t getSpdifSource(){return oldspdifSource;};
};

#endif  //IanFIFO_H_
