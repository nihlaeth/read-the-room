rtrd: rtrd.c cronparser.c sockets.c daemonize.c pickfile.c
	gcc -o bin/rtrd rtrd.c cronparser.c sockets.c daemonize.c pickfile.c

test-rtrd: testrtrd.c sockets.c
	gcc -o bin/testrtrd testrtrd.c sockets.c

test: testdaemon.c testpickfile.c testcronparser.c daemonize.c cronparser.c pickfile.c testclient.c testserver.c sockets.c
	gcc -o bin/testpickfile testpickfile.c cronparser.c pickfile.c 
	gcc -o bin/testcronparser testcronparser.c cronparser.c
	gcc -o bin/testdaemon testdaemon.c daemonize.c
	gcc -o bin/testclient testclient.c sockets.c
	gcc -o bin/testserver testserver.c sockets.c
