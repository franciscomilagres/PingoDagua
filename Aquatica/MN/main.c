#include <MSP430F2274.h>
#include "SERIAL.h"
#include "../uwicmp.h"


/*get_answer:
* Tarefa: Analisa a mensagem "ask" e retorna a resposta em
*ans, caso haja.
* parametro ans deve estar inicializado.
* Retorna: 1 caso haja resposta e 0 caso contrario.
*/
int get_answer(struct Uw_Packet ask, struct Uw_Packet *ans){
    u_int8_t value;
    
    value = (ask.hdr.byte1 & B1_TYPE) >> 6;
    if((value == 1) && ((ask.hdr.byte1&B1_CODE) == 0)){               //tratando id e code
      P1OUT &= 0xCF;
      P1OUT |= ((ask.hdr.byte3 & 0x03) << 6);  //sinalizando 2 bits menos significativos do SEQUENCE
      ans->hdr.byte1 &= ~B1_TYPE;      //zerando campo type (echo reply)
      ans->hdr.byte1 &= ~B1_CODE;      //zerando campo code
      
			ans->hdr.byte1 &= ~B1_CHECKSUM;		//zerando checksum;
			ans->hdr.byte2 &= ~B2_CHECKSUM;

      ans->hdr.byte2 &= ~B2_ECHO_ID;		//zerando ID
			ans->hdr.byte3 &= ~B3_ECHO_ID;
			ans->hdr.byte2 |= (ask.hdr.byte2 & B2_ECHO_ID);      //passando ID
      ans->hdr.byte3 |= (ask.hdr.byte3 & B3_ECHO_ID);
      
      ans->hdr.byte3 &= ~B3_ECHO_SEQ;								//zerando sequence
			ans->hdr.byte3 |= (ask.hdr.byte3 & B3_ECHO_SEQ); //passando sequence
      
			ans->data[0] = ask.data[0];						//passando data

			return 1;
    }
		return 0;
}

void main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  struct Uw_Packet uwask, uwans;
  
  P1DIR = 0xC0;
  P1OUT = 0xC0;
  
  SERIAL_init(9600,0,0,0,0,0);
  SERIAL_disable_interruption_receive();
  
  while(1){
    SERIAL_enable_interruption_receive();
    __bis_SR_register(LPM0_bits + GIE);
    SERIAL_disable_interruption_receive();
    if(SERIAL_has_new_msg()){
      uwask = *(struct Uw_Packet *)rx_buffer;
      if(get_answer(uwask, &uwans)){
	      UWICMP_setChecksum(&uwans, 4);
	      SERIAL_write_vector((unsigned char *)&uwans, 4);
			}
    }
  }
  
}
