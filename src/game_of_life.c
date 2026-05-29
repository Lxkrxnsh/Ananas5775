#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>

#define WIDTH 80
#define HEIGHT 25
#define ALIVE 'O'
#define DEAD ' '

int init_grid(int grid[HEIGHT][WIDTH]);
void draw_grid(int grid[HEIGHT][WIDTH]);
int count_neighbors(int grid[HEIGHT][WIDTH], int y, int x);
void update_grid(int current[HEIGHT][WIDTH], int next[HEIGHT][WIDTH]);
void copy_grid(int dest[HEIGHT][WIDTH], int src[HEIGHT][WIDTH]);

int main(void) {
  int grid[HEIGHT][WIDTH] = {0};
  int next_grid[HEIGHT][WIDTH] = {0};
  int speed = 100; // Начальная задержка в миллисекундах
  int running = 1;

  if (!init_grid(grid)) {
    printf("Error: Failed to read initial state.\n");
    return 1;
  }

  /* Перенаправляем stdin на терминал для корректной работы ncurses */
  if (!freopen("/dev/tty", "r", stdin)) {
    printf("Error: Cannot open /dev/tty\n");
    return 1;
  }

  initscr();
  noecho();
  cbreak();
  nodelay(stdscr, TRUE); // Неблокирующий ввод
  curs_set(0);           // Скрываем курсор

  while (running) {
    draw_grid(grid);
    update_grid(grid, next_grid);
    copy_grid(grid, next_grid);

    int ch = getch();
    if (ch == 'a' || ch == 'A') {
      speed -= 10;
      if (speed < 10)
        speed = 10;
    } else if (ch == 'z' || ch == 'Z') {
      speed += 10;
      if (speed > 500)
        speed = 500;
    } else if (ch == ' ') {
      running = 0;
    }

    napms(speed); // Задержка из библиотеки ncurses
  }

  endwin();
  return 0;
}

int init_grid(int grid[HEIGHT][WIDTH]) {
  int ch, r = 0, c = 0;
  while ((ch = getchar()) != EOF && r < HEIGHT) {
    if (ch == '\n') {
      r++;
      c = 0;
    } else if (c < WIDTH) {
      grid[r][c] = (ch == '1') ? 1 : 0;
      c++;
    }
  }
  return 1;
}

void draw_grid(int grid[HEIGHT][WIDTH]) {
  clear();
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      mvaddch(i, j, grid[i][j] ? ALIVE : DEAD);
    }
  }
  mvprintw(0, 0, "Speed control: 'A' (faster), 'Z' (slower) | Space to exit");
  refresh();
}

int count_neighbors(int grid[HEIGHT][WIDTH], int y, int x) {
  int count = 0;
  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      if (dy == 0 && dx == 0)
        continue;
      /* Тороидальное поле: замкнуто само на себя */
      int ny = (y + dy + HEIGHT) % HEIGHT;
      int nx = (x + dx + WIDTH) % WIDTH;
      count += grid[ny][nx];
    }
  }
  return count;
}

void update_grid(int current[HEIGHT][WIDTH], int next[HEIGHT][WIDTH]) {
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      int neighbors = count_neighbors(current, i, j);
      if (current[i][j] == 1) {
        next[i][j] = (neighbors == 2 || neighbors == 3) ? 1 : 0;
      } else {
        next[i][j] = (neighbors == 3) ? 1 : 0;
      }
    }
  }
}

void copy_grid(int dest[HEIGHT][WIDTH], int src[HEIGHT][WIDTH]) {
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      dest[i][j] = src[i][j];
    }
  }
}