#include "pp_funcs.h"
#include "rs232.h"


/*---------------------------------------------------------------*/
/*--- checksum - padrao de complemento de 1                   ---*/
/*---------------------------------------------------------------*/
unsigned short checksum(void *b, int len)
{	unsigned short *buf = (unsigned short *)b;
	unsigned int sum=0;
	unsigned short result;

	for ( sum = 0; len > 1; len -= 2 )
		sum += *buf++;
	if ( len == 1 )
		sum += *(unsigned char*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}

#if ICMP == 1
/*----------------------------------------------------------------*/
/*--- display - present echo info (with RAW socket messages)   ---*/
/*----------------------------------------------------------------*/
void display(void *buf, int bytes)
{	int i;
	struct iphdr *ip = buf;
	struct icmphdr *icmp = buf+ip->ihl*4;
	struct in_addr aux;								//franciscagem

	//printf("----------------");
	for ( i = 0; i < bytes; i++ )
	{
		if ( !(i & 15) ) printf("\n%x:  ", i);
		printf("%x ", ((unsigned char*)buf)[i]);
	}
	printf("\n");

	aux.s_addr = ip->saddr;				//mais franciscagem, pra botar no printf

	printf("IPv%d: hdr-size=%d pkt-size=%d protocol=%d TTL=%d src=%s ",
		ip->version, ip->ihl*4, ntohs(ip->tot_len), ip->protocol,
		ip->ttl, inet_ntoa(aux));
	
	aux.s_addr = ip->daddr;				//mais franciscagem, pra botar no printf

	printf("dst=%s\n", inet_ntoa(aux));
	printf("ICMP: type[%d/%d] checksum[%d] id[%d] seq[%d]\n",
		icmp->type, icmp->code, ntohs(icmp->checksum),
		ntohs(icmp->un.echo.id), ntohs(icmp->un.echo.sequence));
}

#else
/*----------------------------------------------------------------*/
/*--- display - present echo info (with UDP socket messages)   ---*/
/*----------------------------------------------------------------*/
void display(void *buf, int bytes)
{	int i;
	
		struct Packet *pckt = (struct Packet*)buf;						

			printf("------------------------\n");
			for ( i = 0; i < bytes; i++ )
			{
				if ( !(i & 15) ) printf("\n%x:  ", i);
				printf("%x ", ((unsigned char*)buf)[i]);
			}
			printf("\n");
			printf("ICMP: type[%d/%d] checksum[%d] id[%d] seq[%d]\n",
				pckt->hdr.type, pckt->hdr.code, ntohs(pckt->hdr.checksum),
				pckt->hdr.un.echo.id, pckt->hdr.un.echo.sequence);
}

#endif

/*list:
*tarefa: Lista as portas(tty e ttyS) que o programa consegue acessar.
*/
int list(){
	extern char comports[38][16];
	int i, aux, serinfo;										//aux indica se a porta da pra abrir
	int found=0;
	
	printf("Index:\tPort\n");

	for(i = 0; i < NUMCOMS; i++){
		aux = open(comports[i], O_RDWR | O_NOCTTY | O_NDELAY);
		if(aux != -1){																					//se o open pegou
			if(ioctl(aux, TIOCMGET, &serinfo) != -1){					//se eu consigo fuçar na porta
				printf("%d:\t%s\n",i,comports[i]);
				found = 1;
			}
			close(aux);
		}
	}
	return found;
}


/*initCOM:
*tarefa: Pega a porta COM escolhida pelo usuario e abre
*retorna -1 em caso de erro.
*/
int initCOM(){
	int port, erro;
	
	printf("Choose Port(index): ");
	scanf("%d",&port);

	while(port > NUMCOMS){
		printf("ERROR: there is no port with index %d.\n",port);
		printf("Try again: ");
		scanf("%d",&port);
	}
	
	erro = RS232_OpenComport(port, BAUD, MODE);    //abra-te cesamo!
	if(erro == 0){
		
		printf(" Done.\nBaud rate: %d; Data bits: 8; Parity: none; Stop bits: 1.\nOk? ",BAUD);
		printf("Yes... You have no option.\n");
		RS232_flushRXTX(port);		//limpando buffer da porta pra pingar
		return port;
	}
	else {
		printf("\nChoose an index number from the list.\n");
		return erro;
	}
}
