#define DEBUG 0

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <pthread.h>

#include "utils.h"
#include "list.h"
#include "pos.h"
#include "snake.h"

int main(int argc, char** argv)
{
	srand(time(0));
	setbuf(stdout, NULL);
	getch_init();

	screen_save();

	screen_clear();
	cursor_move_to(0, 0);

	int width, height;
	get_screen_size(&width, &height);
	width -= 2;
	height -= 2;

	struct snake_game game = make_snake_game(1, 1, width, height);
	if (argc == 2) game.fps = atoi(argv[1]);
	snake_game_draw_border(&game);
	snake_game_spawn_food(&game);

	struct snake snake = make_snake(&game, "\x1b[38;2;255;255;0m");
	snake_begin_input(&snake);
	list_push(&snake.cells, ((struct pos){2, 5}), struct pos);
	list_push(&snake.cells, ((struct pos){3, 5}), struct pos);
	list_push(&snake.cells, ((struct pos){4, 5}), struct pos);
	snake_draw(&snake);
	while (1) snake_update(&snake);

	cursor_move_to(0, 34);

	screen_restore();

	return 0;
}