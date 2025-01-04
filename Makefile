CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lssl -lcrypto

OBJS = backup_manager.o file_handler.o Main.o file_modifier.o menu.o
NETWORK_OBJS = network.o

# Cible par défaut pour compiler les deux programmes
all: Main Network

Main: $(OBJS)
	$(CC) $(CFLAGS) -o Main $(OBJS) $(LDFLAGS)

Network: $(NETWORK_OBJS)
	$(CC) $(CFLAGS) -o Network $(NETWORK_OBJS) $(LDFLAGS)

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

network.o: network.c network.h
	$(CC) $(CFLAGS) -c network.c

%.o: %.c
	$(CC) -c $(CFLAGS) -c $< -o$@

clean:
	rm -f *.o Main Network