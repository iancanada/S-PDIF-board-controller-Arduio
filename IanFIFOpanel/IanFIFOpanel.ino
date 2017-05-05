/**
 * @Arduino Ian FIFO  controller
 *
 * Copyright (C) Ian Jin
 * iancanada.mail@gmail.com
 * Twitter: @iancanadaTT
 *
 * 2015-08-30 First release Ver1.00
 *
 * Tested on Arduino Uno and MEGA 2560
 * need a DRF0009 LCD KeyPad shield
 * works with:
 * 1. FIFO II with/without a S/PDIF board
 * 2. FIFO I with a S/PDIF board
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
  * conect to Ian FIFO KIT
  * DFR0009 PINS    Ian FIFO KIT PINS       SIGNAL NAME  INPUT/OUTPUT      NOTE
  *     D0 ---------- JUMPER1 (FIFO II)       fifoPin      OUTPUT          not connected for FIFO I
  *     D1 ---------- JUMPER2 (FIFO II)       i2s1Pin      INPUT PULLUP    not connected for FIFO I
  *     D2 ---------- JUMPER3 (FIFO II)       i2s2Pin      INPUT PULLUP    not connected for FIFO I
  *     D3 ---------- K1+     (S/PDIF board)  spdifPin     OUTPUT          not connected if there is no S/PDIF board
  *     D11 --------- COX+    (S/PDIF board)  coxPin       INPUT PULLUP    not connected if there is no S/PDIF board
  *     D12 --------- OPT+    (S/PDIF board)  optPin       INPUT PULLUP    not connected if there is no S/PDIF board
  *     D13 --------- TTL+    (S/PDIF board)  ttlPin       INPUT PULLUP    not connected if there is no S/PDIF board
  *
  *     GND --------- JUMPER1GND(FIFO II) or COX-(S/PDIF board)        has to be connected in either case
  *
  *     see IanFIFO.h for more details
  */

#include "IanArduinoRTOS.h"
#include "IanFIFOII.h"
#include "DFR0009.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>

CIanFIFO IanFIFO(0,1,2,3,11,12,13);  //initial IanFIFO class
DFR0009 Key(0);                      //analog pin 0 for DFR0009 key pin
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); //select pins used on the LCD panel of DFR0009
spdifSource_t current_spdif, selected_spdif;
int _taskDelay = 0;

#define _setTask _taskDelay = 30

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  selected_spdif = (spdifSource_t)EEPROM.read(0);
  if((selected_spdif != OPT) && (selected_spdif != COX) && (selected_spdif != TTL) && (selected_spdif != BKD))
    selected_spdif = BKD;
}

void loop() {
  IanOS.run();

  if(IanFIFO.checkSpdifFlag() || (current_spdif == SPDIFUNKNOWN)) {
    current_spdif = IanFIFO.getSpdifSource();
    _setTask;
  }

  switch(Key.getflag()) {
    case btnRIGHT:
      IanFIFO.setFifoPin(0);
      selected_spdif = COX;
      EEPROM.write(0, selected_spdif);
      _setTask;
      break;
    case btnLEFT:
      IanFIFO.setFifoPin(0);
      selected_spdif = TTL;
      EEPROM.write(0, selected_spdif);
      _setTask;
      break;
    case btnUP:
      IanFIFO.setFifoPin(0);
      selected_spdif = OPT;
      EEPROM.write(0, selected_spdif);
      _setTask;
      break;
    case btnDOWN:
      IanFIFO.setFifoPin(0);
      selected_spdif = BKD;
      EEPROM.write(0, selected_spdif);
      _setTask;
      break;
  }
}

void IanOStasks(void) {
  IanFIFO.scan();
  Key.scan();

  if(_taskDelay > 0) {
    if((--_taskDelay) == 0) {
      if(current_spdif != selected_spdif)
        IanFIFO.launchSpdifPin(30);
    }
  }
}
