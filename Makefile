CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -g
SRCS    = main.c patient.c file.c observation.c medecin.c \
          historique.c fichiers.c catastrophe.c menu.c utils.c
OBJS    = $(SRCS:.c=.o)
TARGET  = urgences

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c structures.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) *.txt

run: all
	./$(TARGET)

.PHONY: all clean run
