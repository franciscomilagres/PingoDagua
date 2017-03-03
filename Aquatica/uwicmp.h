/* * * * * * * * * * * * * * * * * * * * * * * * * * * **
* UFMG: UNIVERSIDADE FEDERAL DE MINAS GERAIS            *
* ICEX: INSTITUTO DE CIENCIAS EXATAS                    *
* DCC: DEPARTAMENTO DE CIENCIA DA COMPUTACAO            *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*               ICMP SUBAQUATICO V 0.1                  *
*               UNDERWATER ICMP  V 0.1                  *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
* Este arquivo faz parte de uma proposta de compressao  *
* de cabecalhos do protocolo ICMP, para aplicacao em    *
* ambiente subaquatico. Contem estruturas e principais  *
* funcionalidades para os dados. Nesta primeira versao o*
* foco foi a aplicacao do comando Ping.                 *
*                Deus seja louvado                      *
* Autor: Francisco Milagres                             *
* Data: 17/03/2016                                      *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 

#include "ip_icmp.h"

//Macros pra ajudar a LER os bits
#define B1_TYPE 0xc0          //Campo Type  (use no byte 1)
#define B1_CODE 0x20          //Campo Code  (use no byte 1)
#define B1_CHECKSUM 0x1f      //5 bits mais significativos do checksum.
#define B2_CHECKSUM 0xe0      //3 bits menos significativos do checksum (use no byte 2)
#define B2_ECHO_ID 0x1f           //5 mais significativos do ID (use no byte 2)

#define B3_ECHO_ID 0xe0       //3 menos significativos do ID
#define B3_ECHO_SEQ 0x1f

#define SIZE 64

//Byte do Payload que contem o ID do destinatario
#define PL_IDBYTE 19

//estrutura que contem um pacote ICMP comum
struct Packet{
  struct icmphdr hdr;
  char payload[SIZE - sizeof(struct icmphdr)];
};

//estrutura de cabecalho underwater icmp
typedef struct Underwater_icmphdr {
  u_int8_t byte1;         //2 bits Type, 1 Code, 5 checksum(HighS).
  u_int8_t byte2;         //3 bits checksum(LowS), 5 ID(HighS).
  u_int8_t byte3;         //3 ID(LowS), 5 Sequence Number.
} Uw_icmphdr;

//estrutura que contem um pacote Underwater
struct Uw_Packet{
  Uw_icmphdr hdr;     //Como o proprio nome ja diz. Cabecalho uicmp
  u_int8_t data[3];         //payload + possiveis futuras concatenacoes
};


/*checksum:
*Tarefa: Computa o checksum de uma mensagem quebrada em palavras de 16 bits
*A mensagem eh recebida em b e seu tamanho, em len (dado em bytes).
*Retorna: o resultado do checksum (complemento de 1).
*/
unsigned int checksum(struct Packet *p, int len);


/*UWICMP_checksum:
*Tarefa: Computa o checksum do pacote p.
*Parametro: datalen (bytes) tamanho de p->data.
*Retorna: o resultado do checksum (complemento de 1).
*/
u_int8_t UWICMP_checksum(struct Uw_Packet p, int datalen);


/*UWICMP_init:
* Inicializa a lista encadeada em map
* Retorna nada nao e na proxima versao nao vai ter isso (todo)
*/
void UWICMP_init();


/*UWICMP_setChecksum:
* Tarefa: seta o cabecalho do checksum da mensagem dest
* Parametros: bytes = bytes ADICIONAIS na mensagem
*/
void UWICMP_setChecksum(struct Uw_Packet *dest, int bytes);


/*UWICMP_reorder:
*Tarefa: Inverte a "byteorder" de data.
*Parametro: data valor cuja ordem se deseja inverter
*Retorna: o resultado da inversão.
*/
u_int16_t UWICMP_reorder(u_int16_t data);


/*UICMP_compress:
* Tarefa: comprime o pacote *source e o retorna em *dest. Ambos os
* parametros precisam estar inicializados.
* Retorna: 1 = sucesso. -1 = problema em map (provavelmente map cheio)
*/
int UWICMP_compress(struct Packet *source, struct Uw_Packet *dest);


/*UICMP_decompress:
* Tarefa: descomprime o pacote *source e o retorna em *dest. Ambos os
* parametros precisam estar inicializados.
* Retorna: 1 = sucesso. -1 = problema em map
*/
int UWICMP_decompress(struct Uw_Packet *source, struct Packet *dest);

/*TODO:
*Remover UWICMP_init quando map for dinamico!
*/
