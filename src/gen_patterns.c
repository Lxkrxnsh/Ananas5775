#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define WIDTH 80
#define HEIGHT 25

#define ALIVE 'O'
#define DEAD ' '

static void init_grid(int grid[HEIGHT][WIDTH]);
static void draw_grid(int grid[HEIGHT][WIDTH]);
static int count_neighbors(int grid[HEIGHT][WIDTH], int y, int x);
static void update_grid(int current[HEIGHT][WIDTH], int next[HEIGHT][WIDTH]);
static void copy_grid(int dest[HEIGHT][WIDTH], int src[HEIGHT][WIDTH]);

int main(void) {
  int grid[HEIGHT][WIDTH] = {0};
  int next[HEIGHT][WIDTH] = {0};
  int running = 1;
  int speed = 100;

  init_grid(grid);

  FILE *tty = fopen("/dev/tty", "r");
  if (tty == NULL) {
    return 1;
  }

  SCREEN *term = newterm(NULL, stdout, tty);
  if (term == NULL) {
    fclose(tty);
    return 1;
  }
  set_term(term);

  noecho();
  cbreak();
  nodelay(stdscr, TRUE);
  curs_set(FALSE);

  while (running) {
    int ch;
    while ((ch = getch()) != ERR) {
      if (ch == 'a' || ch == 'A') {
        speed -= 10;
        if (speed < 20)
          speed = 20;
      } else if (ch == 'z' || ch == 'Z') {
        speed += 10;
        if (speed > 500)
          speed = 500;
      } else if (ch == ' ') {
        running = 0;
      }
    }

    draw_grid(grid);
    update_grid(grid, next);
    copy_grid(grid, next);

    usleep(speed * 1000);
  }

  endwin();
  delscreen(term);
  fclose(tty);

  return 0;
}

static void init_grid(int grid[HEIGHT][WIDTH]) {
  int ch;
  int r = 0;
  int c = 0;

  while ((ch = getchar()) != EOF && r < HEIGHT) {
    if (ch == '\n') {
      r++;
      c = 0;
    } else if (c < WIDTH) {
      grid[r][c] = (ch == '1') ? 1 : 0;
      c++;
    }
  }
}

static void draw_grid(int grid[HEIGHT][WIDTH]) {
  erase();

  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      mvaddch(i, j, grid[i][j] ? ALIVE : DEAD);
    }
  }

  mvprintw(HEIGHT - 1, 0, "A - faster | Z - slower | SPACE - quit");

  refresh();
}

static int count_neighbors(int grid[HEIGHT][WIDTH], int y, int x) {
  int count = 0;

  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      if (dy == 0 && dx == 0)
        continue;

      int ny = (y + dy + HEIGHT) % HEIGHT;
      int nx = (x + dx + WIDTH) % WIDTH;

      count += grid[ny][nx];
    }
  }
  return count;
}

static void update_grid(int current[HEIGHT][WIDTH], int next[HEIGHT][WIDTH]) {
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      int n = count_neighbors(current, i, j);

      if (current[i][j] == 1)
        next[i][j] = (n == 2 || n == 3);
      else
        next[i][j] = (n == 3);
    }
  }
}

static void copy_grid(int dest[HEIGHT][WIDTH], int src[HEIGHT][WIDTH]) {
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      dest[i][j] = src[i][j];
    }
  }
}