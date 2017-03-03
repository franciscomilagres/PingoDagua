#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define COM 0
#define BAUD 9600
#define MODE "8N1"
#define NUMCOMS 38
#define SIZE 64

/*ICMP: Indica qua o tipo de socket utilizado pelo programa que usa
* esta biblioteca. 1 = ICMP (RAW socket), 0 = UDP (UDP socket).
*/
#define ICMP 1

//estrutura que vai conter a mensagem a ser enviada
struct Packet {
	struct icmphdr hdr;
	char payload[SIZE - sizeof(struct icmphdr)];
};


/*--------------------------------------------------------------------*/
/*--- checksum - padrao de complemento de 1                   ---*/
/*--------------------------------------------------------------------*/
unsigned short checksum(void *, int);


/*--------------------------------------------------------------------*/
/*--- display - present echo info                                  ---*/
/*--------------------------------------------------------------------*/
void display(void *, int);


/*list:
*tarefa: Lista as portas(tty e ttyS) que o programa consegue acessar.
*/
int list();


/*initCOM:
*tarefa: Pega a porta COM escolhida pelo usuario e abre
*retorna -1 em caso de erro.
*/
int initCOM();

