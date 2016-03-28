test: testdaemon.c testpickfile.c testcronparser.c daemonize.c cronparser.c pickfile.c
	gcc -o testpickfile testpickfile.c cronparser.c pickfile.c 
	gcc -o testcronparser testcronparser.c cronparser.c
	gcc -o testdaemon testdaemon.c daemonize.c
