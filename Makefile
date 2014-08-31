OBJECTS = main.o midi.o ident.o sysex.o text.o
BINARY = mksd
CC = gcc
CFLAGS = -Wall

$(BINARY) : $(OBJECTS)
	$(CC) $(CFLAGS) -lm $^ -o $@

.SUFFIXES : .c .o

.c.o:
	$(CC) $(CFLAGS) -c $<

main.o: main.c types.h midi.h ident.h sysex.h text.h
midi.o: midi.c midi.h types.h
ident.o: ident.c ident.h types.h midi.h
sysex.o: sysex.c sysex.h types.h midi.h ident.h
text.o: text.c text.h

.PHONY : clean

clean :
	rm -f $(BINARY) $(OBJECTS)

