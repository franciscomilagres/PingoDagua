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


/*pongerRS232
*Tarefa: Recebe uma mensagem de ping e a manda em um pong.
*portd: Descritor da porta Serial que esta sendo utilizada.
*/
void pongerRS232(int portd){
	int bytes, aux;
	char buffer[SIZE];
	struct Packet *pckt;
	
	while(1){
		/*por alguma razao a PollComport nao eh blocante, mas quando
		* soco ela num loop so dela, ela fica; Enfim, assim funciona. */
		bytes = 0;			//preparando pra proxima leva
		aux = 0;
		bzero(buffer,sizeof(buffer));
		//recebendo mensagem completa
		do{
			aux = RS232_PollComport(portd, &buffer[bytes], SIZE);   //recebendo a partir da primeira posicao vazia do buffer.
			if(aux > 0)				//se recebeu algo e nao deu erro
				bytes += aux;		//incrementando qtd de bytes recebidos
			usleep(100000);
		}while(bytes < 64 && aux > -1);

		if(aux < 0){
			perror("ponger: ");
			exit(1);
		}

		//buffer[bytes] = '\0';			//transformando em string
		//printf("received:\n%s\n",buffer);
		pckt = (struct Packet *)buffer;
		if(pckt->hdr.type == ICMP_ECHO){				//se for um ping ne
			//printf("Received a ping from %d, seq %d!\n",pckt->hdr.un.echo.id, pckt->hdr.un.echo.sequence);
			display(buffer, sizeof(buffer));
			pckt->hdr.type = ICMP_ECHOREPLY;
			bytes = RS232_SendBuf(portd, buffer, SIZE);
			printf("Sent echo reply %d...\n",bytes);
		}
		else{
			printf("The type is %d\n",pckt->hdr.type);
		}

	}

}


//Eh a main, dispensa comentarios;
int main(){
	int port, erro;

	
	if(!list()){				//se a lista nao tem nem uma porta acessivel
		printf("No available COM ports found...\n");
		return 1;
	}

	port = initCOM();			//capturando e abrindo porta do usuario
	if(port >= 0)
		pongerRS232(port);	//se tudo der certo abre o ponger.

	return 0;
}
