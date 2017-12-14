
SRC_H = $(wildcard *.h)
TARGETS = genzbf checksum

CC = gcc
CFLAGS = -Wall -O2 -g

all: $(TARGETS)

clean:
	$(RM) -f $(TARGETS)

genzbf:	genzbf.c $(SRC_H)
#	$(CC) -DCONFIG_SIGBLOCK_SUPPORT $(CFLAGS) -o $@ genzbf.c
	$(CC) $(CFLAGS) -o $@ genzbf.c

checksum: checksum.c $(SRC_H)
	$(CC) $(CFLAGS) -o $@ checksum.c

