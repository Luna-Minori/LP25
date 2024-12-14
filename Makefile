CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lssl -lcrypto

# List of object files
OBJS = backup_manager.o file_handler.o Main.o file_modifier.o

# Target to build the executable
Main: $(OBJS)
	$(CC) $(CFLAGS) -o Main $(OBJS) $(LDFLAGS)

# Rules to build object files

file_modifier.o: file_modifier.c file_modifier.h
	$(CC) $(CFLAGS) -c file_modifier.c $(LDFLAGS)

backup_manager.o: backup_manager.c backup_manager.h
	$(CC) $(CFLAGS) -c backup_manager.c

file_handler.o: file_handler.c file_handler.h
	$(CC) $(CFLAGS) -c file_handler.c

Main.o: Main.c Main.h
	$(CC) $(CFLAGS) -c Main.c

# Clean up build files
clean:
	rm -f *.o Main