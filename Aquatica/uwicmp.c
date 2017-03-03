/* * * * * * * * * * * * * * * * * * * * * * * * * * * **
* UFMG: UNIVERSIDADE FEDERAL DE MINAS GERAIS            *
* ICEX: INSTITUTO DE CIENCIAS EXATAS                    *
* DCC: DEPARTAMENTO DE CIENCIA DA COMPUTACAO            *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*               ICMP SUBAQUATICO V 0.1                  *
*               UNDERWATER ICMP  V 0.1                  *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
* Este arquivo constitui uma proposta de compressao dos *
* cabecalhos do protocolo ICMP, para aplicacao em       *
* ambiente subaquatico. Contem estruturas e principais  *
* funcionalidades para os dados. Nesta primeira versao o*
* foco foi a aplicacao do comando Ping.                 *
*                Deus seja louvado                      *
* Autor: Francisco Milagres                             *
* Data: 17/03/2016                                      *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 

#ifndef UWICMP
#define UWICMP 1


//#include <msp430f247.h>          //tamanhos de int e long do msp
#include <string.h>
#include <stdlib.h>
//#include <stdio.h>      so descomenta pra testes no pc
//#include "ip_icmp.h"    ja ta incluido no h
#include "uw_ids.h"
#include "uwicmp.h"


//vai mapear ids reais e ids comprimidos
idsmap map;			//precisa ser inicializada


/*checksum:
*Tarefa: Computa o checksum de uma mensagem quebrada em palavras de 16 bits
*A mensagem eh recebida em b e seu tamanho, em len (dado em bytes).
*Retorna: o resultado do checksum (complemento de 1).
*/
unsigned int checksum(struct Packet *p, int len)
{ unsigned int *buf = (unsigned int *)p;
  unsigned long sum=0;
  unsigned int result;

  for ( sum = 0; len > 1; len -= 2 ){
    sum += *buf++;
  }
  if ( len == 1 )
    sum += *(unsigned char*)buf;
  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  result = ~sum;
  return result;
}


/*UWICMP_checksum:
*Tarefa: Computa o checksum do pacote p.
*Parametro: datalen (bytes) tamanho de p->data.
*Retorna: o resultado do checksum (complemento de 1).
*/
u_int8_t UWICMP_checksum(struct Uw_Packet p, int datalen){
  u_int8_t *buf = (u_int8_t *)&p;
  u_int8_t result;
  unsigned int sum;
  //printf("Checksum - Data: %d em %d\n",p.data[0], p.data);
  
  p.hdr.byte1 &= ~B1_CHECKSUM;   //zerando bits do checksum pq ele 
  p.hdr.byte2 &= ~B2_CHECKSUM;   //n ocupa so 1 byte, pra somar da rruim

  for(sum = 0; datalen>0; datalen--){
    //printf("Buf: %d - %d\n",*buf, buf);
    sum += *buf++;            //somando e apontando pro proximo byte
  }
  result = sum % 256;
  result = ~result;           //faz parte do algoritmo, nao sei
  
  return result;
}

/*UWICMP_init:
* Inicializa a lista encadeada em map
* Retorna nada nao e na proxima versao nao vai ter isso (todo no .h)
*/
void UWICMP_init(){
	UWIDs_init(&map);
}


/*UWICMP_reorder:
*Tarefa: Inverte a "byteorder" de data.
*Parametro: data valor cuja ordem se deseja inverter
*Retorna: o resultado da inversão.
*/
u_int16_t UWICMP_reorder(u_int16_t data){
	u_int16_t result = 0;

	result = (data & 0xFF) << 8;
	result |= (data >> 8) & 0xFF;

	return result;
}



/*UWICMP_setEchoHdr:
* Tarefa: preenche um cabecalho ICMP subaquatico do tipo Echo, recebendo os dados nos parametros.
* menos o Checksum.
* Parametros: type < 4, code < 2, id < 32.
* Usuarios nao devem usar essa funcao.
*/
void UWICMP_setEchoHdr(Uw_icmphdr *echo, u_int8_t type, u_int8_t code, u_int8_t id, u_int8_t seq){
  
  memset(echo, 0, sizeof(Uw_icmphdr));
  
  //aqui infelizmente nao da pra usar aqueles defines

  type = type << 6;     //000000xx --> xx000000
  echo->byte1 = type;
  code = code << 5;    //0000000x --> 00x00000
  echo->byte1 |= code;
  
  echo->byte2 |= (id & 0xf8) >> 3;   //5 HighS do ID, nos 5LowS do byte2

  echo->byte3 |= (id & 0x07) << 5;   //3 LowS do ID nos 3 HighS do byte3
  echo->byte3 |= seq & 0x1f;
  

}


/*UWICMP_setChecksum:
* Tarefa: seta o cabecalho do checksum da mensagem dest
* Parametros: bytes = bytes ADICIONAIS na mensagem
*/
void UWICMP_setChecksum(struct Uw_Packet *dest, int bytes){
  u_int8_t checksum;
  
  //printf("setChecksum - Data: %d ou %d\n",dest->data[0], *(dest->data));

	//limpando checksum
	dest->hdr.byte1 &= ~B1_CHECKSUM;
	dest->hdr.byte2 &= ~B2_CHECKSUM;

  checksum = UWICMP_checksum(*dest, sizeof(Uw_icmphdr)+bytes+1);      //pode ter bytes adicionais em data, que ja tem um byte
  
  dest->hdr.byte1 |= (checksum & 0xf8) >> 3;    //5 HighS do Checksum, nos 5 LowS do byte1
  dest->hdr.byte2 |= (checksum & 0x07) << 5;    //3 LowS do Checksum, nos 3 HighS do byte2
}


/*UWICMP_compress:
* Tarefa: comprime o pacote *source e o retorna em *dest. Ambos os
* parametros precisam estar inicializados.
* Retorna: 1 = sucesso. -1 = problema em map (provavelmente map cheio)
*/
int UWICMP_compress(struct Packet *source, struct Uw_Packet *dest){
  u_int8_t type, code, id, seq;
  short bytes = 0, i=1;       //bytes adicionais nos dados (data)

  switch(source->hdr.type){         //comprimindo type
    case ICMP_ECHO:
      type = 1;
      break;
    case ICMP_ECHOREPLY:
      type = 0;
      break;
    case ICMP_DEST_UNREACH:
      type = 2;
      break;
    default:
      type = 3;
      bytes++;
  }

  switch(source->hdr.code){         //comprimindo code
    case 0:
      code = 0;
      break;
    default:
      code = 1;
      bytes++;
  }
  

  if(get_uwID(&map, source->hdr.un.echo.id) == -1)
    return -1;          //nem a pau que vou abrir chave e identar a galera
  id = get_uwID(&map, source->hdr.un.echo.id);
  
  seq = (source->hdr.un.echo.sequence & 0x1F00) >> 8; //5 bits LSB (big endian) do original, acho que funciona.
  
  UWICMP_setEchoHdr(&(dest->hdr),type, code, id, seq);    //setando cabecalho sem checksum
  //dest->data = (u_int8_t *)malloc((bytes+1)*sizeof(u_int8_t));
  
  dest->data[0] = source->payload[PL_IDBYTE];  //pegando so o byte do payload que indica o ID
  if(bytes){            //tem campo com valor fora do comum, adicionando
    if(type == 3){      //type incomum
      dest->data[i] = source->hdr.type;   //colocando tipo incomum la.
      i++;        //nova posicao de insercao do proximo incomum, se houver
    }
    if(code == 1){      //code incomum
      dest->data[i] = source->hdr.code;
      //i++;        descomente esta linha se for colocar mais campos
    }
  }
  UWICMP_setChecksum(dest, bytes);   //finalmente computando checksum
  return 1;
  
}


/*UWICMP_decompress:
* Tarefa: descomprime o pacote *source e o retorna em *dest. Ambos os
* parametros precisam estar inicializados.
* Retorna: 1 = sucesso. -1 = problema em map
*/
int UWICMP_decompress(struct Uw_Packet *source, struct Packet *dest){
  int aux;

  aux = (source->hdr.byte1 & B1_TYPE) >> 6;    //xx000000 --> 000000xx
  switch(aux){
    case 1:
      dest->hdr.type = ICMP_ECHO;
      break;
    case 0:
      dest->hdr.type = ICMP_ECHOREPLY;
      break;
    case 2:
      dest->hdr.type = ICMP_DEST_UNREACH;
      break;
    default:
      dest->hdr.type = source->data[1];     //type incomum sempre concatenado aqui
  }

  if((source->hdr.byte1 & B1_CODE) == 0){
    dest->hdr.code = 0;
  }
  else{
    if(aux == 3)                  //type incomum => code no byte 2
      dest->hdr.code = source->data[2];
    else                          //type comum => code no byte 1
      dest->hdr.code = source->data[1];
  }

  aux = (source->hdr.byte2 & B2_ECHO_ID) << 3;  //000xxxxx --> xxxxx000
  aux |= (source->hdr.byte3 & B3_ECHO_ID) >> 5; //00000xxx --> xxx00000
  aux = get_realID(&map, aux);
  if(aux == -1)
    return -1;
  dest->hdr.un.echo.id = aux;
	
  dest->hdr.un.echo.sequence &= 0xE0FF;     //zerando os bits de seq
  dest->hdr.un.echo.sequence |= ((source->hdr.byte3 & B3_ECHO_SEQ) << 8); 	//00000000 000xxxxx --> 000xxxxx 00000000
  
  dest->payload[PL_IDBYTE] = source->data[0];     //voltando payload

  dest->hdr.checksum = 0;
  dest->hdr.checksum = checksum(dest, sizeof(struct Packet));
  
  return 1;
}

#endif

/*TODO:
*Suporte a Big endian e little endian;
*Suporte a tamanhos diferentes (tb no ICMPforwarder)
*Remover UWICMP_init quando map for dinamico!
*/
