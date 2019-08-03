//###########################################################################
//
// FILE:   Spi.c
//
// TITLE:  DSP280x SPI Initialization & Support Functions.
//
//###########################################################################

#include "main.h"

//---------------------------------------------------------------------------
// InitSPI: 
//---------------------------------------------------------------------------
// This function initializes the SPI(s) to a known state.
//
interrupt void spicTxFifoIsr(void);
interrupt void spicRxFifoIsr(void);



void InitSpiaGpio()
{

   EALLOW;
/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled by the user.  
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;   // Enable pull-up on GPIO16 (SPISIMOA)
    GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;   // Enable pull-up on GPIO17 (SPISOMIA)
    GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;   // Enable pull-up on GPIO18 (SPICLKA)
    GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;   // Enable pull-up on GPIO19 (SPISTEA)


/* Set qualification for selected pins to asynch only */
// This will select asynch (no qualification) for the selected pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 3; // Asynch input GPIO16 (SPISIMOA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3; // Asynch input GPIO17 (SPISOMIA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3; // Asynch input GPIO18 (SPICLKA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3; // Asynch input GPIO19 (SPISTEA)
    
/* Configure SPI-A pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be SPI functional pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 1; // Configure GPIO16 as SPISIMOA
    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 1; // Configure GPIO17 as SPISOMIA
    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 1; // Configure GPIO18 as SPICLKA
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 1; // Configure GPIO19 as SPISTEA

    EDIS;
}

#if DSP28_SPIB
void InitSpibGpio()
{
   EALLOW;
    	
/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled disabled by the user.  
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;     // Enable pull-up on GPIO12 (SPISIMOB)
//  GpioCtrlRegs.GPAPUD.bit.GPIO24 = 0;     // Enable pull-up on GPIO24 (SPISIMOB)

    GpioCtrlRegs.GPAPUD.bit.GPIO13 = 0;     // Enable pull-up on GPIO13 (SPISOMIB)                                                                           
//  GpioCtrlRegs.GPAPUD.bit.GPIO25 = 0;     // Enable pull-up on GPIO25 (SPISOMIB)

    GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;     // Enable pull-up on GPIO14 (SPICLKB)                                                                           
//  GpioCtrlRegs.GPAPUD.bit.GPIO26 = 0;     // Enable pull-up on GPIO26 (SPICLKB) 

    GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;     // Enable pull-up on GPIO15 (SPISTEB)                                                                           
//  GpioCtrlRegs.GPAPUD.bit.GPIO27 = 0;     // Enable pull-up on GPIO27 (SPISTEB) 

    
/* Set qualification for selected pins to asynch only */
// This will select asynch (no qualification) for the selected pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAQSEL1.bit.GPIO12 = 3;   // Asynch input GPIO12 (SPISIMOB)
//  GpioCtrlRegs.GPAQSEL2.bit.GPIO24 = 3;   // Asynch input GPIO24 (SPISIMOB)

    GpioCtrlRegs.GPAQSEL1.bit.GPIO13 = 3;   // Asynch input GPIO13 (SPISOMIB)                                                                                       
//  GpioCtrlRegs.GPAQSEL2.bit.GPIO25 = 3;   // Asynch input GPIO25 (SPISOMIB)

    GpioCtrlRegs.GPAQSEL1.bit.GPIO14 = 3;   // Asynch input GPIO14 (SPICLKB)                                                                                        
//  GpioCtrlRegs.GPAQSEL2.bit.GPIO26 = 3;   // Asynch input GPIO26 (SPICLKB) 

    GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 3;   // Asynch input GPIO15 (SPISTEB)                                                                                        
//  GpioCtrlRegs.GPAQSEL2.bit.GPIO27 = 3;   // Asynch input GPIO27 (SPISTEB) 

/* Configure SPI-B pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be SPI functional pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 3;    // Configure GPIO12 as SPISIMOB
//  GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 3;    // Configure GPIO24 as SPISIMOB
         
    GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 3;    // Configure GPIO13 as SPISOMIB     
//  GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 3;    // Configure GPIO25 as SPISOMIB         

    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 3;    // Configure GPIO14 as SPICLKB     
//  GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 3;    // Configure GPIO26 as SPICLKB         

    GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 3;    // Configure GPIO15 as SPISTEB     
//  GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 3;    // Configure GPIO27 as SPISTEB         

    EDIS;
}   
#endif //endif DSP28_SPIB

#if DSP28_SPIC
void InitSpicGpio()
{
   EALLOW;
    	
/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled disabled by the user.  
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAPUD.bit.GPIO20 = 0;   // Enable pull-up on GPIO20 (SPISIMOC)
    GpioCtrlRegs.GPAPUD.bit.GPIO21 = 0;   // Enable pull-up on GPIO21 (SPISOMIC)                                                                                                             
    GpioCtrlRegs.GPAPUD.bit.GPIO22 = 0;   // Enable pull-up on GPIO22 (SPICLKC)                                                                                                             
    GpioCtrlRegs.GPAPUD.bit.GPIO23 = 0;   // Enable pull-up on GPIO23 (SPISTEC)                                                                                                             
    
/* Set qualification for selected pins to asynch only */
// This will select asynch (no qualification) for the selected pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAQSEL2.bit.GPIO20 = 3;   // Asynch input GPIO20 (SPISIMOC)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO21 = 3;   // Asynch input GPIO21 (SPISOMIC)                                                                                                                           
    GpioCtrlRegs.GPAQSEL2.bit.GPIO22 = 3;   // Asynch input GPIO22 (SPICLKC)                                                                                                                            
    GpioCtrlRegs.GPAQSEL2.bit.GPIO23 = 3;   // Asynch input GPIO23 (SPISTEC)                                                                                                                            

/* Configure SPI-C pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be SPI functional pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 2;   // Configure GPIO20 as SPISIMOC
    GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 2;   // Configure GPIO21 as SPISOMIC     
    GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 2;   // Configure GPIO22 as SPICLKC    
    GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 2;   // Configure GPIO23 as SPISTEC     

    EDIS;
}
#endif // endif DSP28_SPIC   


#if DSP28_SPID
void InitSpidGpio()
{
   EALLOW;
/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled disabled by the user.  
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 0;    // Enable pull-up on GPIO1 (SPISIMOD)
    GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;    // Enable pull-up on GPIO3 (SPISOMID)  
    GpioCtrlRegs.GPAPUD.bit.GPIO5 = 0;    // Enable pull-up on GPIO5 (SPICLKD)                                                                                                                       
    GpioCtrlRegs.GPAPUD.bit.GPIO7 = 0;    // Enable pull-up on GPIO7 (SPISTED)                                                                                                                       
    
/* Set qualification for selected pins to asynch only */
// This will select asynch (no qualification) for the selected pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAQSEL1.bit.GPIO1 = 3;   // Asynch input GPIO1 (SPISIMOD)
    GpioCtrlRegs.GPAQSEL1.bit.GPIO3 = 3;   // Asynch input GPIO3 (SPISOMID)                                                                                                                                     
    GpioCtrlRegs.GPAQSEL1.bit.GPIO5 = 3;   // Asynch input GPIO5 (SPICLKD)                                                                                                                                      
    GpioCtrlRegs.GPAQSEL1.bit.GPIO7 = 3;   // Asynch input GPIO7 (SPISTED)                                                                                                                                      

/* Configure SPI-D pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be SPI functional pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 2;   // Configure GPIO1 as SPISIMOD
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 2;   // Configure GPIO3 as SPISOMID     
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 2;   // Configure GPIO5 as SPICLKD   
    GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 2;   // Configure GPIO7 as SPISTED     

    EDIS;
}   
#endif // endif DSP28_SPID


void InitSpi(void)
{

   EALLOW;
   SysCtrlRegs.PCLKCR0.bit.SPICENCLK = 1;   // SPI-C
   PieVectTable.SPIRXINTC = &spicRxFifoIsr;
   PieVectTable.SPITXINTC = &spicTxFifoIsr;
   EDIS;
   PieCtrlRegs.PIEIER6.bit.INTx5=1;     // Enable PIE Group 6, INT 5, RXINT
   PieCtrlRegs.PIEIER6.bit.INTx6=1;     // Enable PIE Group 6, INT 6, TXINT
   IER = M_INT6;                        // Enable CPU INT6

   InitSpicGpio();
   SpicRegs.SPICCR.bit.SPISWRESET=0; // Reset SCI

   SpicRegs.SPICCR.all=0x0047;       //8-bit character
//   SpicRegs.SPICTL.all=0x0013;       //overrun Int enabled, TALK, SPI Int enabled ///SLAVE
   SpicRegs.SPICTL.all=0x0002;       //TALK  ///SLAVE
   SpicRegs.SPISTS.all=0x0000;
//   SpicRegs.SPIBRR=0x0026;           // Baud rate ~400K
   SpicRegs.SPIBRR=0x007F;           // Baud rate ~100K
   SpicRegs.SPIFFTX.all=0xC020;      // Enable FIFO's, set TX FIFO level to 0
   SpicRegs.SPIFFRX.all=0x0021;      // Set RX FIFO level to 1
   SpicRegs.SPIFFCT.all=0x00;
//   SpicRegs.SPIFFCT.all=0x18;
//   SpicRegs.SPIPRI.all=0x0010;
   SpicRegs.SPIPRI.all=0x0000;

   SpicRegs.SPICCR.bit.SPISWRESET=1;  // Enable SPI

//   SpicRegs.SPIFFTX.bit.TXFIFO=1;
   SpicRegs.SPIFFRX.bit.RXFIFORESET=1;

 
}

//---------------------------------------------------------------------------
// Example: InitSpiGpio: 
//---------------------------------------------------------------------------
// This function initializes GPIO pins to function as SPI pins
//
// Each GPIO pin can be configured as a GPIO pin or up to 3 different
// peripheral functional pins. By default all pins come up as GPIO
// inputs after reset.  
// 
// Caution: 
// For each SPI peripheral
// Only one GPIO pin should be enabled for SPISOMO operation.
// Only one GPIO pin should be enabled for SPISOMI operation. 
// Only one GPIO pin should be enabled for SPICLKA operation. 
// Only one GPIO pin should be enabled for SPISTEA operation. 
// Comment out other unwanted lines.

void InitSpiGpio()
{

   InitSpiaGpio();
#if DSP28_SPIB
   InitSpibGpio();
#endif // endif DSP28_SPIB
#if DSP28_SPIC     
   InitSpicGpio();
#endif // endif DSP28_SPIC
#if DSP28_SPID
   InitSpidGpio();
#endif // endif DSP28_SPID     
}

interrupt void spicTxFifoIsr(void)
{
	PieCtrlRegs.PIEACK.all |= 0x20;  		// Issue PIE ACK
	PieCtrlRegs.PIEIER6.bit.INTx6 = 0;     // Disable PIE Group 6, INT 6, TXINT
	EINT;

    if( hostItf.txFrame.offset == hostItf.txFrame.length ){
		SpicRegs.SPICCR.bit.SPISWRESET = 0;
		SpicRegs.SPICCR.bit.SPISWRESET = 1;
		SpicRegs.SPICTL.bit.MASTER_SLAVE = 0;
		SpicRegs.SPIFFTX.bit.TXFIFO = 0;
		SpicRegs.SPIFFTX.bit.SPIRST = 0;
		SpicRegs.SPIFFTX.bit.SPIRST = 1;
		SpicRegs.SPIFFRX.bit.RXFIFORESET = 1;
		hostItf.state &= ~HOST_STATE_TX;
	}
	else{
//		SpicRegs.SPICTL.bit.MASTER_SLAVE = 1;
	    while( (hostItf.txFrame.offset != hostItf.txFrame.length) && (SpicRegs.SPIFFTX.bit.TXFFST<16) )
	    {
			SpicRegs.SPITXBUF = (__byte((int*)hostItf.txFrame.buffer,hostItf.txFrame.offset++))<<8;
		}
	}

	DINT;   
    SpicRegs.SPIFFTX.bit.TXFFINTCLR=1;  // Clear Interrupt flag
	PieCtrlRegs.PIEIER6.bit.INTx6 = 1;     // Enable PIE Group 6, INT 6, TXINT

//	PieCtrlRegs.PIEACK.all|=0x20;  		// Issue PIE ACK
}

interrupt void spicRxFifoIsr(void)
{
    unsigned int tmpBuf [8],len = SpicRegs.SPIFFRX.bit.RXFFIL;


    {
        unsigned int i = 0;
        for (; i < len; i++) {
            __byte((int*)tmpBuf,i) = SpicRegs.SPIRXBUF;
        }
    }
	PieCtrlRegs.PIEACK.all |= 0x20;  		// Issue PIE ACK
	PieCtrlRegs.PIEIER6.bit.INTx5=1;     // Disable PIE Group 6, INT 5, RXINT
	EINT;
/*
	if(SciaRegs.SCIRXST.bit.RXERROR){
		SciaRegs.SCICTL1.bit.SWRESET = 1;	
		SciaRegs.SCICTL1.bit.SWRESET = 0;	
		InitSci_();
	}
	else
*/	if(len = HostReceive(tmpBuf,len,IfaceSPI)){
		SpicRegs.SPIFFRX.bit.RXFFIL = len;
	}

	DINT;   

	SpicRegs.SPIFFRX.bit.RXFFOVFCLR=1;  // Clear Overflow flag
	SpicRegs.SPIFFRX.bit.RXFFINTCLR=1; 	// Clear Interrupt flag

	PieCtrlRegs.PIEIER6.bit.INTx5=1;     // Enable PIE Group 6, INT 5, RXINT

//	PieCtrlRegs.PIEACK.all|=0x20;       // Issue PIE ack
}


//===========================================================================
// End of file.
//===========================================================================
