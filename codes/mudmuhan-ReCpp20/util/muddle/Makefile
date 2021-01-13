CFLAGS = -w -g
LIBS =
CC = g++

OBJECTS = muddle.o macro.o conn.o

muddle: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o muddle $(LIBS)

clean: 
	rm $(OBJECTS)

