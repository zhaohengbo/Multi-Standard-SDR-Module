//###########################################################################
//
// FILE:	utils.h
//
// TITLE:	Support Functions.
//
//###########################################################################

Uint16 CalcCRC16(unsigned int *buf, unsigned int lenBytes,unsigned int offset);
void CopyByteBuf(	WORD *destination_buf, 
					WORD destination_offset, 
					WORD *source_buf, 
					WORD source_offset,
					WORD byte_length );
