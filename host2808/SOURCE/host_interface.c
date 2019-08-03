/****************************************************************************
 *		FILE:		host_interface.c										*
 *																			*
 *		TITLE:		Host-interface for S-FSK and OFDM SoM					*
 *					according to "PLC SoM API Specification"				*
 ****************************************************************************/

#include "main.h"


HOST_ITF hostItf;

WORD events;
WORD hostTmpBuf[50];
DATA_VECTOR_TYPE	hostTmpVector;


signed int hostAckTimeout, hostRxTimeout, i2cBusBusyTimeout;

// --------------------------------------------------------------------------

// Local funcs

void HostInit( WORD iFaceType )
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
			(hostItf.txFrame.buffer[1]>>8) != HOST_CMD_DATA ) 
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
				SpicRegs.SPICTL.bit.MASTER_SLAVE = 1;
				SpicRegs.SPIFFRX.bit.RXFIFORESET=0;
				SpicRegs.SPIFFTX.bit.TXFIFO=1;
				SpicRegs.SPITXBUF = 0xFF00;
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

void HostSendError( WORD errorCode )
{
	DATA_VECTOR_TYPE error;
	error.buffer = hostTmpBuf;
	error.length = 1;
	error.offset = 0;
	hostTmpBuf[0] = errorCode;
	HostSend( &error, HOST_CMD_ERROR );
}
// --------------------------------------------------------------------------

void HostCmdProcess( void )
{
	WORD cmd = (hostItf.rxFrame.buffer[1]>>8) & 0x00FF;

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
			WORD error = hostItf.rxFrame.buffer[2] & 0x00FF;
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

	default:
		HostSendError(HOST_ERROR_UNSUPP_CMD);
		break;
	}

}
// --------------------------------------------------------------------------


