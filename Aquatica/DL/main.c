#include <msp430f247.h>
#include <stdlib.h>
#include "SERIALDL.h"
#include "../uwicmp.h"
//#include "../uw_ids.h"

/* DL_init:
*Tarefa: Inicializacoes a serem feitas pela DL
*/
void DL_init(){

}


void main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  struct Packet pckt;
  struct Uw_Packet uwpckt;
  P1DIR = 0x1E;
  P1OUT = 0x1E;
  uwpckt.data[0] = 0;
	UWICMP_init();
  SERIAL_init(9600,0,0,0,0,0);
  SERIAL_disable_interruption_receive(0);
  SERIAL_disable_interruption_receive(1);
  
  while(1){
    SERIAL_enable_interruption_receive(0);
    __bis_SR_register(LPM0_bits + GIE);
    SERIAL_disable_interruption_receive(0);
    
    if(SERIAL_has_new_msg(0)){
      pckt = *(struct Packet *)rx_buffer;
      SERIAL_clear_buffer();
      P1OUT &= 0xE1;
      P1OUT |= (char) ((pckt.hdr.un.echo.sequence & 0x0F00) >> 7); 		//MSB
      UWICMP_compress(&pckt, &uwpckt);
      SERIAL_write_vector(1,(unsigned char *)&uwpckt,UWSIZE);
      SERIAL_enable_interruption_receive(1);
      __bis_SR_register(LPM0_bits + GIE);
      SERIAL_disable_interruption_receive(1);
      if(SERIAL_has_new_msg(1)){
        uwpckt = *(struct Uw_Packet *)rx_buffer;
        SERIAL_clear_buffer();
        UWICMP_decompress(&uwpckt, &pckt);
        SERIAL_write_vector(0,(unsigned char*)&pckt,SIZE);        
      }        
    }
    
  }
}
