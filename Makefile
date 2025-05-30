CC = clang
CFLAGS = -Wall -Wextra -Werror -pedantic
FORMAT = clang-format -i

all: queue.o rwlock.o

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c

rwlock.o: rwlock.c rwlock.h
	$(CC) $(CFLAGS) -c rwlock.c

format:
	$(FORMAT) *.c *.h

clean:
	rm -f *.o

