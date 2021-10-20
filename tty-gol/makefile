PREFIX = ~/.local

all:
	gcc ./tty-gol.c -o ./tty-gol -Wall
tc: all
	./tty-gol
install:
	mkdir -p ${PREFIX}/bin/
	cp ./tty-gol ${PREFIX}/bin/
