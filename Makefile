COMPILER = gcc
CCFLAGS = -Wall -g

all: index

module1: index

tokenizer.o: tokenizer.c tokenizer.h
	$(COMPILER) $(CCFLAGS) -c tokenizer.c tokenizer.h

indexer.o: indexer.c indexer.h
	$(COMPILER) $(CCFLAGS) -c indexer.c indexer.h

index: tokenizer.o indexer.o
	$(COMPILER) $(CCFLAGS) -o index tokenizer.o indexer.o


clean:
	rm -f *.o
	rm -f index
	rm -f *.h.gch
