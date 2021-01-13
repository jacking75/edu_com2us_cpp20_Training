CFLAGS = -m486 -O2 
LIBS = -lm -s 
CC = cc

OBJECTS = global.o main.o files1.o files2.o command1.o command2.o \
	  command3.o command4.o command5.o command6.o command7.o \
	  command8.o magic1.o magic2.o magic3.o dm1.o io.o player.o \
	  creature.o room.o object.o update.o post.o misc.o

OBJECTS2 = editor.o editor2.o editor3.o editor4.o compress.o

OBJECTS3 = chat.o

editor: $(OBJECTS2)
	$(CC) $(CFLAGS) $(OBJECTS2) -o editor $(LIBS)
	chmod o-rwx editor 

frp: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o frp $(LIBS)
	chmod o-wrx frp

chat: $(OBJECTS3)
	$(CC) $(CFLAGS) $(OBJECTS3) -o chat $(LIBS)
	chmod o-rwx chat

clean: 
	rm $(OBJECTS) $(OBJECTS2)

