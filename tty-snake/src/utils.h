#ifndef __UTILS_H__
#define __UTILS_H__

#define BLOCK "█"
#define BGCOLOR "\x1b[38;2;0;0;0m"
#define INITIAL_CAPACITY 20

void cursor_move_to(int x, int y);
void get_screen_size(int* x, int* y);
void screen_clear(void);
void screen_save(void);
void screen_restore(void);
void draw_cell(char* color, int x, int y);
void getch_exit(void);
void getch_init(void);
uint32_t getch(void);
void* read_input(void* v_key_out);
void quit(void);

struct termios oldt;

enum
{
	KEY_UP=438,
	KEY_DOWN=439,
	KEY_RIGHT=440,
	KEY_LEFT=441,
	KEY_DELETE=295,
	KEY_BACKSPACE=127,
	KEY_CTRL_BACKSLASH=28,
	KEY_RETURN=13,
	KEY_SHIFT_RETURN=438,
	KEY_ESCAPE=27,
	KEY_CTRL_BACKSPACE=8,
	KEY_TAB='\t',
};

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

void screen_save(void)
{
	system("tput smcup");
}
void screen_restore(void)
{
	system("tput rmcup");
	getch_exit();
}

void draw_cell(char* color, int x, int y)
{
	cursor_move_to(x, y);
	printf("%s" BLOCK BLOCK, color);
}

void getch_exit(void)
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldt);
}

void getch_init(void)
{
	tcgetattr(STDIN_FILENO, &oldt);
	struct termios newt = oldt;
	newt.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	newt.c_oflag &= ~(OPOST);
	newt.c_cflag |= (CS8);
	newt.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	newt.c_cc[VMIN] = 1;
	newt.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &newt);
}

uint32_t getch(void)
{
	uint64_t retval = 0;
	uint64_t output = 0;
	read(STDIN_FILENO, &output, 8);
	return ((output > 255) * 255) + (output % 255);
}

void* read_input(void* v_key_out)
{
	uint32_t* key_out = (uint32_t*)v_key_out;
	while (1 == 1)
	{
		*key_out = getch();
		char str[2] = { *key_out, 0 };
	}
}

void quit(void)
{
	screen_restore();
	getch_exit();
	exit(0);
}

#endif