#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

struct Board_t {
  int ch,running,w,h;
};
typedef struct Board_t Board;
Board * NewBoard();
Board * DestroyBoard(Board * b);

struct Game_t {
  int columns, rows;
};
typedef struct Game_t Game;
Game * NewGame(Board * b);

void DrawBoard(Board * b, Game * g);

int main(int argc, const char ** argv)
{
  Board * board;
  board = NewBoard();
  mvprintw(0,0, " [misw] %dx%d", board->w, board->h);
  /* How big can the board be? */
  int y,x;
  for(y = 0; y < rows; ++y) {
    move(y*2+1,0);
    for(x = 0; x < columns; ++x) {
      printw("+---");
    }
    printw("+");
    move(y*2+2,0);
    for(x = 0; x < columns; ++x) {
      printw("| ? ");
    }
    printw("|");
  }
  move(y*2+1,0);
  for(x = 0; x < columns; ++x) {
    printw("+---");
  }
  printw("+");
  while(board->running == 1) {
    board->ch = getch();
    switch(board->ch) {
      case 'q' : { board->running = 0; break; }
    }
  }
  board = DestroyBoard(board);
  return 0;
}


Board * NewBoard()
{
  Board * b = malloc(sizeof(Board));
  initscr();
  b->running = 1;
  getmaxyx(stdscr, b->h, b->w);
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_YELLOW, COLOR_BLACK);
  init_pair(3, COLOR_GREEN, COLOR_BLACK);
  init_pair(4, COLOR_RED, COLOR_BLACK);
  clear();
  noecho();
  cbreak();
  curs_set(0);
  return b;
}
Board * DestroyBoard(Board * b)
{
  if (b != (Board *)NULL) {
    free(b);
    clrtoeol();
    refresh();
    endwin();
  }
  return (Board *)NULL;
}

Game * NewGame(Board * b)
{
  Game * g = malloc(sizeof(Game));
  g->columns = b->w / 4;
  if ((g->columns * 4 + 1) > b->w) {
    g->columns -= 1;
  }
  g->rows = b->h / 2;
  g->rows -= 1; // Remove two ascii rows for menu and last row.
  return g;
}
Game * DestroyGame(Game * g)
{
  if (g != (Game *)NULL) {
    free(g);
  }
  return (Game *)NULL;
}