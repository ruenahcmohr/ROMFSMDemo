/***********************************************************************************

Encoder tracker

INPUTS
 - paws 1 bit

OUTPUTS
 - 4 bits for digit
 
FEEDBACK
 - output bits (4).



Address bits      8 bit rom size

       -- no parallel roms available --
     8                  2 k
     9                  4 k
     10                 8 k
     
       -- eeproms available from here --
     11                 16 k  (28C16)
     12                 32 k  (28C32)
     
       -- eprom practical sizes from here --
     13                 64 k  (2764)
     14                 128 k (27128)
     15                 256 k 
     16                 512 k
     17                 1 M  (27010)
     18                 2 M
     19                 4 M
     20                 8 M

       -- flash from here up --



**************************************************************************************/


#include <stdio.h>
#include <stdint.h>
#include "ROMLib.h"


// the number of address lines you need
// this project has 4 feedback bits and 1 input
#define InputBits 5

// the output data size, 8 or 16
#define OutputBits 8

// default output value    Due to a new 'or' preference this is broken, leave at 0
#define DFOutput  0x00




int main(void) {

  // (I)nputs are values, we will buid them from the 'bit field' on the address lines
  uint16_t pawsI, lastDigitI;                       // values are 16 bits      
  
  // (O)utputs are values, that we make in our 'task' code, they will made into a 'bit field' for the ROM outputs
  uint32_t thisDigitO ;                             // bit fields are 32 bits
  

  uint32_t out; // system thing
  setup();      // open output file.
  
  // loop thru each address
  for( A=0; A<(1<<InputBits); A++) { // loop thru all the addresses in our project
  
     // assign default values for outputs
     thisDigitO = 0;
     
     out        = 0; // DFOutput;
     
     // build input values from address bitfield
     spliceValueFromField( &lastDigitI,        A,  4,   0,1,2,3);  
     spliceValueFromField( &pawsI,             A,  1,   4);       

     // do task
     // format:  if (--)       variable=...

     //if ((mode==0)&&(ramp>24))                drive     = 1;
     //if (((mode==0)&&(ramp>24))||(mode==1))   highspeed = 1;
     //if ((mode==1)&&(ramp<setpoint))          drive     = 1;          
     //if (1)                                   countO    = countI + offsets[encDeltaStateI];  // the output splicing will auto limit to 6 bits.     
     //if (1)                                   oldencO   = currencI;
     
     // machine will start at address 0, example sequence is    'CAFE42'     
     if (lastDigitI ==  0)                      thisDigitO = 0x0C; // this is just an initial condition
     
     if (lastDigitI == 0x0C)                    thisDigitO = 0x0A; 
     if (lastDigitI == 0x0A)                    thisDigitO = 0x0F;
     if (lastDigitI == 0x0F)                    thisDigitO = 0x0E;
     if (lastDigitI == 0x0E)                    thisDigitO = 0x04;
     if (lastDigitI == 0x04)                    thisDigitO = 0x02;
     if (lastDigitI == 0x02)                    thisDigitO = 0x0C; // go back to start
     
     // if the paws signal is high, stay on the same state.
     if (pawsI == 1)                            thisDigitO = lastDigitI; // this will override any of the prev stuff.
   
     // reconstitute the output
     spliceFieldFromValue( &out, thisDigitO,     4,  0,1,2,3);     
     
     // submit to file
     write(fd, &out, OutputBits>>3);  
  }
  
  cleanup(); // close file
  return 0;
}




