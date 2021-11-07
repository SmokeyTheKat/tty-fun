#ifndef __SNAKE_H__
#define __SNAKE_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <pthread.h>

#include "list.h"
#include "pos.h"

struct snake;
struct snake_game;

struct snake_game make_snake_game(int x, int y, int width, int height);
void snake_game_spawn_food(struct snake_game* game);
void snake_game_draw_food(struct snake_game* game);
void snake_game_draw_border(struct snake_game* game);

struct snake make_snake(struct snake_game* game, char* color);
void snake_draw(struct snake* snake);
void snake_undraw_tail(struct snake* snake);
void snake_move(struct snake* snake);
void snake_interrupt_key(struct snake* snake);
bool snake_hit_self(struct snake* snake);
bool snake_hit_wall(struct snake* snake);
bool snake_is_dead(struct snake* snake);
void snake_die(struct snake* snake);
bool snake_is_on_food(struct snake* snake);
void snake_update(struct snake* snake);
void snake_begin_input(struct snake* snake);

struct snake_game
{
	int x, y;
	int width, height;
	struct pos food;
	int fps;
};

struct snake
{
	struct snake_game* game;
	struct list cells;
	struct pos vel;
	bool expand;
	uint32_t current_key;
	char* color;
};

struct snake_game make_snake_game(int x, int y, int width, int height)
{
	struct snake_game game = {
		x, y,
		width, height,
		{0, 0},
		100000,
	};
	return game;
}

void snake_game_spawn_food(struct snake_game* game)
{
	game->food = (struct pos){rand() % game->width, rand() % game->height};
	snake_game_draw_food(game);
}

void snake_game_draw_border(struct snake_game* game)
{
	cursor_move_to(game->x - 1, game->y - 1);
	for (int i = 0; i < game->width + 2; i++)
		printf("\x1b[38;2;255;255;255m" BLOCK BLOCK);

	cursor_move_to(game->x - 1, game->y + game->height);
	for (int i = 0; i < game->width + 2; i++)
		printf("\x1b[38;2;255;255;255m" BLOCK BLOCK);

	cursor_move_to(game->x - 1, game->y - 1);
	for (int i = 0; i < game->height + 2; i++)
		printf("\x1b[38;2;255;255;255m" BLOCK BLOCK "\x1b[2D\x1b[B");

	cursor_move_to(game->x + game->width, game->y - 1);
	for (int i = 0; i < game->height + 2; i++)
		printf("\x1b[38;2;255;255;255m" BLOCK BLOCK "\x1b[2D\x1b[B");
}

void snake_game_draw_food(struct snake_game* game)
{
	cursor_move_to(game->food.x + game->x, game->food.y + game->y);
	printf("\x1b[38;2;0;255;0m" BLOCK BLOCK);
}

struct snake make_snake(struct snake_game* game, char* color)
{
	struct snake snake = {
		game,
		make_list(INITIAL_CAPACITY, struct snake),
		{ 1, 0 },
		false,
		0,
		color,
	};
	return snake;
}

void snake_draw(struct snake* snake)
{
	for (list_iterate(&snake->cells, i, struct pos))
		draw_cell(snake->color, i->x + snake->game->x, i->y + snake->game->y);
}

void snake_undraw_tail(struct snake* snake)
{
	struct pos last = list_first(&snake->cells, struct pos);
	draw_cell(BGCOLOR, last.x + snake->game->x, last.y + snake->game->y);
}

void snake_move(struct snake* snake)
{
	if (!snake->expand)
	{
		snake_undraw_tail(snake);
		list_pop_back(&snake->cells, struct pos);
	}
	snake->expand = false;
	struct pos last = list_last(&snake->cells, struct pos);
	list_push(&snake->cells, pos_add(last, snake->vel), struct pos);
	snake_draw(snake);
}

void snake_interrupt_key(struct snake* snake)
{
	if (snake->current_key == 'q') quit();
	else if (snake->current_key == KEY_UP) snake->vel = (struct pos){0, -1};
	else if (snake->current_key == KEY_DOWN) snake->vel = (struct pos){0, 1};
	else if (snake->current_key == KEY_LEFT) snake->vel = (struct pos){-1, 0};
	else if (snake->current_key == KEY_RIGHT) snake->vel = (struct pos){1, 0};
}

bool snake_hit_wall(struct snake* snake)
{
	struct pos pos = list_last(&snake->cells, struct pos);
	bool res = pos.x >= snake->game->width || pos.y >= snake->game->height ||
		   pos.x < 0 || pos.y < 0;
	return res;
}

bool snake_hit_self(struct snake* snake)
{
	for (list_iterate(&snake->cells, i, struct pos))
	{
		for (list_iterate(&snake->cells, j, struct pos))
		{
			if (i != j && pos_cmp(*i, *j))
			{
				return true;
			}
		}
	}
	return false;
}

bool snake_is_dead(struct snake* snake)
{
	return snake_hit_self(snake) || snake_hit_wall(snake);
}

void snake_die(struct snake* snake)
{
	quit();
}

bool snake_is_on_food(struct snake* snake)
{
	struct pos pos = list_last(&snake->cells, struct pos);
	return pos_cmp(snake->game->food, pos);
}

void snake_update(struct snake* snake)
{
	snake_interrupt_key(snake);
	snake_move(snake);
	if (snake_is_dead(snake))
		snake_die(snake);
	if (snake_is_on_food(snake))
	{
		snake->expand = true;
		snake_game_spawn_food(snake->game);
	}
	usleep(snake->game->fps);
}

void snake_begin_input(struct snake* snake)
{
	pthread_t input_thread;
	pthread_create(&input_thread, 0, read_input, &snake->current_key);
}

#endif