PREFIX=~/.local

all:
	gcc ./src/tty-snake.c -o ./tty-snake -lm -lpthread -g
tc: all
	./tty-snake
install: all
	cp ./tty-snake ${PREFIX}/bin/