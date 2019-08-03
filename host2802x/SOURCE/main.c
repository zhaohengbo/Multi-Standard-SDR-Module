/****************************************************************************
 *		FILE:		main.c													*
 *																			*
 *		TITLE:		Application entry point									*
 ****************************************************************************/

#include "main.h"
Uint16 PduCnt,data,size;



//-----------------------------------------------------------------------------
int main(void)
{
	//char sms[64]="Texas Instruments Power Line Communication Solutions";
    
    
    
    
    //Uint16 sms[]={0x5465,0x7861,0x7320,0x496e,0x7374,0x7275,0x6d65,0x6e74,
    //0x7320,0x506f,0x7765,0x7220,0x4c69,0x6e65,0x2043,0x6f6d,0x6d75,0x6e69,
    //0x6361,0x7469,0x6f6e,0x2053,0x6f6c,0x7574,0x696f,0x6e73};

	Uint16 sms[]={0x6554,0x6178,0x2073,0x6e49,0x7473,0x7572,0x656d,0x746e,
	0x2073,0x6f50,0x6577,0x2072,0x694c,0x656e,0x4320,0x6d6f,0x756d,0x696e,
	0x6163,0x6974,0x6e6f,0x5320,0x6c6f,0x7475,0x6f69,0x736e};

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


	//HostGetInfo();  // create the GET_INFO packet 
	//HostTick();     // send it!

	PduCnt=0;  //init Gui message counter
	data=0x3431;
	data=(Uint32)&sms;
	size=sizeof(sms);
	SendGuiMessage(data,size,1);


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
