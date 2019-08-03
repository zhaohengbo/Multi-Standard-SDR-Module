//###########################################################################
//
// FILE:	utils.h
//
// TITLE:	Support Functions.
//
//###########################################################################

Uint16 CalcCRC16(unsigned int *buf, unsigned int lenBytes,unsigned int offset);
void CopyByteBuf(	Uint16 *destination_buf, 
					Uint16 destination_offset, 
					Uint16 *source_buf, 
					Uint16 source_offset,
					Uint16 byte_length );
