#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define FGBLOCK "█"
#define BGBLOCK "█"
#define BGCOLOR "\x1b[38;2;0;0;0m"
#define FGCOLOR "\x1b[38;2;255;255;0m"

typedef unsigned char byte;

enum
{
	STATE_EMPTY=0,
	STATE_ALIVE=1,
};

struct gol
{
	int width;
	int height;
	
	byte** grid;
	byte** old_grid;
};

void cursor_move_to(int x, int y);
void get_screen_size(int* x, int* y);
void screen_clear(void);

struct gol make_gol(int width, int height);
void gol_randomize(struct gol* gol);
int gol_count_neighbors(struct gol* gol, int x, int y);
void gol_draw(struct gol* gol);
void gol_draw_cell(struct gol* gol, int x, int y, int state);
void gol_copy_grid(struct gol* gol);
void gol_update(struct gol* gol);

int sleep_time = 100000;

void cursor_move_to(int x, int y)
{
	x *= 2;
	printf("\x1b[%d;%dH", y+1, x+1);
}

void get_screen_size(int* x, int* y)
{
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	(*x) = w.ws_col/2;
	(*y) = w.ws_row;
}

void screen_clear(void)
{
	printf(BGCOLOR "\x1b[2J");
}

struct gol make_gol(int width, int height)
{
	struct gol gol = {width, height, 0, 0};
	gol.grid = malloc(sizeof(byte*) * height);
	gol.old_grid = malloc(sizeof(char*) * height);

	for (int y = 0; y < height; y++)
	{
		gol.grid[y] = malloc(sizeof(byte) * width);
		gol.old_grid[y] = malloc(sizeof(byte) * width);
	}

	return gol;
}

void gol_randomize(struct gol* gol)
{
	for (int y = 0; y < gol->height; y++)
		for (int x = 0; x < gol->width; x++)
			gol->grid[y][x] = rand() % 2;
}

int gol_count_neighbors(struct gol* gol, int x, int y)
{
	int count = 0;
	count += gol->grid[(y+1) % gol->height][(x-1 + gol->width) % gol->width];
	count += gol->grid[(y+1) % gol->height][x];
	count += gol->grid[(y+1) % gol->height][(x+1) % gol->width];
	count += gol->grid[y][(x-1 + gol->width) % gol->width];
	count += gol->grid[y][(x+1) % gol->width];
	count += gol->grid[(y-1 + gol->height) % gol->height][(x-1 + gol->width) % gol->width];
	count += gol->grid[(y-1 + gol->height) % gol->height][x];
	count += gol->grid[(y-1 + gol->height) % gol->height][(x+1) % gol->width];
	return count;
}

void gol_draw(struct gol* gol)
{
	screen_clear();
	for (int y = 0; y < gol->height; y++)
	{
		for (int x = 0; x < gol->width; x++)
		{
			if (gol->grid[y][x] == STATE_ALIVE)
			{
				cursor_move_to(x, y);
				printf("%s", FGCOLOR FGBLOCK FGBLOCK);
			}
		}
	}
}

void gol_draw_cell(struct gol* gol, int x, int y, int state)
{
	cursor_move_to(x, y);
	if (state == STATE_ALIVE)
		printf("%s", FGCOLOR FGBLOCK FGBLOCK);
	else printf("%s", BGCOLOR BGBLOCK BGBLOCK);
}

void gol_copy_grid(struct gol* gol)
{
	for (int y = 0; y < gol->height; y++)
		for (int x = 0; x < gol->width; x++)
			gol->grid[y][x] = gol->old_grid[y][x];
}

void gol_update(struct gol* gol)
{
	usleep(sleep_time);
	for (int y = 0; y < gol->height; y++)
	{
		for (int x = 0; x < gol->width; x++)
		{
			int state = gol->grid[y][x];

			int neighbors = gol_count_neighbors(gol, x, y);

			if (state == STATE_EMPTY && neighbors == 3)
			{
				gol->old_grid[y][x] = STATE_ALIVE;
				gol_draw_cell(gol, x, y, STATE_ALIVE);
			}
			else if (state == STATE_ALIVE && (neighbors < 2 || neighbors > 3))
			{
				gol->old_grid[y][x] = STATE_EMPTY;
				gol_draw_cell(gol, x, y, STATE_EMPTY);
			}
			else gol->old_grid[y][x] = gol->grid[y][x];
		}
	}
	gol_copy_grid(gol);
}

int main(int argc, char** argv)
{
	srand(time(0));
	setbuf(stdout, NULL);

	if (argc == 2)
		sleep_time = atoi(argv[1]);

	screen_clear();
	cursor_move_to(0, 0);


	int width, height;
	get_screen_size(&width, &height);

	struct gol gol = make_gol(width, height);
	gol_randomize(&gol);
	gol_draw(&gol);
	while (1) gol_update(&gol);

	return 0;
}
