#include <stdlib.h>
#include <strings.h>
#include <fcntl.h>
#include <errno.h>
//#include <sys/socket.h>
#include <resolv.h>
//#include <netdb.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <netinet/ip_icmp.h>
#include "rs232.h"
#include "pp_funcs.h"

int pid;

/*pingerRS232:
*tarefa: Encarregada de enviar o ping via RS232 a cada segundo
*param: portd - porta que sera usada.
*/
void pingerRS232(int portd){
	struct Packet pckt;
	int sent, seq=1, i;
	
	bzero(&pckt, sizeof(pckt));		//preparando mensagem ICMP
	pckt.hdr.type = ICMP_ECHO;
	pckt.hdr.un.echo.id = pid;
	for(i = 0; i < SIZE - sizeof(struct icmphdr); i++)
		pckt.payload[i] = i + '0';	//enchendo payload de caracteres legiveis
	

	while(1){				//começando a pingada
		printf("\nPing: Msg %d\n",seq);
		pckt.hdr.un.echo.sequence = seq++;
		pckt.hdr.checksum = checksum(&pckt, sizeof(struct Packet));

		sent = RS232_SendBuf(portd, (unsigned char *)&pckt, sizeof(pckt));
		if(sent == -1){
			perror("pinger: ");
			exit(0);
		}
		printf("%d: Sent %d bytes... that is\n",pid,sent);
		display((void *)&pckt, sizeof(struct Packet));
		sleep(1);
		//printf("Posso enviar outro? ");
		//scanf(" %d", &sent);
		
	}

}

/*listenerRS232:
*tarefa: Espera os pongs que o pinger mandar
*/
void listenerRS232(int portd){
	
	int resplen, aux;			//pq todo programa tem que ter um aux...
	unsigned char buffer[SIZE];		//alguem deu a ideia de colocar 1024. Nao sei pq...

	while(1){		//E aqui eh o loop das pongadas
		bzero(&buffer, sizeof(buffer));
		aux = 0;
		resplen = 0;		//preparando pra leva de resposta
		do{
			aux = RS232_PollComport(portd, &buffer[resplen], SIZE);
			if(aux > 0)
				resplen += aux;			//incrementando qtd de bytes recebidos
			usleep(100000);
		}while(resplen < 64 && aux > -1);
		
		if(aux > -1){
			printf("\n%d: Listener Received...", pid);
			display(buffer, sizeof(buffer));
		}
		else{
			perror("Listener: ");
			return;
		}
	}

}



//eh a Main, dispensa comentarios.
int main(){
	int port;
	
	if(!list()){				//se a lista nao tem nem uma porta acessivel
		printf("No available COM ports found...\n");
		return 1;
	}
	
	port = initCOM();			//pegando a porta escolhida pelo usuario e abrindo.
	
	printf("\nOpenning port %d...\n",port);
	
	if(port != -1){
		
		pid = fork();
		if(pid == 0){
			listenerRS232(port);
		}
		else{
			pingerRS232(port);
		}	
	}

	printf("%d closing...\n",pid);
	RS232_CloseComport(port);

	

	return 0;
}
