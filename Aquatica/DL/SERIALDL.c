/*
*************************************************************************************
*                   UNIVERSIDADE FEDERAL DE MINAS GERAIS                            *
*   AQUANODE: PROTOTIPAÇÃO DE UM NÓ SENSOR PARA REDES DE SENSORES AQUÁTICAS         *
*************************************************************************************
*                         BIBLIOTECA DE COMUNICAÇÃO SERIAL                          *
*************************************************************************************
* DESENVOLVIDO POR:LABORATÓRIO DE ENGENHARIA DE SISTEMA DE COMPUTAÇÃO UFV-FLORESTAL *
* ALTERADO POR: LABORATORIO DE ENGENHARIA E COMPUTADORES (UFMG)                     *
* DATA:                                                                             *
* APROVADO POR:                                                                     *
* DATA:                                                                             *
* VERSÃO:                                                                           *
*************************************************************************************/

/**
* @file SERIALDL.c
* @brief Esta biblioteca tem como objetivo possibilitar uma comunicacao via serial
*/

#include <msp430f247.h>
#include "SERIALDL.h"

/**
* @var rx_buffer ponteiro unsigned char referente a buffer de
* armazenamento de valores lidos da serial
* @var uwsize tamanho esperado da msg underwater
*/
unsigned char rx_buffer[SIZE];
unsigned char *rx_buffer_pointer;
unsigned short uwsize = UWSIZE;
/**
* @var extern SERIAL_rxbuffer_size variavel short referente a tamanho do buffer de
* armazenamento de valores lidos da serial
*/
unsigned short rx_buffer_counter;
/**
* @var flag1_new_msg variavel unsigned char referente a nova 
* mensagem recebida na USCI_A0.
* @var flag2_new_msg variável unsigned char referente a nova
* mensagem recebida na USCI_A1.
*/
unsigned short flag1_new_msg;
unsigned short flag2_new_msg;
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
  
  // Espera liberar o transmissor.
  while(!(IFG2 & 2));
  // Seleciona funcoes secundarias dos pinos P3.4 e P3.5:
  // P3.4 = UCA0TXD; P3.5= UCA0RXD; P3.6 = UCA1TXD ; P3.7 = UCA1RXD
  P3SEL  = 0xF0;
  // P3.4 e P3.6 (TXD's) devem ser saídas.
  P3DIR |= 0x50;
  // P3.5 e P3.7(RXD's) deve ser entrada.
  P3DIR &= 0x5F;
  
  // Colocando os parametros nas suas devidas posições dentro do registrador de cfg.
  // configura paridade, tipo de paridade, ordem dos dados, se tem ou nao stop bits
  // e numero de bits enviado de cada vez.
  UCA0CTL0 |= parity         << 7;              //BIT 7 = UC Parity Enable.
  if(parity)
    UCA0CTL0 |= type_of_parity << 6;            //BIT 6 = UC Parity(usado se a paridade esta habilitada).
  UCA0CTL0 |= data_order     << 5;              //BIT 5 = UCMSB.
  UCA0CTL0 |= stop_bits      << 3;              //BIT 3 = UC StoP Bits.
  UCA0CTL0 |= data_bits      << 4;              //BIT 4 = UC7BIT.
  
  // Faz o mesmo para a outra porta serial.
  UCA1CTL0 |= parity         << 7;              //BIT 7 = UC Parity Enable.
  if(parity)
    UCA1CTL0 |= type_of_parity << 6;            //BIT 6 = UC Parity(usado se a paridade esta habilitada).
  UCA1CTL0 |= data_order     << 5;              //BIT 5 = UCMSB.
  UCA1CTL0 |= stop_bits      << 3;              //BIT 3 = UC StoP Bits.
  UCA1CTL0 |= data_bits      << 4;              //BIT 4 = UC7BIT.  
  
  /** @var bits_per_sec((CLOCK / (speed * 2)) * 2) definicao do valor em ms do intervalo de
  * envio/recebimento de cada bit
  */
  int  bits_per_sec = (CLOCK / (speed * 2)) * 2;
  
  // Desabilita o hardware de comunicacao serial 0(UART0) para configura-lo.
  UCA0CTL1  = 1;
  // Seleciona a fonte de clock para a comunicação.
  UCA0CTL1 |= CLOCKSOURCE;
  // Parte menos significativa do bits_per_sec(registrador que armazena o tempo de cada bit).
  UCA0BR0   = (bits_per_sec & 0xff);
  // Parte mais significativa do bits_per_sec(registrador que armazena o tempo de cada bit).
  UCA0BR1   = (bits_per_sec >> 8);
  // Recebe o ajuste fino da configuração da temporização de comunicação.
  UCA0MCTL  = (UCBRF << 4) | (UCBRS << 1);
  // Reativa UART0 desligando o UCSWRST.
  UCA0CTL1 &= ~1;
  
  // Aplicando o mesmo procedimento com o UART 1:
  UCA1CTL1 = 1;                             // Desabilita.
  UCA1CTL1 |= CLOCKSOURCE;                  // Seleciona a fonte de clock.
  UCA1BR0 = (bits_per_sec & 0xff);          // Parte menos significativa.
  UCA1BR1 = (bits_per_sec >>8);             // Parte mais significativa.
  UCA1MCTL = (UCBRF << 4) | (UCBRS << 1);       // Ajuste fino.
  UCA1CTL1 &= ~1;
  
  // Limpando flags de nova mensagem e buffer:
  flag1_new_msg = 0;                        
  flag2_new_msg = 0;
  rx_buffer_counter = 0;
  rx_buffer_pointer = rx_buffer;            // Configurando o apontador para o buffer.
  
  SERIAL_init_clock_1MHZ();
  __delay_cycles(1000);
  
}
/**
* @fn SERIAL_disable_interruption_receive(void)
* @brief Esta função desativa a interrupção de recebimento de mensagem da porta desejada
* @param port - argumento char referente à porta que se deseja desabilitar
* (0 - UCA0 [PC] / 1 - UCA1 [Sonda] )
* @return Esta função não possui retorno
*/
void SERIAL_disable_interruption_receive(char port){
  if(port)                              // Se port for 1, desabilita interrupção da UCA1.
    UC1IE &= ~UCA1RXIE;
  else                                  // Caso for 0, desabilita interrupção da UCA0.
    IE2 &= ~UCA0RXIE;
}
/**
* @fn SERIAL_enable_interruption_receive(void)
* @brief Esta função habilita a interrupção de recebimento de mensagem da porta desejada
* @param port - argumento char referente à porta que se deseja habilitar
* @return Esta função não possui retorno
*/
void SERIAL_enable_interruption_receive(char port){
  if(port)                              // Se port for 1, habilita interrupção da UCA1.
    UC1IE |= UCA1RXIE;
  else                                  // Caso for 0, habilita interrupção da UCA0.
    IE2 |= UCA0RXIE;
}
/**
* @fn unsigned int SERIAL_read_byte(unsigned char *data)
* @brief Leitura de um byte via SERIAL.
* @param unsigend char *data - Ponterio unsigend char para receber o dado lido
* @param port - argumento char referente à porta de onde será feita a leitura
* (0 - UCA0 [PC] / 1 - UCA1 [Sonda] )
* @return Esta função não retorna valor.
*/
void  SERIAL_read_byte(char port, unsigned char *data){
  
  rx_buffer_counter = 0;
  rx_buffer_pointer = data;
  if(port){
    flag2_new_msg = 0;
    UC1IE |= UCA1RXIE;
    __bis_SR_register(LPM0_bits + GIE);         // Aciona interrupções LPM0.
    UC1IE &= ~UCA1RXIE;
    flag2_new_msg = 0;
  }
  else{
    flag1_new_msg = 0;  
    IE2 |= UCA0RXIE;
    __bis_SR_register(LPM0_bits + GIE);     // Aciona interrupcoes LPM0 w/
    IE2 &= ~UCA0RXIE;
    flag1_new_msg = 0;
  }
  rx_buffer_pointer = rx_buffer;
  rx_buffer_counter = 0;
  
}

/**@fn unsigned char SERIAL_has_new_msg(void)
*@brief Esta função retorna um indicardor de nova msg recebida
*@param port - argumento referente à porta de onde se deseja verificar.
*@return Esta função retorna 1 se possui nova msg 0 se não possui.
*/
unsigned short SERIAL_has_new_msg(char port){
  char resp;
  if(port){                             // Se for da UART 1.
    resp = flag2_new_msg;
    flag2_new_msg = 0;
  }
  else{                                 // Se for da UART 0.
    resp = flag1_new_msg;
    flag1_new_msg = 0;
  }
  return resp;
}
/**
* @fn void SERIAL_write_byte(char byte_data);
* @brief Escrita de um byte via SERIAL.
* @param byte_data um argumento char referente ao byte que deseja enviar
* @param port - argumento char referente à porta para onde irá enviar
* @return Esta função não possui retorno
*/
void SERIAL_write_byte(char port, unsigned char byte_data){
  
  if(port){
    // Aguarda liberar o transmissor:
    while(!(UC1IFG & 2));
    // Transmite o dado.
    UCA1TXBUF = byte_data;
    // Limpa o indicador de transmissor livre.
    UC1IFG &= 0xFB;
  }
  else{
    // Espera liberar o transmissor:
    while(!(IFG2 & 2));
    // Transmite o dado.
    UCA0TXBUF = byte_data;
    // Limpa o indicador de transmissor livre.
    IFG2 &= 0xFB;
  }
  __delay_cycles(1000);
  
}
/**
* @fn void SERIAL_write_vector(char byte_data);
* @brief Escrita de vários bytes via SERIAL.
* @param byte_data ponteiro char que indica o vetor de bytes que se deseja enviar
* @param port - argumento char referente à porta para onde irá enviar
* @return Esta função não possui retorno
*/
void SERIAL_write_vector(char port, unsigned char *byte_vector, unsigned short len){
  unsigned short i;

  if(port){                   // Transmissão da sonda 
    for(i = 0; i < len; i++){
      // Espera liberar o transmissor:
      while(!(UC1IFG & 2));
      // Transmite byte a byte.
      UCA1TXBUF = byte_vector[i];
      // Limpa o indicador de transmissor livre.
      UC1IFG &= 0xFB;
    }
  }  
  else{
    for(i = 0; i < len; i++){  
      // Espera liberar o transmissor:
      while(!(IFG2 & 2));
      // Transmite byte a byte.
      UCA0TXBUF = byte_vector[i];
      // Limpa o indicador de transmissor livre.
      IFG2 &= 0xFB;
    }
  }
  __delay_cycles(1000);             //(?) Verificar se precisa do delay e onde ele deve ficar (no for ou nao).
}

/**
* @fn void SERIAL_clear_buffer(void);
* @brief Esta função limpa o buffer.
* @param Esta função não recebe parâmetro.
* @return Esta função não possui retorno
*/
void SERIAL_clear_buffer(void){
  short i;
  for(i=0;i<SIZE;i++)
    rx_buffer[i] = 0;
}

// Deve-se colocar o vetor de interrupções de determinado dispositvo.
#pragma vector=USCIAB0RX_VECTOR
// Interrupção será gerada quando recebe um dado na UART 0.
__interrupt void USCIA0RX_ISR (void){
  // Armazena os dados lidos no buffer:
  rx_buffer_pointer[rx_buffer_counter++] = UCA0RXBUF;
  IFG2 &= (~1);                 // Limpa flag de dado recebido.
  if( rx_buffer_counter == SIZE){
        flag1_new_msg = 1;
        rx_buffer_counter = 0;              // Zerando contador.
        __bic_SR_register_on_exit(LPM0_bits + GIE); // Sai do estado de baixo consumo.
  }
}

#pragma vector=USCIAB1RX_VECTOR
// Interrupção gerada quando recebe um dado na UART 1.
__interrupt void USCIA1RX_ISR(void){
  // Armazena dados lidos no buffer.
  rx_buffer_pointer[rx_buffer_counter++] = UCA1RXBUF;   
  UC1IFG &= 0xFE;                 // (~1) - Limpa flag de dado recebido.
  //verificacao de valores incomuns (bytes extras)
  if(rx_buffer_counter == 1){
    uwsize = ((rx_buffer_pointer[0]&0xc0) == 0xc0)?UWSIZE+1:UWSIZE;
    uwsize = (rx_buffer_pointer[0]&0x20)?uwsize+1:uwsize;
  }
  else if(rx_buffer_counter >= uwsize){
    flag2_new_msg = 1;
    rx_buffer_counter = 0;                // Zerando contador.
    __bic_SR_register_on_exit(LPM0_bits + GIE); // Sai do estado de baixo consumo.
   }
  
}

