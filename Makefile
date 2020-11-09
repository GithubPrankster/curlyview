src = $(wildcard src/*.c) $(wildcard src/**/*.c)
obj = $(src:.c=.o)

CFLAGS = -Wall -Ofast -march=native -mtune=native -s -Iinclude -std=gnu99
LDFLAGS = -lSDL2 -ldl -lm

curlyview: $(obj)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) curlyview
