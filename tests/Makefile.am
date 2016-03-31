check_PROGRAMS = testcronparser testdaemonize testpickfile testrtrd
testcronparser_SOURCES = testcronparser.c \
			 tap.c \
			 $(abs_top_srcdir)/src/cronparser.c \
			 $(abs_top_srcdir)/src/memory.c
testdaemonize_SOURCES = testdaemonize.c tap.c \
			$(abs_top_srcdir)/src/daemonize.c
testpickfile_SOURCES = testpickfile.c \
		       $(abs_top_srcdir)/src/cronparser.c \
		       $(abs_top_srcdir)/src/memory.c \
		       $(abs_top_srcdir)/src/pickfile.c
testrtrd_SOURCES = testrtrd.c \
		   tap.c \
		   $(abs_top_srcdir)/src/sockets.c

TESTS = $(check_PROGRAMS)
