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


#include "driver/tsl2561.h"
#include "driver/i2c.h"



bool TSL2561_init(uint8 addr){
	TSL2561_writeByte(addr, TSL2561_REGISTER_CONTROL, TSL2561_CONTROL_POWERON);	
	return (bool) TSL2561_readByte(addr, TSL2561_REGISTER_CONTROL); 
}

void TSL2561_writeByte(uint8 addr, uint8 reg, uint8 data){
	i2c_start();
	i2c_writeByte(addr<<1 | 0); //address dev for write
	i2c_check_ack(); 
	i2c_writeByte(TSL2561_COMMAND_BIT | reg); //command code
	i2c_check_ack();
	i2c_writeByte(data);
	i2c_check_ack();
	i2c_stop();
}

uint8 TSL2561_readByte(uint8 addr, uint8 reg){
	uint8 data;
	i2c_start();
	i2c_writeByte(addr<<1 | 0); //address dev for write
	i2c_check_ack(); 
	i2c_writeByte(TSL2561_COMMAND_BIT | reg); //command code
	i2c_check_ack();
	i2c_start();
	i2c_writeByte(addr<<1 | 1); //address dev for read
	i2c_check_ack(); 
	data = i2c_readByte();
	i2c_send_ack(0);
	i2c_stop();
	return data;
}

void TSL2561_writeWord(uint8 addr, uint8 reg, uint16 data){
	i2c_start();
	i2c_writeByte(addr<<1 | 0); //address dev for write
	i2c_check_ack(); 
	i2c_writeByte(TSL2561_COMMAND_BIT | TSL2561_WORD_BIT | reg); //command code
	i2c_check_ack();
	i2c_writeByte((uint8) data);
	i2c_check_ack();
	i2c_writeByte((uint8) data>>8);
	i2c_check_ack();
	i2c_stop();
}

uint16 TSL2561_readWord(uint8 addr, uint8 reg){
	uint16 data;
	i2c_start();
	i2c_writeByte(addr<<1 | 0); //address dev for write
	i2c_check_ack(); 
	i2c_writeByte(TSL2561_COMMAND_BIT | TSL2561_WORD_BIT | reg); //command code
	i2c_check_ack();
	i2c_start();
	i2c_writeByte(addr<<1 | 1); //address dev for read
	i2c_check_ack(); 
	data = i2c_readByte();
	i2c_send_ack(1); //NACK
	data = data | i2c_readByte()<<8;
	i2c_send_ack(0); //ACK
	i2c_stop();
	return data;
}


uint32 TSL2561_autoLux(uint8 addr, bool autorange){
	uint8 tsl2561_package = TSL2561_readByte(addr, TSL2561_REGISTER_ID)>>6;
	uint8 t_reg = TSL2561_readByte(addr, TSL2561_REGISTER_TIMING);
	uint8 tsl2561_gain = t_reg  & 0x10;
	uint8 tsl2561_integ = t_reg & 0x03;
	uint16 tsl2561_ch0 = TSL2561_readWord(addr, TSL2561_REGISTER_CHAN0_LOW);
	uint16 tsl2561_ch1 = TSL2561_readWord(addr, TSL2561_REGISTER_CHAN1_LOW);

	if((tsl2561_ch0 == 0) || (tsl2561_ch1 == 0)){
		return 0; //todo autorange
	}

	

	return TSL2561_toLux(tsl2561_package, tsl2561_gain, tsl2561_integ, tsl2561_ch0, tsl2561_ch1);
}


uint32 TSL2561_toLux(uint8 package, uint8 gain, uint8 integ, uint16 ch0, uint16 ch1){
	uint32 chScale;
	uint32 channel1 = (uint32) ch1;
	uint32 channel0 = (uint32) ch0;

	switch(integ)
	{
		case TSL2561_INTEG_13_7MS:
			chScale = TSL2561_LUX_CHSCALE_TINT0;
			break;
		case TSL2561_INTEG_101MS:
			chScale = TSL2561_LUX_CHSCALE_TINT1;
			break;
		case TSL2561_INTEG_402MS:
			chScale = (1 << TSL2561_LUX_CHSCALE);
			break;
		default:
			return 0; //can't handle manual integration time here
			break;
	}

	if(!gain){
		chScale = chScale << 4;
	}

	channel0 = (channel0 * chScale) >> TSL2561_LUX_CHSCALE;
	channel1 = (channel1 * chScale) >> TSL2561_LUX_CHSCALE;

	uint32 ratio = 0;
	if (channel0 != 0){
		ratio = (channel1 << (TSL2561_LUX_RATIOSCALE+1)) / channel0;
	}

	ratio = (ratio + 1) >> 1;

	uint16 b, m;

	switch(package)
	{
		case TSL2561_PACKAGE_T_FN_CL:
			if((ratio >= 0) && (ratio <= TSL2561_LUX_K1T))
				{b=TSL2561_LUX_B1T; m=TSL2561_LUX_M1T;}
			else if(ratio <= TSL2561_LUX_K2T)
				{b=TSL2561_LUX_B2T; m=TSL2561_LUX_M2T;}
			else if(ratio <= TSL2561_LUX_K3T)
				{b=TSL2561_LUX_B3T; m=TSL2561_LUX_M3T;}
			else if(ratio <= TSL2561_LUX_K4T)
				{b=TSL2561_LUX_B4T; m=TSL2561_LUX_M4T;}
			else if(ratio <= TSL2561_LUX_K5T)
				{b=TSL2561_LUX_B5T; m=TSL2561_LUX_M5T;}
			else if(ratio <= TSL2561_LUX_K6T)
				{b=TSL2561_LUX_B6T; m=TSL2561_LUX_M6T;}
			else if(ratio <= TSL2561_LUX_K7T)
				{b=TSL2561_LUX_B7T; m=TSL2561_LUX_M7T;}
			else if(ratio > TSL2561_LUX_K8T)
				{b=TSL2561_LUX_B8T; m=TSL2561_LUX_M8T;}
			break;
		case TSL2561_PACKAGE_CS:
			if((ratio >= 0) && (ratio <= TSL2561_LUX_K1C))
				{b=TSL2561_LUX_B1C; m=TSL2561_LUX_M1C;}
			else if(ratio <= TSL2561_LUX_K2C)
				{b=TSL2561_LUX_B2C; m=TSL2561_LUX_M2C;}
			else if(ratio <= TSL2561_LUX_K3C)
				{b=TSL2561_LUX_B3C; m=TSL2561_LUX_M3C;}
			else if(ratio <= TSL2561_LUX_K4C)
				{b=TSL2561_LUX_B4C; m=TSL2561_LUX_M4C;}
			else if(ratio <= TSL2561_LUX_K5C)
				{b=TSL2561_LUX_B5C; m=TSL2561_LUX_M5C;}
			else if(ratio <= TSL2561_LUX_K6C)
				{b=TSL2561_LUX_B6C; m=TSL2561_LUX_M6C;}
			else if(ratio <= TSL2561_LUX_K7C)
				{b=TSL2561_LUX_B7C; m=TSL2561_LUX_M7C;}
			else if(ratio > TSL2561_LUX_K8C)
				{b=TSL2561_LUX_B8C; m=TSL2561_LUX_M8C;}
			break;
	}

	uint32 temp = (channel0 * b) - (channel1 * m);
	if(temp < 0) temp = 0;

	temp += (1 << (TSL2561_LUX_LUXSCALE - 1));

	return (temp >> TSL2561_LUX_LUXSCALE);
}












