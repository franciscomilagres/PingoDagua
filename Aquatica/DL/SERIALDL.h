/*
*************************************************************************************
*                   UNIVERSIDADE FEDERAL DE MINAS GERAIS                            *
*   AQUANODE: PROTOTIPA��O DE UM N� SENSOR PARA REDES DE SENSORES AQU�TICAS         *
*************************************************************************************
*                         BIBLIOTECA DE COMUNICA��O SERIAL                          *
*************************************************************************************
* DESENVOLVIDO POR:LABORAT�RIO DE ENGENHARIA DE SISTEMA DE COMPUTA��O UFV-FLORESTAL *
* DATA:                                                                             *
* APROVADO POR:                                                                     *
* DATA:                                                                             *
* VERS�O:                                                                           *
*************************************************************************************/

/**
* @file SERIAL.h
* @brief Esta biblioteca tem como objetivo possibilitar uma comunicacao via serial
*/
#ifndef _SERIAL_H_
#define _SERIAL_H_

/**
* @def  UCLK  0x00
* @def  ACLK  0x40
* @def  SMCLK 0x80
* @brief defines para determinacao das fontes de clock
*/
#define UCLK  0x00
#define ACLK  0x40
#define SMCLK 0x80

/// @def CLOCKSOURCE SMCLK escolha de qual fonte de clock usar
#define CLOCKSOURCE SMCLK

/// @def CLOCK 1048576 clock em Hz do cristal
#define CLOCK 1048576


// @def SIZE tamanho padrao da msg normal e da UW
#define SIZE 64
#define UWSIZE 4

/**
* @def UCBRF 0
* @def UCBRS 1
* @brief Ajuste fino, consultar tabela p�gina 429 (slau144.pdf)
*/
#define UCBRF 0
#define UCBRS 1

/**
* @var extern SERIAL_rxbuffer ponteiro unsigned char referente a buffer de
* armazenamento de valores lidos da serial
*/
extern unsigned char rx_buffer[SIZE];
extern unsigned char *rx_buffer_pointer;
/**
* @var flag_new_msg variavel unsigned char referente a nova 
* mensagem recebida
*/
/**
* @var extern SERIAL_rxbuffer_size variavel short referente a tamanho do buffer de
* armazenamento de valores lidos da serial
*/
extern unsigned short rx_buffer_counter;
/**
* @var flag1_new_msg variavel unsigned char referente a nova 
* mensagem recebida na USCI_A0.
* @var flag2_new_msg vari�vel unsigned char referente a nova
* mensagem recebida na USCI_A1.
*/
extern unsigned short flag1_new_msg;
extern unsigned short flag2_new_msg;
/**
* @fn void init_clock_1MHZ(void)
* @brief garante que o clock seja de 1MZ
* @return Esta fun��o n�o possui retorno
*/
void SERIAL_init_clock_1MHZ(void);
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
* @return Esta fun��o n�o possui retorno
*/
void SERIAL_init(int speed, int parity, int typeofparity, int dataorder, int stopbits, int databits);
/**
* @fn SERIAL_disable_interruption_receive(void)
* @brief Esta fun��o desativa a interrup��o de recebimento de mensagem
* @param port - argumento inteiro referente � porta que se deseja desabilitar
* (0 - UCA0 [PC] / 1 - UCA1 [Sonda] )
* @return Esta fun��o n�o possui retorno
*/
void SERIAL_disable_interruption_receive(char port);
/**
* @fn SERIAL_enable_interruption_receive(void)
* @brief Esta fun��o habilita a interrup��o de recebimento de mensagem
* @param port - argumento inteiro referente � porta que se deseja habilitar
* (0 - UCA0 [PC] / 1 - UCA1 [Sonda] )
* @return Esta fun��o n�o possui retorno
*/
void SERIAL_enable_interruption_receive(char port);
/**
* @fn unsigned int SERIAL_read_byte(unsigned char *data)
* @brief Leitura de um byte via SERIAL.
* @param unsigend char *data - Ponterio unsigend char para receber o dado lido
* @return Esta fun��o n�o retorna valor.
*/
void  SERIAL_read_byte(char port, unsigned char *data);
/**@fn unsigned char SERIAL_has_new_msg(void)
*@brief Esta fun��o retorna um indicardor de nova msg recebida
*@return Esta fun��o retorna 1 se possui nova msg 0 se n�o possui.
*/
unsigned short SERIAL_has_new_msg(char port);
/**
* @fn void SERIAL_write_byte(char byte_data);
* @brief Escrita de um byte via SERIAL.
* @param byte_data um argumento char referente ao byte que deseja enviar
* @return Esta fun��o n�o possui retorno
*/
void SERIAL_write_byte(char port, unsigned char byte_data);

/**
* @fn void SERIAL_write_vector(char byte_data);
* @brief Escrita de v�rios bytes via SERIAL.
* @param byte_data ponteiro char que indica o vetor de bytes que se deseja enviar
* @return Esta fun��o n�o possui retorno
*/
void SERIAL_write_vector(char port, unsigned char *byte_vector, unsigned short len);

/**
* @fn void SERIAL_clear_buffer(void);
* @brief Esta fun��o limpa o buffer.
* @param Esta fun��o n�o recebe par�metro.
* @return Esta fun��o n�o possui retorno
*/
void SERIAL_clear_buffer(void);
#endif
