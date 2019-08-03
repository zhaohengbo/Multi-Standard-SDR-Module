//###########################################################################
//
// FILE:	I2C.c
//
// TITLE:	DSP280x SCI Initialization & Support Functions.
//
//###########################################################################

#ifndef DSP280X_I2C_DEFINES_H
#define DSP280X_I2C_DEINFES_H

//--------------------------------------------
// Defines
//--------------------------------------------
#define I2C_RX_BUFF_LEN	127
#define I2C_TX_BUFF_LEN	127

#define	I2C_SLAVE_ADDRESS	0x50
//EEPROM

interrupt void i2c_int1a_isr(void);
interrupt void i2c_int2a_isr(void);

#endif  // end of DSP280X_I2C_DEFINES_H definition

//===========================================================================
// End of file.
//===========================================================================
