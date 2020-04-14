CFLAGS = -Wall -Wextra -pedantic -ansi -O2 -lm
SFLAGS = -fsanitize=address -fsanitize=undefined -g3 -lm
DFLAGS = -Wall -Wextra -pedantic -ansi -g3 -lm
EFLAGS = `sdl2-config --cflags` -O2 -Wall -Wextra -pedantic -Wfloat-equal -ansi
LIBS =  `sdl2-config --libs` -lm
CC = clang
BASE = nal
BEXECS = parse interp extension
SEXECS = parse_s interp_s
DEXECS = parse_d interp_d
EXECS = $(BEXECS) $(SEXECS) $(DEXECS)

all : $(BEXECS)

parse : $(BASE).c $(BASE).h
	$(CC) $(BASE).c $(CFLAGS) -o $@
parse_s : $(BASE).c $(BASE).h
	$(CC) $(BASE).c $(SFLAGS) -o $@
parse_d : $(BASE).c $(BASE).h
	$(CC) $(BASE).c $(DFLAGS) -o $@

interp : $(BASE).c $(BASE).h
	$(CC) $(BASE).c $(CFLAGS) -o $@ -DINTERP
interp_s : $(BASE).c $(BASE).h
	$(CC) $(BASE).c $(SFLAGS) -o $@ -DINTERP
interp_d : $(BASE).c $(BASE).h
	$(CC) $(BASE).c $(DFLAGS) -o $@ -DINTERP

extension : extension.c neillsdl2.c neillsdl2.h
	$(CC) extension.c neillsdl2.c -o extension $(EFLAGS) $(LIBS)

test : testparse testinterp

testparse : parse_s parse_d
	./parse_s test1.$(BASE)
	./parse_s test2.$(BASE)
	./parse_s test3.$(BASE)
	./parse_s test4.$(BASE)
	./parse_s test5.$(BASE)
	./parse_s test6.$(BASE)
	./parse_s mytest.$(BASE)
	./parse_s mytest2.$(BASE)
	valgrind ./parse_d test1.$(BASE)
	valgrind ./parse_d test2.$(BASE)
	valgrind ./parse_d test3.$(BASE)
	valgrind ./parse_d test4.$(BASE)
	valgrind ./parse_d test5.$(BASE)
	valgrind ./parse_d test6.$(BASE)
	valgrind ./parse_d mytest.$(BASE)
	valgrind ./parse_d mytest2.$(BASE)

testinterp : interp_s interp_d
	./interp_s test1.$(BASE)
	./interp_s test2.$(BASE)
	./interp_s test4.$(BASE)
	./interp_s test5.$(BASE)
	./interp_s mytest.$(BASE)
	./interp_s mytest2.$(BASE)
	valgrind ./interp_d test1.$(BASE)
	valgrind ./interp_d test2.$(BASE)
	valgrind ./interp_d test4.$(BASE)
	valgrind ./interp_d test5.$(BASE)
	valgrind ./interp_d mytest.$(BASE)
	valgrind ./interp_d mytest2.$(BASE)


clean :
	rm -fr $(EXECS)
