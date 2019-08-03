/****************************************************************************
 *		FILE:		host_interface.h										*
 *																			*
 *		TITLE:		Host-interface for S-FSK and OFDM SoM					*
 *					according to "PLC SoM API Specification"				*
 ****************************************************************************/

#ifndef __HOST_INTERFACE
#define __HOST_INTERFACE


#define HOST_RX_BUFF_LEN 150 //Uint16s
#define HOST_TX_BUFF_LEN 150 //Uint16s

// --------------------------------------------------------------------------


typedef struct _DataVectorType
{
	Uint16 *buffer;
	Uint16 offset;
	Uint16 length;
}	DATA_VECTOR_TYPE;

typedef struct _HOST_ITF
{
	struct tx
	{
		unsigned int buffer[HOST_TX_BUFF_LEN];
		volatile unsigned int length;
		volatile unsigned int offset;
		volatile unsigned int trys;
		volatile unsigned int iface;
	} txFrame;

	 struct rx
	 {
		unsigned int buffer[HOST_RX_BUFF_LEN];
		volatile unsigned int step;
		volatile unsigned int offset;
		volatile unsigned int length;
		volatile unsigned int iface;
	} rxFrame;

	volatile unsigned int state;

} HOST_ITF;



typedef struct
{
	unsigned int deviceType;
	unsigned int softVerMajor		:8;
	unsigned int softVerMinor		:8;
	unsigned int softVerExtention	:8;
	unsigned int softVerBuild		:8;
	unsigned int hardVer			:8;
	unsigned int baudrateMask		:8;
	unsigned int receiveWindow;
	unsigned int canalMaxPackSize;
} DEV_INFO;

// --------------------------------------------------------------------------

enum _Ifaces
{
	IfaceSCI,
	IfaceI2C,
	IfaceSPI
};

#define HOST_MAX_SEND_TRYS			3
#define HOST_RECEIVE_BLOCK_SIZE 	3
#define HOST_DATA_HEADER_SIZE		1


#ifdef OFDM_LV
#define HOST_BAUD_RATE_MASK			0x3F
#endif

#ifdef SFSK
#define HOST_BAUD_RATE_MASK			0x1F
#endif


//define rx steps
#define STEP_PREFIX		0  
#define STEP_LEN_CMD	1
#define STEP_DATA		2

// state for hostItf
#define HOST_STATE_IDLE				0x0000
#define HOST_STATE_RX				0x0001
#define HOST_STATE_TX				0x0002

#define I2C_PREFIX					(I2caRegs.I2COAR<<1)
#define SCI_PREFIX					0xA1
#define SPI_PREFIX					0xA1

#define	HOST_CMD_ERROR				0x00
#define	HOST_CMD_OK					0x01
#define	HOST_CMD_GET_INFO			0x02
#define	HOST_CMD_DATA				0x10
#define	HOST_CMD_DL_SERVICE			0x0D



#define HOST_ERROR_UNSUPP_CMD		0x1
#define HOST_ERROR_UNSUPP_PARAM		0x2
#define HOST_ERROR_RUN_CMD			0x3
#define HOST_ERROR_TOO_LONG_DATA	0x4
#define HOST_ERROR_CRC 				0x5
#define HOST_ERROR_DUBLICATE		0x6
#define HOST_ERROR_BUSY				0x7
#define HOST_ERROR_NACK				0x8

#define HOST_BAUDRATE_MASK_4800		0x01
#define HOST_BAUDRATE_MASK_9600		0x02
#define HOST_BAUDRATE_MASK_19200	0x04
#define HOST_BAUDRATE_MASK_38400	0x08
#define HOST_BAUDRATE_MASK_57600	0x10
#define HOST_BAUDRATE_MASK_115200	0x20





// --------------------------------------------------------------------------

extern HOST_ITF hostItf;
extern DATA_VECTOR_TYPE	hostTmpVector;

// --------------------------------------------------------------------------

// Local funcs

void HostInit( Uint16 iFaceType );

unsigned int HostSend( DATA_VECTOR_TYPE *hostData, unsigned int cmd);
unsigned int HostReceive(unsigned int *buf,unsigned int len, unsigned int ifaceType );


void HostTick( void );
void HostCmdProcess( void );
void HostSendOK( void );
void HostSendError( Uint16 errorCode );
void HostDLSend( Uint16 messAge, Uint16 DestMacAddr);
void HostGetInfo( void );
void SendGuiMessage( Uint16 messAge, Uint16 MsgSize, Uint16 DestMacAddr);
#endif //__HOST_INTERFACE

