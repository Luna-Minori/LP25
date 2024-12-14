#CC = gcc
#CFLAGS = -Wall
#LIBS = -lssl -lcrypto
#TARGET = file_handler
#SRCS = $(wildcard *.c)
#OBJS = $(SRCS:.c=.o)

#all: $(TARGET)

#$(TARGET): $(OBJS)
#	#$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

#%.o: %.c
#	$(CC) $(CFLAGS) -c $< -o $@

#clean:
#	rm -f $(TARGET) $(OBJS)

CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lssl -lcrypto

# List of object files
OBJS = backup_manager.o file_handler.o Main.o

# Target to build the executable
Main: $(OBJS)
	$(CC) $(CFLAGS) -o Main $(OBJS) $(LDFLAGS)

# Rules to build object files
backup_manager.o: backup_manager.c backup_manager.h
	$(CC) $(CFLAGS) -c backup_manager.c

file_handler.o: file_handler.c file_handler.h
	$(CC) $(CFLAGS) -c file_handler.c

Main.o: Main.c Main.h
	$(CC) $(CFLAGS) -c Main.c

# Clean up build files
clean:
	rm -f *.o Main