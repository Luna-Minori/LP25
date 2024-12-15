CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lssl -lcrypto

OBJS = backup_manager.o file_handler.o Main.o file_modifier.o menu.o

Main: $(OBJS)
	$(CC) $(CFLAGS) -o Main $(OBJS) $(LDFLAGS)


file_modifier.o: file_modifier.c file_modifier.h
	$(CC) $(CFLAGS) -c file_modifier.c $(LDFLAGS)

backup_manager.o: backup_manager.c backup_manager.h
	$(CC) $(CFLAGS) -c backup_manager.c

file_handler.o: file_handler.c file_handler.h
	$(CC) $(CFLAGS) -c file_handler.c

menu.o: menu.c menu.h
	$(CC) $(CFLAGS) -c menu.c


Main.o: Main.c Main.h
	$(CC) $(CFLAGS) -c Main.c

clean:
	rm -f *.o Main