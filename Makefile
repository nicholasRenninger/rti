CC     = g++
CFLAGS = -O3 -DNDEBUG -Wall -Wno-deprecated -Wno-sign-compare
MACFLAGS = -L/sw/lib/ -I/sw/include
CODE = globals.cpp bestfirst_searcher.cpp score.cpp timed_automaton.cpp timed_data.cpp timed_structures.cpp -o rti -I.

linux:
	$(CC) $(CFLAGS) $(CODE)

mac:
	$(CC) $(CFLAGS) $(MACFLAGS) $(CODE)

clean:
	-rm -f rti

