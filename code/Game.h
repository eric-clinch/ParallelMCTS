#ifndef GAME_h
#define GAME_h

#include <assert.h>
#include <iostream>
#include <vector>
#include <Board.h>
#include <Player.h>
#include <Strategy.h>

class Game {
  private:
    Board B;
    int max_games;


  public:
    Game();
    Game(int n);
}
