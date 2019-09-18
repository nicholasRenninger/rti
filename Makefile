CC     = g++
CFLAGS = -O4 -DNDEBUG -Wall -Wno-deprecated -Wno-sign-compare
MACFLAGS = -L /usr/local/lib/ -I /usr/local/include
RTI_CODE = globals.cpp bestfirst_searcher.cpp score.cpp timed_automaton.cpp timed_data.cpp timed_structures.cpp -o build/rti -I.
GENERATOR_CODE = generator.cpp globals.cpp score.cpp timed_automaton.cpp timed_data.cpp timed_structures.cpp -o build/generator -I.

linux-rti:
	$(CC) $(CFLAGS) $(RTI_CODE)

mac-rti:
	$(CC) $(CFLAGS) $(MACFLAGS) $(RTI_CODE)

mac-generator:
	$(CC) $(CFLAGS) $(MACFLAGS) $(GENERATOR_CODE)

clean:
	-rm -f rti generator

