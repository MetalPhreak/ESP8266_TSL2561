/*
* The MIT License (MIT)
* 
* Copyright (c) 2015 David Ogilvy (MetalPhreak)
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#ifndef TSL2561_H
#define TSL2561_H

#include "driver/i2c.h"
#include "driver/tsl2561_register.h"

bool TSL2561_init(uint8 addr);
void TSL2561_writeByte(uint8 addr, uint8 reg, uint8 data);
uint8 TSL2561_readByte(uint8 addr, uint8 reg);
void TSL2561_writeWord(uint8 addr, uint8 reg, uint16 data);
uint16 TSL2561_readWord(uint8 addr, uint8 reg);

uint32 TSL2561_autoLux(uint8 addr, bool autorange);
uint32 TSL2561_toLux(uint8 package, uint8 gain, uint8 integ, uint16 ch0, uint16 ch1);

#define TSL2561_getLux(addr) TSL2561_autoLux(addr, 0);



#endif

