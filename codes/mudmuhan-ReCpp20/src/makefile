# 수남이 에라 메세지 -Wall
# 또 다른 컴파일 옵션 -DRECORD_ALL 용량많아 지웠음..debug넣음
# 정균이 -DDEBUG없앰.
# 오토셧다운 없앰. -DAUTOSHUTDOWN
# 라토크 savegame_nomsg 함수를 넣고 싶은 경우 즉, 복사가 많이 되고 있을때에는
#       -DNODUPLE 옵션을 넣어주기를 바람.
CFLAGS = -g -O2 -DFINGERACCT -DNODUPLE -DCHECKDOUBLE -DSUICIDE
LIBS = -lm
CC = cc
OUTFILE = frp.new

OBJECTS = global.o main.o files1.o files2.o files3.o bank.o command1.o command2.o \
	  command3.o command4.o command5.o command6.o command7.o \
	  command8.o command9.o command10.o command11.o command12.o magic1.o magic2.o \
	 magic3.o magic4.o  magic5.o magic6.o magic7.o magic8.o \
	 dm1.o dm2.o dm3.o dm4.o dm5.o dm6.o io.o \
	  player.o creature.o room.o object.o update.o post.o action.o misc.o \
	  special1.o compress.o alias.o board.o

$(OUTFILE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(OUTFILE) $(LIBS)
	/bin/chmod 777 $(OUTFILE)
	mv ../bin/frp ../bin/frp.old
	cp frp.new ../bin/frp
#	strip $(OUTFILE)

# Need to add options to make utils here too
auth: auth.o
	$(CC) auth.o -o auth
	mv auth ../bin

clean: 
	rm $(OBJECTS)

port:
	rm -f io.o main.o;
	make

fing: finger.o
	$(CC) finger.o -o fing
	mv fing ../bin

