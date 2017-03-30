pc_uwicmp:	uwicmp.c	uwicmp.h
			gcc -c uwicmp.c

pc_uw_ids:	uw_ids.c	uw_ids.h
			gcc -c uw_ids.c

msp_uw_ids:	uw_ids.c	uw_ids.h
			msp430-gcc -c uw_ids.c

msp_uwicmp:	uwicmp.c	uwicmp.h
			msp430-gcc -c uwicmp.c

serialdl:
			msp430-gcc -mmcu='msp430f247' -c DL/SERIALDL.c -o SERIALDL.o

serialmn:
			msp430-gcc -mmcu='msp430f2274' -c MN/SERIAL.o -o SERIAL.o

dl_main:	msp_uwicmp	msp_uw_ids	serialdl
			msp430-gcc -mmcu='msp430f247' -Wall DL/main.c uwicmp.o uw_ids.o SERIALDL.o -o dl_main.o
			msp430-objcopy -O ihex dl_main.o main.hex

mn_main:	msp_uwicmp	msp_uw_ids	serialmn
			msp430-gcc -mmcu='msp430f2274' -Wall MN/main.c uwicmp.o uw_ids.o SERIAL.o -o mn_main.o
			msp430-objcopy -O ihex mn_main.o main.hex

debug:	msp_main
			mspdebug uif

clean:
		rm *.o

