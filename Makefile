

armulator :	armulator.c armemu.c armsupp.c arminit.c armvirt.c thumbemu.c armdefs.h armemu.h config.h
	gcc -Wall -o armulator armulator.c armemu.c armsupp.c arminit.c armvirt.c thumbemu.c

clean :
	rm -f armulator


