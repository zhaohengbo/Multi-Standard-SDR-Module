//###########################################################################
//
// FILE:	I2C.c
//
// TITLE:	DSP280x SCI Initialization & Support Functions.
//
//###########################################################################

#include "main.h"   

// Prototype statements for functions found within this file.


volatile unsigned int tmpBuf [8];

 
//---------------------------------------------------------------------------
// InitI2C: 
//---------------------------------------------------------------------------
// This function initializes the I2C to a known state.
//

void InitI2CA(void)
{

	EALLOW;
	/* Enable internal pull-up for the selected pins */
	// Pull-ups can be enabled or disabled disabled by the user.  
	// This will enable the pullups for the specified pins.
	// Comment out other unwanted lines.

	GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;    // Enable pull-up for GPIO32 (SDAA)
	GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;	   // Enable pull-up for GPIO33 (SCLA)

	/* Set qualification for selected pins to asynch only */
	// This will select asynch (no qualification) for the selected pins.
	// Comment out other unwanted lines.

	GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;  // Asynch input GPIO32 (SDAA)
	GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3;  // Asynch input GPIO33 (SCLA)

	/* Configure SCI pins using GPIO regs*/
	// This specifies which of the possible GPIO pins will be I2C functional pins.
	// Comment out other unwanted lines.

	GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;   // Configure GPIO32 for SDAA operation
	GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1;   // Configure GPIO33 for SCLA operation

	SysCtrlRegs.PCLKCR0.bit.I2CAENCLK = 1;   // I2C
	EDIS;

#ifdef SOM1A
	GpioCtrlRegs.GPADIR.bit.GPIO8 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO10 = 0;

	if(!GpioDataRegs.GPADAT.bit.GPIO8 && GpioDataRegs.GPADAT.bit.GPIO10){
		I2caRegs.I2COAR = 0x0061;		// Own address
		GpioCtrlRegs.GPADIR.bit.GPIO8 = 1;
		GpioCtrlRegs.GPADIR.bit.GPIO10 = 1;
		GpioDataRegs.GPASET.bit.GPIO10 = 1;
		GpioDataRegs.GPACLEAR.bit.GPIO8 = 1;
	}
	else if(GpioDataRegs.GPADAT.bit.GPIO8 && !GpioDataRegs.GPADAT.bit.GPIO10){
		I2caRegs.I2COAR = 0x0062;		// Own address
		GpioCtrlRegs.GPADIR.bit.GPIO8 = 1;
		GpioCtrlRegs.GPADIR.bit.GPIO10 = 1;
		GpioDataRegs.GPASET.bit.GPIO8 = 1;
		GpioDataRegs.GPACLEAR.bit.GPIO10 = 1;
	}
	else if(GpioDataRegs.GPADAT.bit.GPIO8 && GpioDataRegs.GPADAT.bit.GPIO10){
		I2caRegs.I2COAR = 0x0063;		// Own address
		GpioCtrlRegs.GPADIR.bit.GPIO8 = 1;
		GpioCtrlRegs.GPADIR.bit.GPIO10 = 1;
		GpioDataRegs.GPASET.bit.GPIO8 = 1;
		GpioDataRegs.GPASET.bit.GPIO10 = 1;
	}
	else{
		I2caRegs.I2COAR = 0x0060;		// Own address
		GpioCtrlRegs.GPADIR.bit.GPIO8 = 1;
		GpioCtrlRegs.GPADIR.bit.GPIO10 = 1;
		GpioDataRegs.GPACLEAR.bit.GPIO8 = 1;
		GpioDataRegs.GPACLEAR.bit.GPIO10 = 1;
	}

#else
	GpioCtrlRegs.GPADIR.bit.GPIO21 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO22 = 0;

	if(!GpioDataRegs.GPADAT.bit.GPIO21 && GpioDataRegs.GPADAT.bit.GPIO22){
		I2caRegs.I2COAR = 0x0061;		// Own address
		GpioCtrlRegs.GPADIR.bit.GPIO21 = 1;
		GpioCtrlRegs.GPADIR.bit.GPIO22 = 1;
		GpioDataRegs.GPASET.bit.GPIO22 = 1;
		GpioDataRegs.GPACLEAR.bit.GPIO21 = 1;
	}
	else if(GpioDataRegs.GPADAT.bit.GPIO21 && !GpioDataRegs.GPADAT.bit.GPIO22){
		I2caRegs.I2COAR = 0x0062;		// Own address
		GpioCtrlRegs.GPADIR.bit.GPIO21 = 1;
		GpioCtrlRegs.GPADIR.bit.GPIO22 = 1;
		GpioDataRegs.GPASET.bit.GPIO21 = 1;
		GpioDataRegs.GPACLEAR.bit.GPIO22 = 1;
	}
	else if(GpioDataRegs.GPADAT.bit.GPIO21 && GpioDataRegs.GPADAT.bit.GPIO22){
		I2caRegs.I2COAR = 0x0063;		// Own address
		GpioCtrlRegs.GPADIR.bit.GPIO21 = 1;
		GpioCtrlRegs.GPADIR.bit.GPIO22 = 1;
		GpioDataRegs.GPASET.bit.GPIO21 = 1;
		GpioDataRegs.GPASET.bit.GPIO22 = 1;
	}
	else{
		I2caRegs.I2COAR = 0x0060;		// Own address
		GpioCtrlRegs.GPADIR.bit.GPIO21 = 1;
		GpioCtrlRegs.GPADIR.bit.GPIO22 = 1;
		GpioDataRegs.GPACLEAR.bit.GPIO21 = 1;
		GpioDataRegs.GPACLEAR.bit.GPIO22 = 1;
	}
#endif
	I2caRegs.I2CSAR = I2C_SLAVE_ADDRESS;

//I2caRegs.I2COAR = I2C_SLAVE_ADDRESS;
//I2caRegs.I2CSAR = 0x0060;

	I2caRegs.I2CMDR.bit.IRS = 0;

	//master clk = 384615 Hz
	I2caRegs.I2CPSC.all = 5;		// 60/(5+1)=10MHz
	I2caRegs.I2CCLKL = 8;			// NOTE: must be non zero
	I2caRegs.I2CCLKH = 8;			// NOTE: must be non zero

//	I2caRegs.I2CIER.all = 0x24;		// Enable SCD & ARDY interrupts
	I2caRegs.I2CIER.all = 0x20;		// Enable SCD
//	I2caRegs.I2CIER.all = 0x60;		// Enable SCD & AAS  interrupts


	I2caRegs.I2CFFTX.all = 0x4060;	// Enable FIFO mode and TXFIFO
//	I2caRegs.I2CFFTX.all = 0x6040;	// Enable FIFO mode and TXFIFO
	I2caRegs.I2CFFRX.all = 0x2061;	// Enable RXFIFO, clear RXFFINT,

	I2caRegs.I2CMDR.all = 0x0020;	// Take I2C out of reset

	EALLOW;	// This is needed to write to EALLOW protected registers
	PieVectTable.I2CINT1A = &i2c_int1a_isr;
	PieVectTable.I2CINT2A = &i2c_int2a_isr;
	EDIS;   // This is needed to disable write to EALLOW protected registers

	// Enable I2C interrupt 1 in the PIE: Group 8 interrupt 1
	PieCtrlRegs.PIEIER8.bit.INTx1 = 1;

	// Enable I2C interrupt 2 in the PIE: Group 8 interrupt 2
	PieCtrlRegs.PIEIER8.bit.INTx2 = 1;

	// Enable CPU INT8 which is connected to PIE group 8
	IER |= M_INT8;

	return;
}

//---------------------------------------------------------------------------------  

interrupt void i2c_int1a_isr(void)     // I2C-A
{

	I2caRegs.I2CMDR.bit.MST = 0;
	I2caRegs.I2CMDR.bit.TRX = 0;

	if((hostItf.state & HOST_STATE_TX)!=0){
		hostItf.state &= ~HOST_STATE_TX;
		//_SetEvent(EV_I2CA_SENT);
	}

	I2caRegs.I2CSTR.bit.SCD = 1;
	// Enable future I2C (PIE Group 8) interrupts
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
}

//---------------------------------------------------------------------------------  

interrupt void i2c_int2a_isr(void)     
{
    unsigned int len;

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
	PieCtrlRegs.PIEIER8.bit.INTx2 = 0;
	EINT;
	if(I2caRegs.I2CFFTX.bit.TXFFINT){
	    if( hostItf.txFrame.offset == hostItf.txFrame.length ){
//			I2caRegs.I2CMDR.bit.MST = 0;
//			I2caRegs.I2CMDR.bit.TRX = 0;
			I2caRegs.I2CFFTX.bit.TXFFRST = 0;
		}
	    while( (hostItf.txFrame.offset != hostItf.txFrame.length) && (I2caRegs.I2CFFTX.bit.TXFFST<16) ){
			I2caRegs.I2CDXR = __byte((int*)hostItf.txFrame.buffer,hostItf.txFrame.offset++);
			//hostItf.txFrame.length--;
		}
		if( hostItf.txFrame.offset == hostItf.txFrame.length ){
			I2caRegs.I2CFFTX.bit.TXFFIL = 0;
		}
		I2caRegs.I2CFFTX.bit.TXFFINTCLR = 1;
	}
	if(I2caRegs.I2CFFRX.bit.RXFFINT){


//		len = I2caRegs.I2CFFRX.bit.RXFFST;
		len = I2caRegs.I2CFFRX.bit.RXFFIL;
	    {
	        unsigned int i = 0;
	        for (; i < len; i++) {
	            __byte((int*)tmpBuf,i) = I2caRegs.I2CDRR;
	        }
	    }
		if(len = HostReceive((unsigned int*)tmpBuf,len,IfaceI2C)){
			//SetEvent(EV_I2CA_RCVD);
			I2caRegs.I2CFFRX.bit.RXFFIL = len;
		}

/*
	    do{
			__byte((int*)host.rx.frame,host.rx.idx++) = I2caRegs.I2CDRR;
		}while(I2caRegs.I2CFFRX.bit.RXFFST);
*/
		I2caRegs.I2CFFRX.bit.RXFFINTCLR = 1;
	}
	DINT;
	PieCtrlRegs.PIEIER8.bit.INTx2 = 1;
	// Enable future I2C (PIE Group 8) interrupts
//	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
}

//---------------------------------------------------------------------------------  

//--------------------------------------------------------------------------


//===========================================================================
// End of file.
//===========================================================================
