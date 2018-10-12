#ifndef MISW_H
#define MISW_H

enum CellState {
  BlankCell   = 0,
  HiddenCell  = 1,
  NumericCell = 2,
  MineCell    = 4,
  FlaggedCell = 8
};

struct Cell_t {
  int
    state,
    value;
};
typedef struct Cell_t Cell;
Cell * NewCell();
Cell * DestroyCell(Cell * c);

enum GameState {
  StartedGame = 0,
  PlayingGame = 1,
  WinningGame = 2,
  LosingGame  = 3,
};

struct Game_t {
  int
    columns,
    flags,
    mines,
    rows,
    state,
    user_flags,
    user_x,
    user_y;
  Cell ** data;
};
typedef struct Game_t Game;

struct Board_t {
  int
    ch,
    running,
    w,
    h;
  Game * game;
};
typedef struct Board_t Board;

Board * NewBoard();
Board * DestroyBoard(Board * b);
Game * NewGame(Board * b);
Game * DestroyGame(Game * g);
void BoardTick(Board * b);
void BoardDraw(Board * b);
void GameSelectCell(Game * g);
void GameFlagCell(Game * g);
void FloodFillCell(Game * g, int y, int x);

#endif