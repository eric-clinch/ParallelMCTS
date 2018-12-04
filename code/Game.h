#ifndef GAME_h
#define GAME_h

#include <Board.h>
#include <Strategy.h>
#include <MCTS.h>
#include <assert.h>
#include <iostream>
#include <vector>

class Game {
 private:
  Board* B;
  int max_games;
  Strategy* S0;
  Strategy* S1;
  vector<unsigned int> s0_stones_vec;
  vector<unsigned int> s1_stones_vec;

 public:
  Game(Strategy* s0, Strategy* s1, int n) {
    B = new Board();
    max_games = n;
    S0 = s0;
    S1 = s1;
  }

  virtual int runGame() {
    int trials = max_games;
    unsigned int s0_wins = 0;
    unsigned int s1_wins = 0;
    unsigned int s0_stones;
    unsigned int s1_stones;
    Player curr_p;
    Player enemy_p;
    while (trials) {
      std::cout << trials << std::endl;
      curr_p = P0;
      enemy_p = P1;
      s0_stones = 0;
      s1_stones = 0;
      // initial moves to avoid bug of returning on empty board
      Move m = S0->getMove(*B, curr_p, enemy_p);
      std::cout << m.getRow() << " " << m.getCol() << "\n";
      B->makeMove(m, curr_p);
      std::cout << B->toString();
      m = S1->getMove(*B, enemy_p, curr_p);
      B->makeMove(m, enemy_p);
      std::cout << B->toString();
      while (!(B->gameIsOver())) {
        if (curr_p == P0) {
          m = S0->getMove(*B, curr_p, enemy_p);
        } else {
          m = S1->getMove(*B, curr_p, enemy_p);
        }
        int captured_stones = B->makeMove(m, curr_p);
        if (curr_p == P0) {
          s0_stones += captured_stones;
        } else {
          s1_stones += captured_stones;
        }
        std::cout << B->toString();
        curr_p = (curr_p == P0) ? P1 : P0;
        enemy_p = (enemy_p == P1) ? P0 : P1;
      }
      if (B->playerScore(P0))
        s0_wins++;
      else
        s1_wins++;
      s0_stones_vec.push_back(s0_stones);
      s1_stones_vec.push_back(s1_stones);
      trials--;
    }
    return s0_wins;
  }
};

#endif
