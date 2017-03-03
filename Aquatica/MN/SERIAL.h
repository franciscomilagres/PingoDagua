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

/// @def CLOCK 1000000 clock em Hz do cristal
#define CLOCK 1000000

/**
* @def UCBRF 0
* @def UCBRS 6
* @brief Ajuste fino, consultar tabela página 429 (slau144.pdf)
*/
#define UCBRF 0
#define UCBRS 1

/*
* @def UWSIZE 4 tamanho padrao da mensagem uwicmp
* @def BUFSIZE 6 tamanho do buffer
*/
#define UWSIZE 4
#define BUFSIZE 6
/**
* @var extern SERIAL_rxbuffer ponteiro unsigned char referente a buffer de
* armazenamento de valores lidos da serial
*/
extern unsigned char rx_buffer[BUFSIZE];
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
* @var flag_new_msg variavel unsigned char referente a nova 
* mensagem recebida
*/
extern unsigned short flag_new_msg;
/**
* @fn void init_clock_1MHZ(void)
* @brief garante que o clock seja de 1MZ
* @return Esta função não possui retorno
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
* @return Esta função não possui retorno
*/
void SERIAL_init(int speed, int parity, int typeofparity, int dataorder, int stopbits, int databits);
/**
* @fn SERIAL_disable_interruption_receive(void)
* @brief Esta função desativa a interrupção de recebimento de mensagem
* @return Esta função não possui retorno
*/
void SERIAL_disable_interruption_receive(void);
/**
* @fn SERIAL_enable_interruption_receive(void)
* @brief Esta função habilita a interrupção de recebimento de mensagem
* @return Esta função não possui retorno
*/
void SERIAL_enable_interruption_receive(void);
/**
* @fn unsigned int SERIAL_read_byte(unsigned char *data)
* @brief Leitura de um byte via SERIAL.
* @param unsigend char *data - Ponterio unsigend char para receber o dado lido
* @return Esta função não retorna valor.
*/
void  SERIAL_read_byte(unsigned char *data);
/**@fn unsigned char SERIAL_read_buffer(void)
*@brief Esta função ler o valor do buffer de msg recebida
*@return Esta função retorna o valor lido do buffer.
*/
void SERIAL_read_buffer(unsigned char *msg);
/**@fn unsigned char SERIAL_has_new_msg(void)
*@brief Esta função retorna um indicardor de nova msg recebida
*@return Esta função retorna 1 se possui nova msg 0 se não possui.
*/
unsigned short SERIAL_has_new_msg(void);
/**
* @fn void SERIAL_write_byte(char byte_data);
* @brief Escrita de um byte via SERIAL.
* @param byte_data um argumento char referente ao byte que deseja enviar
* @return Esta função não possui retorno
*/
void SERIAL_write_byte(unsigned char byte_data);

/**
* @fn void SERIAL_write_vector(char byte_data);
* @brief Escrita de vários bytes via SERIAL.
* @param byte_data ponteiro char que indica o vetor de bytes que se deseja enviar
* @param len - quantidade de bytes a serem escritos.
* @return Esta função não possui retorno
*/
void SERIAL_write_vector(unsigned char *byte_vector, unsigned short len);

#endif
