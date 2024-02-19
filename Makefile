CC=gcc
CFLAGS=-Wall -Wextra
LDFLAGS=-lm -lGL -lGLU -lglfw

OUT=rc
OBJS=main.o

$(OUT): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm $(OBJS)
	rm $(OUT)
