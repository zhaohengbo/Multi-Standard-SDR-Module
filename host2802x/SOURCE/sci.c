//###########################################################################
//
// FILE:	Sci.c
//
// TITLE:	DSP280x SCI Initialization & Support Functions.
//
//###########################################################################

#include "main.h"

#define meterHost


//---------------------------------------------------------------------------
// InitSci: 
//---------------------------------------------------------------------------
// This function initializes the SCI(s) to a known state.
//
void InitSci_(void)
{
	// Initialize SCI-A:

   EALLOW;  
   SysCtrlRegs.PCLKCR0.bit.SCIAENCLK = 1;   // SCI-A
   PieVectTable.SCIRXINTA = &sciaRxFifoIsr;
   PieVectTable.SCITXINTA = &sciaTxFifoIsr;
   EDIS;    

   PieCtrlRegs.PIEIER9.bit.INTx1=1;     // PIE Group 9, INT1
   PieCtrlRegs.PIEIER9.bit.INTx2=1;     // PIE Group 9, INT2

    // Note: Clocks were turned on to the SCIA peripheral
    // in the InitSysCtrl() function
    
 	SciaRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback 
                                   // No parity,8 char bits,
                                   // async mode, idle-line protocol
//	SciaRegs.SCICCR.bit.LOOPBKENA = 1;
	SciaRegs.SCICTL1.all =0x0083;  // enable TX, RX, internal SCICLK,RX ERR 
                                   // Disable SLEEP, TXWAKE
	SciaRegs.SCICTL2.all = 0;	//disable TX, RX int 



    SciaRegs.SCIFFTX.all=0xC060;
    SciaRegs.SCIFFRX.all=0x0061;
    SciaRegs.SCIFFCT.all=0x0;


	SciaRegs.SCICTL1.all =0x0023;  // Relinquish SCI from Reset 

	SciaRegs.SCIFFTX.bit.TXFIFOXRESET=0;
    SciaRegs.SCIFFRX.bit.RXFIFORESET=1;	 

 	IER |= M_INT9;

}	


interrupt void sciaTxFifoIsr(void)
{   
	PieCtrlRegs.PIEACK.all|=0x100;      // Issue PIE ACK
    PieCtrlRegs.PIEIER9.bit.INTx2 = 0;     // PIE Group 9, INT2
	EINT;

    if( hostItf.txFrame.offset == hostItf.txFrame.length ){
    	SciaRegs.SCIFFTX.bit.TXFIFOXRESET=0;
		hostItf.state &= ~HOST_STATE_TX;
	}
    while( (hostItf.txFrame.offset != hostItf.txFrame.length) && (SciaRegs.SCIFFTX.bit.TXFFST<4) )
    {
		SciaRegs.SCITXBUF = __byte( (int*)hostItf.txFrame.buffer, hostItf.txFrame.offset++ );
		//hostItf.txFrame.offset++;
		//hostItf.txFrame.length--;
	}
	DINT;   
	SciaRegs.SCIFFTX.bit.TXFFINTCLR = 1;	// Clear SCI Interrupt flag
    PieCtrlRegs.PIEIER9.bit.INTx2 = 1;     // PIE Group 9, INT2

//	PieCtrlRegs.PIEACK.all|=0x100;      // Issue PIE ACK

}


interrupt void sciaRxFifoIsr(void)
{   
//    Uint16 i;
//    rdataA[i]=SciaRegs.SCIRXBUF.all;	 // Read data 
    unsigned int tmpBuf [8],len = SciaRegs.SCIFFRX.bit.RXFFIL;


    {
        unsigned int i = 0;
        for (; i < len; i++) {
            __byte((int*)tmpBuf,i) = SciaRegs.SCIRXBUF.bit.RXDT;
        }
    }
   	PieCtrlRegs.PIEACK.all|=0x100;       // Issue PIE ack
    PieCtrlRegs.PIEIER9.bit.INTx1 = 0; 
	EINT;


	if(SciaRegs.SCIRXST.bit.RXERROR){
		SciaRegs.SCICTL1.bit.SWRESET = 1;	
		SciaRegs.SCICTL1.bit.SWRESET = 0;	
		InitSci_();
	}
	else if(len = HostReceive(tmpBuf,len,IfaceSCI)){
//		SetEvent(EV_SCIA_RCVD);
		SciaRegs.SCIFFRX.bit.RXFFIL = len;
	}


	DINT;   
	SciaRegs.SCIFFRX.bit.RXFFOVRCLR = 1;   // Clear Overflow flag
	SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1;   // Clear Interrupt flag
    PieCtrlRegs.PIEIER9.bit.INTx1 = 1;     

//	PieCtrlRegs.PIEACK.all|=0x100;       // Issue PIE ack

}

//---------------------------------------------------------------------------
// Example: InitSciGpio: 
//---------------------------------------------------------------------------
// This function initializes GPIO pins to function as SCI pins
//
// Each GPIO pin can be configured as a GPIO pin or up to 3 different
// peripheral functional pins. By default all pins come up as GPIO
// inputs after reset.  
// 
// Caution: 
// Only one GPIO pin should be enabled for SCITXDA/B operation.
// Only one GPIO pin shoudl be enabled for SCIRXDA/B operation. 
// Comment out other unwanted lines.

void InitSci(void)
{
  
	InitSci_();

	SciaRegs.SCIHBAUD    =0x0000;  // 38400  
	SciaRegs.SCILBAUD    =0x0030;
}

//===========================================================================
// End of file.
//===========================================================================
