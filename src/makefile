CC=gcc

binaries=mash

all: $(binaries)

mash: mash.c
	$(CC) $^ -o $@

clean:
	$(RM) -f $(binaries) *.o