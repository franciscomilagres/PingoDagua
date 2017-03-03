/*
*************************************************************************************
*                   UNIVERSIDADE FEDERAL DE MINAS GERAIS                            *
*   AQUANODE: PROTOTIPAÇÃO DE UM NÓ SENSOR PARA REDES DE SENSORES AQUÁTICAS         *
*************************************************************************************
*                         BIBLIOTECA DE COMUNICAÇÃO SERIAL                          *
*************************************************************************************
* DESENVOLVIDO POR:LABORATÓRIO DE ENGENHARIA DE SISTEMA DE COMPUTAÇÃO UFV-FLORESTAL *
* DATA:                                                                             *
* APROVADO POR:                                                                     *
* DATA:                                                                             *
* VERSÃO:                                                                           *
*************************************************************************************/

/**
* @file SERIAL.c
* @brief Esta biblioteca tem como objetivo possibilitar uma comunicacao via serial
*/

#include <MSP430F2274.h>
#include "SERIAL.h"

/**
* @var extern SERIAL_rxbuffer ponteiro unsigned char referente a buffer de
* armazenamento de valores lidos da serial
*/
unsigned char rx_buffer[BUFSIZE];
unsigned char *rx_buffer_pointer;
/**
* @var flag_new_msg variavel unsigned char referente a nova 
* mensagem recebida
*/
/**
* @var extern SERIAL_rxbuffer_size variavel short referente a tamanho do buffer de
* armazenamento de valores lidos da serial
*/
unsigned short rx_buffer_counter;
/**
* @var flag_new_msg variavel unsigned char referente a nova 
* mensagem recebida
*/
unsigned short flag_new_msg;

/*
* @var uwsize tamanho da mensagem a ser recebida (pode variar)
*/
unsigned short uwsize = UWSIZE;

/**
* @fn void init_clock_1MHZ(void)
* @brief garante que o clock seja de 1MZ
* @return Esta função não possui retorno
*/
void SERIAL_init_clock_1MHZ(void){
  
  BCSCTL2 = SELM_0 + DIVM_0 + DIVS_0;
  if (CALBC1_1MHZ != 0xFF){
    DCOCTL = 0x00;
    BCSCTL1 = CALBC1_1MHZ;/** Configura DCO para 1MHz */
    DCOCTL = CALDCO_1MHZ;
  }
  BCSCTL1 |= XT2OFF + DIVA_0;
  BCSCTL3 = XT2S_0 + LFXT1S_2 + XCAP_1;
}
/**
* @fn void SERIAL_init(int speed, int parity, int typeofparity, int dataorder, int stopbits, int databits)
* @brief Inicializa transmissao serial
* @param speed um argumento inteiro referente a velocidade de transmissao
* (38400 / 19200 / 9600 / 4800 / 2400)
* @param parity um argumento inteiro referente a paridade
* ( 1 - Habilita / 0 - Desabilita )
* @param type_of_parity um argumento inteiro referente a tipo de paridade
* ( Tipo de paridade: 1 - Impar / 0 - Par )
* @param data_order um argumento inteiro referente a ordem dos dados
* ( 1 - Mais significativo primeiro / 0 - Menos significativo primeiro )
* @param stop_bits um argumento inteiro referente a numero de bits de parada
* ( 0 - um stop bit / 1 - dois stop bits )
* @param data_bits um argumento inteiro referente a quantidade de bites de dados
* ( 0 - 8bits / 1 - 7bits )
* @param unsigned char *buffer ponteiro unsigned char referente ao buffer de recebimento.
* @return Esta função não possui retorno
*/
void SERIAL_init(int speed, int parity, int type_of_parity, int data_order, int stop_bits, int data_bits){
  
  // espera liberar o transmissor
  while(!(IFG2 & 2));
  //seleciona funcoes secundarias dos pinos P3.4 e P3.5:
  // P3.4 = TXD; P3.5= RXD
  P3SEL  = 0x30;
  // P3.4 (TXD) deve ser saída
  P3DIR |= 0x10;
  // P3.5 (RXD) deve ser entrada
  P3DIR &= ~0x20;
  
  // colocando os parametros nas suas devidas posiçoes dentro do registrador de cfg.
  // configura paridade, tipo de paridade, ordem dos dados, se tem ou nao stop bits
  // e numero de bits enviado de cada vez
  UCA0CTL0 |= parity         << 1;
  UCA0CTL0 |= type_of_parity << 1;
  UCA0CTL0 |= data_order     << 1;
  UCA0CTL0 |= stop_bits      << 1;
  UCA0CTL0 |= data_bits      << 3;
  
  /** @var bits_per_sec((CLOCK / (speed * 2)) * 2) definicao do valor em ms do intervalo de
  * envio/recebimento de cada bit
  */
  int  bits_per_sec = (CLOCK / (speed * 2)) * 2;
  
  // desabilita o hardware de comunicacao serial (UART) para configura-lo
  UCA0CTL1  = 1;
  // seleciona a fonte de clock para a comunicacao
  UCA0CTL1 |= CLOCKSOURCE;
  // Parte menos significativa do bits_per_sec (registrador que armazena o tempo de cada bit)
  UCA0BR0   = (bits_per_sec & 0xff);
  // Parte mais significativa do bits_per_sec (registrador que armazena o tempo de cada bit)
  UCA0BR1   = (bits_per_sec >> 8);
  // recebe o ajuste fino da configuracao da temporizacao de comunicacao
  UCA0MCTL  = (UCBRF << 4) | (UCBRS << 1);
  // Reativa UART desligando o UCSWRST
  UCA0CTL1 &= ~1;
  flag_new_msg = 0;
  rx_buffer_counter = 0;
  rx_buffer_pointer = rx_buffer;
  SERIAL_init_clock_1MHZ();
  __delay_cycles(1000);
  
}
/**
* @fn SERIAL_disable_interruption_receive(void)
* @brief Esta função desativa a interrupção de recebimento de mensagem
* @return Esta função não possui retorno
*/
void SERIAL_disable_interruption_receive(void){
     IE2 &= ~UCA0RXIE;
}
/**
* @fn SERIAL_enable_interruption_receive(void)
* @brief Esta função habilita a interrupção de recebimento de mensagem
* @return Esta função não possui retorno
*/
void SERIAL_enable_interruption_receive(void){
    IE2 |= UCA0RXIE; 
}
/**
* @fn unsigned int SERIAL_read_byte(unsigned char *data)
* @brief Leitura de um byte via SERIAL.
* @param unsigend char *data - Ponterio unsigend char para receber o dado lido
* @return Esta função não retorna valor.
*/
void  SERIAL_read_byte(unsigned char *data){
  
  rx_buffer_counter = 0;
  flag_new_msg = 0;
  rx_buffer_pointer = data;
  IE2 |= UCA0RXIE;
  __bis_SR_register(LPM0_bits + GIE);/// aciona interrupcoes LPM0 w/
  IE2 &= ~UCA0RXIE;
  rx_buffer_pointer = rx_buffer;
  rx_buffer_counter = 0;
  flag_new_msg = 0;
  
}
/**@fn unsigned char SERIAL_read_buffer(void)
*@brief Esta função ler o valor do buffer de msg recebida
*@return Esta função retorna o valor lido do buffer.
*/
void SERIAL_read_buffer(unsigned char *msg){
  short i;
  for(i = 1; i < rx_buffer_counter;i++){
    msg[i-1] = rx_buffer[i];
  }
  flag_new_msg = 0;
  rx_buffer_counter = 0;
}

/**
* @fn void SERIAL_clear_buffer(void);
* @brief Esta função limpa o buffer.
* @param Esta função não recebe parâmetro.
* @return Esta função não possui retorno
*/
void SERIAL_clear_buffer(void){
  short i;
  for(i=0;i<BUFSIZE;i++)
    rx_buffer[i] = 0;
}

/**@fn unsigned char SERIAL_has_new_msg(void)
*@brief Esta função retorna um indicardor de nova msg recebida
*@return Esta função retorna 1 se possui nova msg 0 se não possui.
*/
unsigned short SERIAL_has_new_msg(void){
     return flag_new_msg;
}
/**
* @fn void SERIAL_write_byte(char byte_data);
* @brief Escrita de um byte via SERIAL.
* @param byte_data um argumento char referente ao byte que deseja enviar
* @return Esta função não possui retorno
*/
void SERIAL_write_byte(unsigned char byte_data){
  
  /// espera liberar o transmissor
  while(!(IFG2 & 2));
  // transmite
  UCA0TXBUF = byte_data;
  /// limpa o indicador de transmissor livre
  IFG2 &= (~2);
  __delay_cycles(1000);
  
}

/**
* @fn void SERIAL_write_vector(char byte_data);
* @brief Escrita de vários bytes via SERIAL.
* @param byte_data ponteiro char que indica o vetor de bytes que se deseja enviar
* @param len - quantidade de bytes a serem escritos.
* @return Esta função não possui retorno
*/
void SERIAL_write_vector(unsigned char *byte_vector, unsigned short len){
  short i;
  for(i = 0; i < len; i++){  
      // Espera liberar o transmissor:
      while(!(IFG2 & 2));
      // Transmite byte a byte.
      UCA0TXBUF = byte_vector[i];
      // Limpa o indicador de transmissor livre.
      IFG2 &= 0xFB;
    }
}

/// deve-se colocar o vetor de interrupcoes de determinado dispositvo
#pragma vector=USCIAB0RX_VECTOR
/// interrupcao sera gerada quando recebe um dado
__interrupt void USCIA0RX_ISR (void){
  
  rx_buffer_pointer[rx_buffer_counter++] = UCA0RXBUF; 
  IFG2 &= (~1);//limpa flag de dado recebido
  
  if(rx_buffer_counter == 1){
        uwsize = ((rx_buffer_pointer[0]&0xc0) == 0xc0)?UWSIZE+1:uwsize;
        uwsize = (rx_buffer_pointer[0]&0x20)?uwsize+1:uwsize;
  }  
  else if(rx_buffer_counter >= uwsize){
        flag_new_msg = 1;
        rx_buffer_counter = 0;        				// Zerando contador.
        __bic_SR_register_on_exit(LPM0_bits + GIE); // Sai do estado de baixo consumo.
  }
} 
