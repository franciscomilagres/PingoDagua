teste:	teste.c	pc_uwicmp pc_uw_ids
			gcc -o teste teste.c uwicmp.o uw_ids.o

pc_uwicmp:	uwicmp.c	uwicmp.h
			gcc -c uwicmp.c

pc_uw_ids:	uw_ids.c	uw_ids.h
			gcc -c uw_ids.c

msp_uw_ids:	uw_ids.c	uw_ids.h
			msp430-gcc -c uw_ids.c

msp_uwicmp:	uwicmp.c	uwicmp.h
			msp430-gcc -c uwicmp.c

msp_main:	msp_uwicmp msp_uw_ids
			msp430-gcc -mmcu='msp430f247' -Wall -o main.o main.c uwicmp.o uw_ids.o
			msp430-objcopy -O ihex main.o main.hex

debug:	msp_main
			mspdebug uif

clean:
		rm *.o
		rm teste

