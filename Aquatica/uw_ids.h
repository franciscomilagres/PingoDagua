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
* ambiente subaquatico. Contem especificamente o compo- *
* nente responsavel pela compressao do campo ID do cabe-*
* calho ICMP, realizando um mapeamento.                 *
*                Deus seja louvado                      *
* Autor: Francisco Milagres                             *
* Data: 17/03/2016                                      *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 

#include <sys/cdefs.h>
#include <sys/types.h>


#ifndef UIDS
#define UIDS 1

#define MSB 0xFF00
#define LSB 0x00FF
typedef struct idcell* Pointer;



/*struct idcell
*Mapeia um uwID para um realID.
*/
typedef struct idcell{
	u_int16_t realID;
	u_int8_t uwID;

	Pointer next;						//celula seguinte na lista encadeada

} IDcell;

typedef struct ListIDs{
	int numIDs;					//numero de ids
	
	Pointer first;					//primeiro da lista
	Pointer last;						//ultimo da lista

} idsmap;


/*UWIDs_init (pro todo):
* Aloca e inicia uma lista encadeada em l
* Retorna 1 em caso de sucesso e -1 em caso de fracasso.

int UWIDs_init(idsmap **l); */

/*UWIDs_init:
* Inicia uma lista encadeada ja alocada, em l
* retorna nada nao e na proxima versao nao vai ter isso (todo)
*/
void UWIDs_init(idsmap *l);


/* addID:
*Tarefa: adiciona o id e o uwid em map
*Retorna 1 em caso de sucesso ou -1 em caso de erro
*/
int addID(idsmap *map, u_int16_t realID, u_int8_t uwID);


/*removeID:
*Tarefa: remove a célula que contem realID
*/
void removeID(idsmap *map, u_int16_t realID);


/*get_uwID:
*Tarefa: retorna o uwID correspondente ao realID recebido nos parametros.
* Se nao existir, cria um novo uwID para este realID.
* Se nao houver mais IDs disponiveis, retorna -1
*/
int get_uwID(idsmap *map, u_int16_t realID);


/*get_realID:
*Tarefa: retorna o realID correspondente ao uwID recebido nos parametros.
* Se nao existir, retorna -1. (tecnicamente isto deve ser impossivel)
*/
int get_realID(idsmap *map, u_int8_t uwID);


/*exist_ID:
*Tarefa: Procura pelo uwID em map. Se existir, retorna 1. Se nao, retorna 0.
*/
int exist_uwID(idsmap *map, u_int8_t uwID);


#endif

/*TODO:
*Passar declaracao de map pra ca, sendo restrita a esta biblioteca
* e passar map pra alocacao dinamica
*/
