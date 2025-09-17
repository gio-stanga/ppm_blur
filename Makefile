# Simple Makefile for ppm_blur project

CC = gcc
CFLAGS = -Wall -Wextra -O2
OBJ = ppm_blur.o data.o
TARGET = ppm_blur

# default rule
all: $(TARGET)

# link
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) -lm

# compile .c -> .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# clean up
clean:
	rm -f $(OBJ) $(TARGET)

