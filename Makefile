CC      = gcc
CFLAGS  = -O
LDFLAGS  = -O


all: shell

shell:  shell.o
	$(CC) -g -o $@ $^ $(LDFLAGS)

run:
	./shell

test:
	./shell < testfile

clean:
	rm shell


.c.o:
	$(CC)  $(CFLAGS) -c $<
