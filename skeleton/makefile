CC=gcc
CFLAGS=-Wall -Wextra -g
LIBS=-lpthread -lconfig

SERVER_OBJS=threeds.o unixds.o proto.o worker.o
ADMIN_OBJS=client_admin.o proto.o
USER_OBJS=client_user.o proto.o

help:
	@echo "make serverds      - build UNIX server"
	@echo "make client_admin  - build admin client"
	@echo "make client_user   - build ordinary client"
	@echo "make clean         - remove binaries and generated media"

all: serverds client_admin client_user

serverds: $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o serverds $(SERVER_OBJS) $(LIBS)

client_admin: $(ADMIN_OBJS)
	$(CC) $(CFLAGS) -o client_admin $(ADMIN_OBJS) $(LIBS)

client_user: $(USER_OBJS)
	$(CC) $(CFLAGS) -o client_user $(USER_OBJS) $(LIBS)

threeds.o: threeds.c
	$(CC) $(CFLAGS) -c threeds.c -o threeds.o

unixds.o: unixds.c proto.h
	$(CC) $(CFLAGS) -c unixds.c -o unixds.o

proto.o: proto.c proto.h
	$(CC) $(CFLAGS) -c proto.c -o proto.o

worker.o: worker.c
	$(CC) $(CFLAGS) -c worker.c -o worker.o

client_admin.o: client_admin.c proto.h
	$(CC) $(CFLAGS) -c client_admin.c -o client_admin.o

client_user.o: client_user.c proto.h
	$(CC) $(CFLAGS) -c client_user.c -o client_user.o

clean:
	rm -f *.o serverds client_admin client_user
	rm -f /tmp/video.sock
	rm -f out_720p.mp4 out_480p.mp4 out_res1.mp4 out_res2.mp4
	rm -f thumb_*.jpg