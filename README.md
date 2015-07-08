ESP8266 TSL2561 Driver
========

Driver Library for the TAOS TSL2561 light sensor. 

The TLS2561 is an i2c device with two diode sensors and intgrated ADCs for measuring diode response. One diode measures broadband light spectrum while the other measures infrared spectrum light only. Subtracting the IR response from the Broadband response gives an approximation of human-eye response. 

Usage
========
In user_init call:

i2cinit();

TSL2561_init(TSL2561_ADDR_F);


Note: Use TSL2561_ADDR_L or TSL2561_ADDR_H if your device address pin is connected to VCC or GND


To get a reading in Lux:

uint32 lux = TSL2561_getLux(TSL2561_ADDR_F);


To set Gain or Integration Time:

TSL2561_writeByte(TSL2561_ADDR_F, TSL2561_REGISTER_TIMING, TSL2561_INTEG_13_7MS | TSL2561_GAIN_X16);


See TSL2561_register.h for a full list of register names and options. 

See i2c.h to set the GPIO pins for i2c. Default is SCK = GPIO0, SDA = GPIO2.

