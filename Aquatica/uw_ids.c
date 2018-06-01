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

#include <stdlib.h>
#include "uw_ids.h"

/*UWIDs_init:
* Aloca e inicia uma lista encadeada em l
* Retorna 1 em caso de sucesso e -1 em caso de fracasso.
*/
int UWIDs_init(idsmap **l){
	*l = (idsmap *)malloc(sizeof(idsmap));
	if(*l == NULL)
		return -1;
	(*l)->numIDs = 0;
	(*l)->first = NULL;
	(*l)->last = NULL;
	return 1;
}

/*UWIDs_init:
* Inicia uma lista encadeada ja alocada, em l
* retorna nada nao e na proxima versao nao vai ter isso

void UWIDs_init(idsmap *l){
	l->numIDs = 0;
	l->first = NULL;
	l->last = NULL;
}
*/

/* addID:
*Tarefa: adiciona o id e o uwid em map
*Retorna 1 em caso de sucesso ou -1 em caso de erro
*/
int addID(idsmap *map, u_int16_t realID, u_int8_t uwID){
	idcPointer aux;
	
	if(map->numIDs == 0){			//se nao tiver nenhuma celula
		map->first = (idcPointer)malloc(sizeof(IDcell));
		if(map->first == NULL)
			return -1;
		map->last = map->first;				//a ultima celula eh a primeira
	}
	else{
		map->last->next = (idcPointer)malloc(sizeof(IDcell));  //se tiver, alocando no next do proximo
		if(map->last->next == NULL)
			return -1;
		map->last = map->last->next;
	}	

	map->last->next = NULL;				//so pra garantir
	aux = map->last;
	aux->realID = realID;
	aux->uwID = uwID;
	map->numIDs++;
	return 1;
}


/*removeID:
*Tarefa: remove a célula que contem o realID
*/
void removeID(idsmap *map, u_int16_t realID){
	idcPointer aux1, aux2;

	aux1 = map->first;
	if(aux1->realID == realID){			//ai nao tem ninguem antes dele
		map->first = aux1->next;
		free(aux1);
		map->numIDs--;
	}
	else{
		while(aux1 != NULL && aux1->realID != realID){
			aux2 = aux1;
			aux1 = aux1->next;
		}
		
		if(aux1 != NULL){
			if(aux1 == map->last)			//se for a ultima da lista
				map->last = aux2;				//a ultima passa a ser a anterior
			
			aux2->next = aux1->next;				//tirando a celula de aux1 da lista
			free(aux1);								//liberando memoria
			map->numIDs--;						//Diminuindo numero de IDs
		}
	}
}


/*exist_uwID:
*Tarefa: Procura pelo uwID em map. Se existir, retorna 1. Se nao, retorna 0.
*/
int exist_uwID(idsmap *map, u_int8_t uwID){
	idcPointer aux;

	aux = map->first;
	while(aux != NULL && aux->uwID != uwID){
		aux = aux->next;
	}
	if(aux == NULL)
		return 0;
	return 1;

}

/*get_uwID:
*Tarefa: retorna o uwID correspondente ao realID recebido nos parametros.
* Se nao existir, cria um novo uwID para este realID.
* Se nao houver mais IDs disponiveis, retorna -1
*/
int get_uwID(idsmap *map, u_int16_t realID){
	idcPointer aux;
	u_int8_t uwID;

	aux = map->first;
	while(aux != NULL && aux->realID != realID){ 
		aux = aux->next;					//procura se esse realID ja esta registrado
	}
	
	if(aux != NULL)						//se achou, retorna.
		return aux->uwID;
	else if(map->numIDs < 256){     //senao, se nao ta na capacidade maxima
		if(!exist_uwID(map, realID >> 8)){   //LSB (big endian)
			uwID = realID >> 8;								 //Resulta o LSB como uwID
		}
		else if(!exist_uwID(map, realID & 0x00FF)){	 //MSB
			uwID = realID & 0x00FF;										 //Mesma coisa
		}
		else {					//pow, ja ta usando os dois?
			uwID = realID >> 8;
			do{
				uwID = (uwID + 1) % 256;
			} while (exist_uwID(map, uwID)); //vai parar, pq numIDs < 256. (11 linhas acima)
		}	
		//encontrado um uwID, vamos adiciona-lo.
		if(addID(map, realID, uwID) == -1){			//se o add falhar
			uwID = -1;
		}
	}
	else{						//numIDs = 256, nao cabe mais.
		uwID = -1;
	}
	return uwID;
}


/*get_realID:
*Tarefa: retorna o realID correspondente ao uwID recebido nos parametros.
* Se nao existir, retorna -1. (tecnicamente isto deve ser impossivel)
*/
int get_realID(idsmap *map, u_int8_t uwID){
	idcPointer aux;

	aux = map->first;
	while(aux != NULL && aux->uwID != uwID){
		aux = aux->next;
	}
	
	if(aux != NULL)
		return aux->realID;

	return -1;						//pro gcc ficar sossegado

}
