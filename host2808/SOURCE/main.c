/****************************************************************************
 *		FILE:		main.c													*
 *																			*
 *		TITLE:		Application entry point									*
 ****************************************************************************/

#include "main.h"

DATA_VECTOR_TYPE dataVect;

//-----------------------------------------------------------------------------
int main(void)
{
	InitSysCtrl();
	InitGpio();
	InitPieCtrl();

	IER = 0x0000;
	IFR = 0x0000;

	InitPieVectTable();

	HostInit( IfaceSCI );	


// Enable interrupts
   PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
   IER |= 0x100;	// Enable CPU INT

// Enable global Interrupts and higher priority real-time debug events:
   EINT;   // Enable Global interrupt INTM

	dataVect.length = 0;
	dataVect.offset = 0;

	HostSend( &dataVect, HOST_CMD_GET_INFO ); // create the packet only

	while(1){
		HostTick();
	}
}

//-----------------------------------------------------------------------------

Uint16 CalcCRC16(unsigned int *buf, unsigned int lenBytes,unsigned int offset)
{
	unsigned int	crc = 0;
	unsigned int	c;
	unsigned int	i;

	while ( lenBytes )
	{
		if(!offset){
			lenBytes--;
			c = ( ((*buf)&0xFF) ^ crc ) & 0xFF;
			i = 8;
			do {
				if ( c & 1 ) {
					c >>= 1;
					c ^= 0xA001;
				}
				else c >>= 1;
			} while( --i );
			crc = (crc>>8) ^ c;
		}
		if( lenBytes )
		{
			offset = 0;
			lenBytes--;
			c = ((((*buf++)>>8)&0xFF) ^ crc) & 0xFF;
			i = 8;
			do {
				if ( c & 1 ) {
					c >>= 1;
					c ^= 0xA001;
				}
				else c >>= 1;
			} while( --i );
			crc = (crc>>8) ^ c;
		}
	}

	return crc;
}
