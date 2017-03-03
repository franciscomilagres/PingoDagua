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
* ambiente subaquatico. Contem funcoes para dispositivo *
* intermediario da rede para UW. Nesta primeira versao o*
* foco foi a aplicacao do comando Ping.                 *
*                Deus seja louvado                      *
* Autor: Francisco Milagres                             *
* Data: 17/03/2016                                      *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 

#include <stdlib.h>
#include <strings.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include "rs232.h"
#include "pp_funcs.h"


/*ICMP_isUW:
*tarefa: Verifica se a msg ICMP eh pra um dispositivo underwater
* com base em campos do payload.
*parametro: size - tamanho da msg.
*Retorna: 1 = sim; 0 = nao.
*PS.: O padrao do usuario no echo so eh aplicado corretamente a
* partir do byte 16, mas depende da quantidade de hexas no padrao.
*/
int ICMP_isUW(struct Packet *msg, int size){
	int answer;

	if(size == SIZE){	//por enquanto aceita apenas o tamanho padrao
		if(msg->hdr.type == ICMP_ECHO && msg->hdr.code == 0 &&
			 ((unsigned char)msg->payload[16]) == 0x0f &&
			 ((unsigned char)msg->payload[17]) == 0xc0) {				
			answer = 1;
		}
		else{
			answer = 0;
		}
	}
	else{
		answer = 0;
	}

	return answer;
}


/*ICMP_forwarder:
*tarefa: Recebe um ping via ICMP e o encaminha para o RS232,
* retirando os cabecalhos IP.
* param: sockfd - descritor do socket ICMP
*/
void ICMP_forwarder(int sockfd, int cportd){
	int bytes, combytes, len, aux;	//todo programa precisa de um aux
	char buffer[256];			//precisa de mais que isso?
	struct sockaddr_in addr;
	struct iphdr *ip;			//pra arrancar fora a parte IP
	struct icmphdr *icmp;

	printf("-DONE...\nListening...\n");

	while(1){
		bytes = 0;
		len = sizeof(struct sockaddr_in);
		bzero(buffer, 256);
		
		//esperando e recebendo msg icmp
		bytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr,&len);

		if(bytes > 0){
			printf("===%d: ICMP - Received ===\n",sockfd);
			display(buffer, bytes);
			
			ip = (struct iphdr *)buffer;					//pegando parte IP da mensagem
			icmp = (struct icmphdr *)(buffer+ip->ihl*4);	//pegando parte ICMP da mensagem (ah va!)
			bytes -= sizeof(struct iphdr);				//nova quantidade de bytes a ser enviada via RS232
			
			if(ICMP_isUW((struct Packet *)(buffer+ip->ihl*4), bytes)){
				//conversoes de Byte order pro dispositivo UW
				//icmp->checksum = ntohs(icmp->checksum);
				//icmp->un.echo.id = ntohs(icmp->un.echo.id);
				//icmp->un.echo.sequence = ntohs(icmp->un.echo.sequence);

				printf("\nForwarding to RS232.\n");

				if(RS232_SendBuf(cportd,(unsigned char *)icmp,bytes)<0){  //encaminhando pro RS232
					printf("I can not forward from ICMP to rs232.\n");
					perror("RS232_SendBuf ");
					exit(2);										//deu ruim
				}
				printf("-DONE...\n");				//deu bom
				
				combytes = 0;
				bzero(buffer, 256);
				printf("Waiting for COM answer...\n");
				do{
					aux = RS232_PollComport(cportd, &buffer[combytes], SIZE);
					if(aux > 0)				//PollComport nao eh blocante
						combytes += aux;		//vai recebendo e contando
					usleep(100000);		//100 ms
				}while(combytes < bytes && aux > -1);
				
				if(aux < 0){
					printf("Error while reading COM port\n");
					perror("RS232_PollComport ");
					exit(2);
				}

				printf("===%d: COM - Received ===\n",cportd);
				//len = sizeof(struct sockaddr_in);

				//conversoes de Byte order pro dispositivo UW
				//icmp->checksum = htons(icmp->checksum);
				//icmp->un.echo.id = htons(icmp->un.echo.id);
				//icmp->un.echo.sequence = htons(icmp->un.echo.sequence);
				
				printf("Forwarding answer to host...\n");

				if(sendto(sockfd, buffer, SIZE, 0, (struct sockaddr*)&addr,len) < 0){
					printf("Send failed.\n");
					perror("sendto ");
					exit(2);
				}
				printf("-DONE...\n----------------------\n\n");

			}//if isUW
			else{											//mensagens ICMP normais, preparar resposta
				printf("Not for RS232...\n\n");
				/*
				icmp->type = 0;
				icmp->checksum = 0;
				icmp->checksum = checksum(icmp, bytes);

				if(sendto(sockfd, icmp, bytes, 0, (struct sockaddr *)&addr, len) < 0){
					perror("sendto ");
					exit(2);
				}
				printf("Answered from localhost...\n");
				*/
			}
		}
		else{
			perror("Listener - recvfrom");
			exit(2);
		}
	}

}

int main(int argc, char **argv){
	struct protoent *proto = NULL;
	int sockfd, comport;
	
	printf("----Welcome to ICMP forwarder!----\n");		//so dizendo que que o programa faz
	printf("I receive ICMP ping messages and forward to COMs (rs232)\n");

	printf("\n----COM Config----\n");
	
	if(!list()){				//se a lista nao tem nem uma porta acessivel
		printf("ERROR: No available COM ports found...\n");
		printf("Please verify your COM devices.\n");
		return 1;			//como vou executar sem uma porta RS232?
	}
	comport = initCOM();			//pegando a porta escolhida pelo usuario e abrindo.
	printf("-DONE...\n");

	printf("\n----ICMP Config ----\n");
	printf("Configuring ICMP socket...\n");

	proto = getprotobyname("ICMP");
	sockfd = socket(PF_INET, SOCK_RAW, proto->p_proto);
	if(sockfd >= 0){
		printf("-DONE...\nStarting forwarder...\n");
		ICMP_forwarder(sockfd,comport);
	}
	else
		perror("socket");
	
	printf("exiting...\n");
	return 1;
}

/*TODO:
*Aceitar diversos tamanhos de mensagem, nao so os 64 bytes
*olhar o procedimento do socket UDP, pra pegar um socket "generico" que
* sirva tanto pro ICMP quanto pro ICMPv6. Ver se já não aceita da maneira
* implementada atualmente (ps. Parece que nao).
*/
