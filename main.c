#include "misw.h"
int main(int argc, const char ** argv)
{
  Board * board;
  board = NewBoard();
  board->game->state = StartedGame; // Show message on first run.
  while(board->running == 1) {
    BoardTick(board);
  }
  board = DestroyBoard(board);
  return 0;
}
