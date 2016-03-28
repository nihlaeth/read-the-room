test: testdaemon.c testpickfile.c testcronparser.c daemonize.c cronparser.c pickfile.c
	gcc -o bin/testpickfile testpickfile.c cronparser.c pickfile.c 
	gcc -o bin/testcronparser testcronparser.c cronparser.c
	gcc -o bin/testdaemon testdaemon.c daemonize.c
