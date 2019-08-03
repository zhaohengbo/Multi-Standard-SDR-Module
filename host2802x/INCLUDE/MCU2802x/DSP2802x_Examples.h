// TI File $Revision: /main/7 $
// Checkin $Date: November 7, 2008   16:44:51 $
//###########################################################################
//
// FILE:   DSP2802x_Examples.h
//
// TITLE:  DSP2802x Device Definitions.
//
//###########################################################################
// $TI Release: 2802x Header Files V1.00 $
// $Release Date: November 10, 2008 $
//###########################################################################

#ifndef DSP2802x_EXAMPLES_H
#define DSP2802x_EXAMPLES_H

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------
      Specify the PLL control register (PLLCR) and divide select (DIVSEL) value.
-----------------------------------------------------------------------------*/

//#define DSP28_DIVSEL   0 // Enable /4 for SYSCLKOUT
//#define DSP28_DIVSEL   1 // Disable /4 for SYSCKOUT
#define DSP28_DIVSEL   2 // Enable /2 for SYSCLKOUT
//#define DSP28_DIVSEL   3 // Enable /1 for SYSCLKOUT


#define DSP28_PLLCR   12    // Uncomment for 60 MHz devices [60 MHz = (10MHz * 12)/2]
//#define DSP28_PLLCR   11
//#define DSP28_PLLCR   10
//#define DSP28_PLLCR    9
//#define DSP28_PLLCR    8      // Uncomment for 40 MHz devices [40 MHz = (10MHz * 8)/2]
//#define DSP28_PLLCR    7
//#define DSP28_PLLCR    6
//#define DSP28_PLLCR    5
//#define DSP28_PLLCR    4
//#define DSP28_PLLCR    3
//#define DSP28_PLLCR    2
//#define DSP28_PLLCR    1
//#define DSP28_PLLCR    0  // PLL is bypassed in this mode
//----------------------------------------------------------------------------

/*-----------------------------------------------------------------------------
      Specify the clock rate of the CPU (SYSCLKOUT) in nS.

      Take into account the input clock frequency and the PLL multiplier
      selected in step 1.

      Use one of the values provided, or define your own.
      The trailing L is required tells the compiler to treat
      the number as a 64-bit value.

      Only one statement should be uncommented.

      Example 1: 40 MHz devices:
                 CLKIN is a 10 MHz crystal or internal 10 MHz oscillator

                 In step 1 the user specified PLLCR = 0x8 for a
                 40 MHz CPU clock (SYSCLKOUT = 40 MHz).

                 In this case, the CPU_RATE will be 25.000L
                 Uncomment the line: #define CPU_RATE 25.000L

      Example 2: 60 MHz devices:
                 CLKIN is a 10 MHz crystal or internal 10 MHz oscillator

                 In step 1 the user specified PLLCR = 0xC for a
                 60 MHz CPU clock (SYSCLKOUT = 60 MHz).

                 In this case, the CPU_RATE will be 16.667L
                 Uncomment the line: #define CPU_RATE 16.667L

-----------------------------------------------------------------------------*/

#define CPU_RATE   16.667L   // for a 60MHz CPU clock speed (SYSCLKOUT)
//#define CPU_RATE   20.000L   // for a 50MHz CPU clock speed  (SYSCLKOUT)
//#define CPU_RATE   25.000L   // for a 40MHz CPU clock speed  (SYSCLKOUT)
//#define CPU_RATE   33.333L   // for a 30MHz CPU clock speed  (SYSCLKOUT)
//#define CPU_RATE   41.667L   // for a 24MHz CPU clock speed  (SYSCLKOUT)
//#define CPU_RATE   50.000L   // for a 20MHz CPU clock speed  (SYSCLKOUT)
//#define CPU_RATE   66.667L   // for a 15MHz CPU clock speed  (SYSCLKOUT)
//#define CPU_RATE  100.000L   // for a 10MHz CPU clock speed  (SYSCLKOUT)

//----------------------------------------------------------------------------


/*-----------------------------------------------------------------------------
      Target device (in DSP2802x_Device.h) determines CPU frequency
      (for examples) - either 60 MHz (for 28026 and 28027) or 40 MHz
      (for 28025, 28024, 28023, and 28022).
      User does not have to change anything here.
-----------------------------------------------------------------------------*/
#if (DSP28_28026||DSP28_28027)    // DSP28_28026||DSP28_28027 devices only
  #define CPU_FRQ_60MHZ    1     // 60 Mhz CPU Freq (10 MHz input clock)
  #define CPU_FRQ_40MHZ    0
#else
  #define CPU_FRQ_60MHZ    0     // DSP28_28025||28024||28023||28022 devices
  #define CPU_FRQ_40MHZ    1     // 40 MHz CPU Freq (10 MHz input clock) by DEFAULT
#endif


// The following pointer to a function call calibrates the ADC and internal oscillators
#define Device_cal (void   (*)(void))0x3D7C80

//---------------------------------------------------------------------------
// Include Example Header Files:
//

#include "DSP2802x_GlobalPrototypes.h"         // Prototypes for global functions within the
                                              // .c files.
#include "DSP2802x_EPwm_defines.h"             // Macros used for PWM examples.
#include "DSP2802x_I2C_defines.h"              // Macros used for I2C examples.

#define PARTNO_28027  0xCF
#define PARTNO_28026  0xC7
#define PARTNO_28025  0xCE
#define PARTNO_28024  0xC6
#define PARTNO_28023  0xCD
#define PARTNO_28022  0xC5

// Include files not used with DSP/BIOS
#ifndef DSP28_BIOS
#include "DSP2802x_DefaultISR.h"
#endif

// DO NOT MODIFY THIS LINE.
#define DELAY_US(A)  DSP28x_usDelay(((((long double) A * 1000.0L) / (long double)CPU_RATE) - 9.0L) / 5.0L)

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  // end of DSP2802x_EXAMPLES_H definition

//===========================================================================
// End of file.
//===========================================================================
