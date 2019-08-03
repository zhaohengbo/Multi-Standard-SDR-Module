//###########################################################################
//
// FILE:	Sci.h
//
// TITLE:	DSP280x SCI Initialization & Support Functions.
//
//###########################################################################

#ifndef __SCI
#define __SCI

interrupt void sciaTxFifoIsr(void);
interrupt void sciaRxFifoIsr(void);

#endif // __SCI
