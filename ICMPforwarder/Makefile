all:	pingrs232	pongrs232	udpforwarder	icmpforwarder

pingrs232:	pingrs232.o	rs232.o	pp_funcs.o
			gcc pingrs232.o rs232.o pp_funcs.o -Wall -Wextra -o2 -o pingerRS232

pongrs232:	pongrs232.o	rs232.o	pp_funcs.o
			gcc pongrs232.o rs232.o pp_funcs.o -Wall -Wextra -o2 -o pongerRS232

pingrs232.o:	pingrs232.c
			gcc -c pingrs232.c

pongrs232.o:	pongrs232.c
			gcc -c pongrs232.c

icmpforwarder:	ICMPforwarder.c pp_funcs.o	rs232.o
			gcc -o icmpforwarder ICMPforwarder.c pp_funcs.o rs232.o

rs232.o:	rs232.c
			gcc -c rs232.c

pp_funcs.o:	pp_funcs.c
			gcc -c pp_funcs.c

clean:
			rm *.o
			rm pingerRS232 pongerRS232 icmpforwarder
