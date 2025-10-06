CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11
SRCDIR = src
BINDIR = bin
SRC = $(SRCDIR)/ls-v1.0.0.c
TARGET = $(BINDIR)/ls

all: $(TARGET)

$(BINDIR):
	mkdir -p $(BINDIR)

$(TARGET): $(SRC) | $(BINDIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -rf $(BINDIR)/*.o $(TARGET)

.PHONY: all clean
