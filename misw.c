#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include "misw.h"

void BoardTick(Board * b)
{
  Game * g = b->game;
  BoardDraw(b);
  b->ch = getch();
  switch(b->ch) {
    case KEY_UP : { if (--g->user_y < 0) g->user_y = 0; break; }
    case KEY_DOWN : { if (++g->user_y >= g->rows) g->user_y = g->rows - 1; break; }
    case KEY_LEFT : { if (--g->user_x < 0) g->user_x = 0; break; }
    case KEY_RIGHT : { if (++g->user_x >= g->columns) g->user_x = g->columns - 1; break; }
    case 'n' : {
      if (b->game->state == StartedGame) {
        b->game->state = PlayingGame;
      }
      b->game = DestroyGame(b->game);
      b->game = NewGame(b);
      break;
    }
    case 'f' : { GameFlagCell(g); break; }
    case 'q' : { b->running = 0; break; }
    case ' ' : { GameSelectCell(g); break; }
  }
}

Board * NewBoard()
{
  srand(time(0));
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
  keypad(stdscr, TRUE);
  b->game = NewGame(b);
  return b;
}
Board * DestroyBoard(Board * b)
{
  if (b != (Board *)NULL) {
    b->game = DestroyGame(b->game);
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
  // Allcoate cells.
  g->data = malloc(sizeof(Cell *) * g->rows * g->columns);
  for(int y = 0; y < g->rows; ++y) {
    for (int x = 0; x < g->columns; ++x) {
      g->data[y * g->columns + x] = NewCell();
    }
  }
  g->user_flags = 0;
  g->user_x = 0;
  g->user_y = 0;
  g->mines = (g->rows * g->columns) / 8;
  g->flags = 0;
  for (int f = 0, a = 0; f < g->mines && a < 1024; ++a) {
    int rx = rand() % g->columns;
    int ry = rand() % g->rows;
    Cell * c = g->data[ry * g->columns + rx];
    if ((c->state & MineCell) == 0) {
      c->state |= MineCell;
      f++;
      // Attempt to increment the numbers around point
      if (ry > 0           && rx > 0             ) { g->data[(ry-1) * g->columns + (rx-1)]->value++; }
      if (ry > 0                                 ) { g->data[(ry-1) * g->columns + (rx  )]->value++; }
      if (ry > 0           && rx < (g->columns-1)) { g->data[(ry-1) * g->columns + (rx+1)]->value++; }
      if (                    rx > 0             ) { g->data[(ry  ) * g->columns + (rx-1)]->value++; }
      if (                    rx < (g->columns-1)) { g->data[(ry  ) * g->columns + (rx+1)]->value++; }
      if (ry < (g->rows-1) && rx > 0             ) { g->data[(ry+1) * g->columns + (rx-1)]->value++; }
      if (ry < (g->rows-1)                       ) { g->data[(ry+1) * g->columns + (rx  )]->value++; }
      if (ry < (g->rows-1) && rx < (g->columns-1)) { g->data[(ry+1) * g->columns + (rx+1)]->value++; }
    }
  }
  g->state = PlayingGame;
  return g;
}
Game * DestroyGame(Game * g)
{
  if (g != (Game *)NULL) {
    for(int y = 0; y < g->rows; ++y) {
      for (int x = 0; x < g->columns; ++x) {
        Cell * n = g->data[y * g->columns + x];
        g->data[y * g->columns + x] = DestroyCell(n);
      }
    }
    free(g->data);
    free(g);
  }
  return (Game *)NULL;
}

void BoardDraw(Board * b)
{
  int y,x;
  Cell * c;
  Game * g = b->game;
  // Check if we are in a winning state.
  if (g->mines == g->flags && g->mines == g->user_flags) {
    g->state = WinningGame;
  }
  mvprintw(0,0, " [misw] %dx%d ", g->columns, g->rows);
  if (g->state == LosingGame) {
    attron(COLOR_PAIR(4));
    attron(WA_STANDOUT);
    printw("  Loser  ");
    attroff(WA_STANDOUT);
    attroff(COLOR_PAIR(4));
  } else if (g->state == WinningGame) {
    attron(COLOR_PAIR(3));
    attron(WA_STANDOUT);
    printw("  Winner  ");
    attroff(WA_STANDOUT);
    attroff(COLOR_PAIR(3));
  } else {
    printw("%d mines", g->mines);
  }
  if (g->state == StartedGame) {
    mvprintw(2, 2, "Usage:");
    mvprintw(3, 2, "      [ARROWS] Move cell.");
    mvprintw(4, 2, "      [SPACE] Activate current cell.");
    mvprintw(5, 2, "      [F] Flag bomb.");
    mvprintw(6, 2, "      [N] New game.");
    mvprintw(7, 2, "      [Q] Quit game.");
    mvprintw(9, 2, "Prease [N] to start.");
    return;
  }
  for(y = 0; y < g->rows; ++y) {
    move(y*2+1,0);
    attron(WA_DIM);
    for(x = 0; x < g->columns; ++x) {
      printw("+---");
    }
    printw("+");
    attroff(WA_DIM);
    move(y*2+2,0);
    for(x = 0; x < g->columns; ++x) {
      c = g->data[y * g->columns + x];
      attron(WA_DIM);
      printw("|");
      attroff(WA_DIM);
      if (x == g->user_x && y == g->user_y) {
        addch('[');
      } else {
        addch(' ');
      }
      if ((c->state & FlaggedCell) == FlaggedCell) {
        if (g->state == PlayingGame) {
          attron(COLOR_PAIR(2));
          attron(WA_BOLD);
          addch('X');
          attroff(WA_BOLD);
          attroff(COLOR_PAIR(2));
        } else {
          if (c->state & MineCell) {
            attron(COLOR_PAIR(4));
            attron(WA_BOLD);
            addch('X');
            attroff(WA_BOLD);
            attroff(COLOR_PAIR(4));
          } else {
            attron(COLOR_PAIR(4));
            attron(WA_STANDOUT);
            addch('/');
            attroff(WA_STANDOUT);
            attroff(COLOR_PAIR(4));
          }
        }
      } else if ((c->state & HiddenCell) == HiddenCell && g->state == PlayingGame) {
        addch('?');
      } else if ((c->state & MineCell) == MineCell) {
        attron(COLOR_PAIR(4));
        attron(WA_BOLD);
        addch('%');
        attroff(WA_BOLD);
        attroff(COLOR_PAIR(4));
      } else if (c->value != 0) {
        attron(COLOR_PAIR(3));
        attron(WA_BOLD);
        addch(g->data[y * g->columns + x]->value + 48);
        attroff(WA_BOLD);
        attroff(COLOR_PAIR(3));
      } else {
        addch(' ');
      }
      if (x == g->user_x && y == g->user_y) {
        addch(']');
      } else {
        addch(' ');
      }
    }
    attron(WA_DIM);
    addch('|');
    attroff(WA_DIM);
  }
  move(y*2+1,0);
  attron(WA_DIM);
  for(x = 0; x < g->columns; ++x) {
    printw("+---");
  }
  addch('+');
  attroff(WA_DIM);
}

Cell * NewCell()
{
  Cell * c = malloc(sizeof(Cell));
  c->state = HiddenCell;
  c->value = 0;
  return c;
}
Cell * DestroyCell(Cell * c)
{
  if (c != (Cell *)NULL) {
    free(c);
  }
  return (Cell *)NULL;
}

void GameSelectCell(Game * g)
{
  Cell * c = g->data[g->user_y * g->columns + g->user_x];
  if (c->state & HiddenCell) {
    if (c->value == 0 && (c->state & MineCell) == 0) {
      FloodFillCell(g, g->user_y, g->user_x);
    } else {
      c->state &= ~HiddenCell;
    }
    if (c->state & MineCell) {
      g->state = LosingGame;
    }
  }
}

void GameFlagCell(Game * g)
{
  Cell * c = g->data[g->user_y * g->columns + g->user_x];
  if (c->state & HiddenCell) {
    if (c->state & FlaggedCell) {
      // Remove Flag
      c->state &= ~FlaggedCell;
      g->user_flags--;
      if (c->state & MineCell) {
        g->flags--;
      }
    } else {
      // Add Flag
      c->state |= FlaggedCell;
      g->user_flags++;
      if (c->state & MineCell) {
        g->flags++;
      }
    }
  }
}

void FloodFillCell(Game * g, int y, int x)
{
  Cell * c;
  if (y >= 0 && y < g->rows && x >= 0 && x < g->columns) {
     c = g->data[y * g->columns + x];
    if ((c->state & HiddenCell) == HiddenCell) {
        c->state &= ~HiddenCell;
    }
    if (c->value == 0 && (c->state & MineCell) != MineCell) {
      if (y > 0 && (g->data[(y-1) * g->columns + x]->state & HiddenCell) == HiddenCell) {
        FloodFillCell(g, y-1, x  ); // Top
      }
      if (x > 0 && (g->data[y * g->columns + x - 1]->state & HiddenCell) == HiddenCell) {
        FloodFillCell(g, y  , x-1); // Left
      }
      if (x < (g->columns-1) && (g->data[y * g->columns + x + 1]->state & HiddenCell) == HiddenCell) {
        FloodFillCell(g, y  , x+1); // Right
      }
      if (y < (g->rows-1) && (g->data[(y+1) * g->columns + x]->state & HiddenCell) == HiddenCell) {
        FloodFillCell(g, y+1, x  ); // Bottom
      }
    }
  }
}
