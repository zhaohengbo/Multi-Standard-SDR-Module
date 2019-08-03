/****************************************************************************
 *		FILE:		host_interface.c										*
 *																			*
 *		TITLE:		Host-interface for S-FSK and OFDM SoM					*
 *					according to "PLC SoM API Specification"				*
 ****************************************************************************/

#include "main.h"


HOST_ITF hostItf;
extern Uint16 PduCnt;
Uint16 events;
Uint16 hostTmpBuf[128];
DATA_VECTOR_TYPE	hostTmpVector;


signed int hostAckTimeout, hostRxTimeout, i2cBusBusyTimeout;

// --------------------------------------------------------------------------

// Local funcs

void HostInit( Uint16 iFaceType )
{
	InitSci();
	InitSpi();
	InitI2CA();

	hostItf.txFrame.length = 0;
	hostItf.txFrame.offset = 0;

	hostItf.rxFrame.length = 0;
	hostItf.rxFrame.offset = 0;
	hostItf.rxFrame.step = STEP_PREFIX;

	hostItf.state = HOST_STATE_IDLE;

	events = 0;

	hostItf.txFrame.iface = iFaceType;

}
// --------------------------------------------------------------------------

unsigned int HostSend( DATA_VECTOR_TYPE *hostData, unsigned int cmd)
{
	Uint16 crc;

	if ( (hostItf.state&HOST_STATE_TX) || (hostData->length > (2*HOST_TX_BUFF_LEN-4)) )
		return 0;

	if( hostData->length && (hostData->buffer != 0) )	{
		CopyByteBuf( hostItf.txFrame.buffer, 4, hostData->buffer, hostData->offset, hostData->length );
	}

	switch( hostItf.txFrame.iface ) 
	{
	case IfaceSCI:
		hostItf.txFrame.buffer[0] = SCI_PREFIX;
		break;
	case IfaceI2C:
		hostItf.txFrame.buffer[0] = I2C_PREFIX;
		break;
	case IfaceSPI:
		hostItf.txFrame.buffer[0] = SPI_PREFIX;
		break;
	}
	
	hostItf.txFrame.buffer[1] = (cmd|0x80)<<8;

	__byte((int*)hostItf.txFrame.buffer,1) = (hostData->length+1);
	__byte((int*)hostItf.txFrame.buffer,2) = (hostData->length+1)>>8;

	hostItf.txFrame.length = hostData->length + 6;

	switch( hostItf.txFrame.iface ) 
	{
	case IfaceSPI:
		hostItf.txFrame.length++;
		__byte((int*)hostItf.txFrame.buffer,hostData->length+6) = 0xFF; //
	case IfaceSCI:
		crc = CalcCRC16( hostItf.txFrame.buffer, hostData->length+3, 1 );
		break;
	case IfaceI2C:
		crc = CalcCRC16( hostItf.txFrame.buffer, hostData->length+4, 0 );
		break;
	}
	
	CopyByteBuf( hostItf.txFrame.buffer, hostData->length+4, &crc, 0, 2 ); // copy crc into packet

	hostItf.txFrame.offset = 0;
	hostItf.txFrame.trys = 0;

	return 1;
}
//--------------------------------------------------------------------------

unsigned int HostReceive(unsigned int *buf,unsigned int len, unsigned int ifaceType )
{
	unsigned int rxffil = 1, prf;

	if( 	((hostItf.state & HOST_STATE_TX) != 0)   )
	{
		return rxffil;
	}
	else{
		switch( hostItf.rxFrame.step ){
			case STEP_DATA:
			    CopyByteBuf( hostItf.rxFrame.buffer, hostItf.rxFrame.offset, buf, 0, len );

				hostItf.rxFrame.offset += len;
				rxffil = hostItf.rxFrame.length - hostItf.rxFrame.offset;

				if( rxffil )
				{
					if( rxffil > HOST_RECEIVE_BLOCK_SIZE )
					{
						if( rxffil == (HOST_RECEIVE_BLOCK_SIZE+1) )
							rxffil = HOST_RECEIVE_BLOCK_SIZE-1;
						else
							rxffil = HOST_RECEIVE_BLOCK_SIZE;

					}
				}
				else
				{
					
					hostItf.state &= ~HOST_STATE_RX;
					hostItf.rxFrame.step = STEP_PREFIX;
					rxffil = 1;
					hostItf.rxFrame.iface = ifaceType;
				}
				break;

			case STEP_LEN_CMD:				
				if( buf[0] < (HOST_RX_BUFF_LEN*2-5) ){
					CopyByteBuf( hostItf.rxFrame.buffer, hostItf.rxFrame.offset, buf, 0, 3 );
	
					hostItf.rxFrame.length = buf[0]+5;
					hostItf.rxFrame.offset += 3;


					rxffil = hostItf.rxFrame.length - hostItf.rxFrame.offset;
					
					if( rxffil > HOST_RECEIVE_BLOCK_SIZE )
					{
						if( rxffil == (HOST_RECEIVE_BLOCK_SIZE+1) )
							rxffil = HOST_RECEIVE_BLOCK_SIZE-1;
						else
							rxffil = HOST_RECEIVE_BLOCK_SIZE;

					}	
					hostItf.rxFrame.step = STEP_DATA;
				}
				else {
					hostItf.rxFrame.step = STEP_PREFIX;
					hostItf.state &= ~HOST_STATE_RX;
					hostItf.rxFrame.length = 0;
					hostItf.rxFrame.offset = 0;
				}

				break;

			default:
				prf = buf[0]&0x00FF;
				if( (prf == SCI_PREFIX) || (prf == (I2C_SLAVE_ADDRESS<<1)) || (prf == SPI_PREFIX) ){
					if( prf == SCI_PREFIX || prf == SPI_PREFIX ){
						__byte((int*)hostItf.rxFrame.buffer,0) = 0;
					}
					else{
						__byte((int*)hostItf.rxFrame.buffer,0) = I2C_SLAVE_ADDRESS<<1;
					}
					rxffil = 3;
					hostItf.rxFrame.offset = 1;
					hostItf.rxFrame.step = STEP_LEN_CMD;

					hostItf.state |= HOST_STATE_RX;

				}
				else{
					hostItf.rxFrame.step = STEP_PREFIX;
				}
				break;
		}
		return rxffil;
	}
}


void HostTick( void )
{
	if( hostItf.rxFrame.length )
	{
		if( (hostItf.state&HOST_STATE_RX) == 0)
		{
			if( CalcCRC16( hostItf.rxFrame.buffer, hostItf.rxFrame.length, 0) == 0 )
			{
				hostItf.txFrame.iface = hostItf.rxFrame.iface;
				HostCmdProcess();
			}
			hostItf.rxFrame.length = 0;
		}
	}


	if( hostItf.txFrame.length )
	{


		if( (hostItf.state&HOST_STATE_TX) == 0 &&
			hostItf.txFrame.offset == hostItf.txFrame.length &&
			(hostItf.txFrame.buffer[1]>>8) != HOST_CMD_DL_SERVICE ) 
		{
			hostItf.txFrame.length = 0;
		}
        else if( (hostItf.state&HOST_STATE_TX) == 0 && hostItf.txFrame.offset == 0 )
		{
			hostItf.txFrame.offset = 0;
			hostItf.state |= HOST_STATE_TX;

			
			switch( hostItf.txFrame.iface ) 
			{
			case IfaceSCI:
			    SciaRegs.SCIFFTX.bit.TXFIFOXRESET=1;
				break;
			case IfaceSPI:
//				LedTgl();
				SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;
				SpiaRegs.SPIFFRX.bit.RXFIFORESET=0;
				SpiaRegs.SPIFFTX.bit.TXFIFO=1;
				SpiaRegs.SPITXBUF = 0xFF00;
				break;
			case IfaceI2C:
				I2caRegs.I2CCNT = hostItf.txFrame.length;
				I2caRegs.I2CFFTX.all = 0x6060;	// I2CFFEN,TXFFRST,TXFFINTCLR,TXFFIENA,TXFFIL=0
				I2caRegs.I2CMDR.all = 0x2E20;// STT,STP,MST,TRX,IRS
				break;
			}
		}
	}
}
// --------------------------------------------------------------------------

void HostSendOK( void )
{
	DATA_VECTOR_TYPE ok;
	ok.length = 0;
	ok.offset = 0;
	HostSend( &ok, HOST_CMD_OK );
}
// --------------------------------------------------------------------------

void HostSendError( Uint16 errorCode )
{
	DATA_VECTOR_TYPE error;
	error.buffer = hostTmpBuf;
	error.length = 1;
	error.offset = 0;
	hostTmpBuf[0] = errorCode;
	HostSend( &error, HOST_CMD_ERROR );
}
// --------------------------------------------------------------------------

void SendGuiMessage( Uint16 messAge, Uint16 MsgSize, Uint16 DestMacAddr)
{	
	DATA_VECTOR_TYPE dlSend;
	Uint16 *messAge_ptr;

	if(PduCnt>255){          // this var must be incrimented for each message
        PduCnt=1;            // needs to wrap (reset) after 255
		}
	else{
		PduCnt++;
		}

	dlSend.buffer = hostTmpBuf;
//	dlSend.length = 7;  // this is in bytes
	dlSend.length = (MsgSize<<1)+5; // this is in bytes
	dlSend.offset = 0;
	hostTmpBuf[0] = 0x0;
	hostTmpBuf[0] |= DestMacAddr<<8;
	hostTmpBuf[1] =	DestMacAddr>>8;
	hostTmpBuf[1] = 0x33<<8; // specifies message to gui
	hostTmpBuf[2] = PduCnt;  
    messAge_ptr = (Uint16*)messAge;  

	CopyByteBuf(hostTmpBuf,5,messAge_ptr,0,MsgSize<<1);

//	hostTmpBuf[2] |= messAge<<8;
//	hostTmpBuf[3] = messAge>>8;

	HostSend( &dlSend, HOST_CMD_DL_SERVICE );
}
// --------------------------------------------------------------------------

void HostDLSend( Uint16 messAge, Uint16 DestMacAddr)
{
	DATA_VECTOR_TYPE dlSend;
	dlSend.buffer = hostTmpBuf;
	dlSend.length = 5;
	dlSend.offset = 1;
	hostTmpBuf[0] = 0x0;
	hostTmpBuf[1] = DestMacAddr;
	hostTmpBuf[2] = messAge;
	HostSend( &dlSend, HOST_CMD_DL_SERVICE );
}
// --------------------------------------------------------------------------

void HostGetInfo( void )
{
	DATA_VECTOR_TYPE dataVect;
	dataVect.length = 0;
	dataVect.offset = 0;
	HostSend( &dataVect, HOST_CMD_GET_INFO );

}
// --------------------------------------------------------------------------

void HostCmdProcess( void )
{
	Uint16 cmd = (hostItf.rxFrame.buffer[1]>>8) & 0x00FF;
	Uint16 i,ack,msg_len,my_msg,msg_from,msg_ofset;

	// check for direction
	if( (cmd&0x80) != 0 )
	{
		return;
	}
	cmd &= 0x7F;


	switch( cmd )
	{
	case HOST_CMD_ERROR:																						// CM_ERROR (0)
		{
			Uint16 error = hostItf.rxFrame.buffer[2] & 0x00FF;
			if(  hostItf.txFrame.length != 0 &&
				(hostItf.state&HOST_STATE_TX) != 0 &&
				(error==HOST_ERROR_RUN_CMD || error==HOST_ERROR_CRC || error==HOST_ERROR_BUSY) )
			{ 
				// trying to resend in HostTick()
				hostItf.state &= ~HOST_STATE_TX;
			}
		}
		break;

	case HOST_CMD_OK:																							// CMD_OK (1)
		if(  hostItf.txFrame.length != 0 && (hostItf.state&HOST_STATE_TX) == 0 )
		{
			hostItf.txFrame.length = 0;
			hostItf.txFrame.offset = 0;
		}
		break;

	case HOST_CMD_GET_INFO:																						// CMD_GET_INFO (2)
		{
			HostSendOK(); //4 test only
		}
		break;

	case HOST_CMD_DL_SERVICE:
		{

  			// determine who sent message
			msg_from = hostItf.rxFrame.buffer[2]>>8;
			msg_from |= hostItf.rxFrame.buffer[3]<<8;
			// determine # bytes of data
			msg_len = hostItf.rxFrame.buffer[0]>>8;
			msg_len |= hostItf.rxFrame.buffer[1]<<8;
			// adjust offset for "other" bytes			
			msg_ofset = msg_len+3;
			
				
			if(0x33 == hostItf.rxFrame.buffer[3]>>8)  //if msg was sent by GUI
			{   
				// print message from GUI			
				for (i=9; i < msg_ofset; i++) {
			        my_msg =  __byte((int*)hostItf.rxFrame.buffer,i);
					// insert I/O-Print func here 
        		}
				
				// ack message from gui
				ack = 0x34;		   // standard response
				ack |= PduCnt<<8;  // tell gui what the message # was recieved
				HostDLSend(ack,msg_from);
			}
			 
			else
			{
				msg_ofset = msg_len+3;	
			
				// print data recieved			
				for (i=7; i < msg_ofset; i++) {
			    	my_msg =  __byte((int*)hostItf.rxFrame.buffer,i);
					// insert I/O-Print func here 
        		}
			}
			
		}
		break;

	default:
		HostSendError(HOST_ERROR_UNSUPP_CMD);
		break;
	}

}
// --------------------------------------------------------------------------


