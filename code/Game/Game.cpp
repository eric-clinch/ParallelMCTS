
#include "Game.h"

std::random_device Game::rd;
std::mt19937 Game::rng(rd());
std::uniform_real_distribution<> Game::uni(0, 1);

// plays a game between strategy S0 and S1 on a board of the given board size
// for a max rounds of maxRounds. If maxRounds < 0, then the game will be
// played until completion. Returns 0 if S0 won and 1 if S1 won.
int Game::runGame(Strategy* S0, Strategy* S1, int boardSize, int maxRounds,
                  bool verbose) {
  Player curr_p;
  Player enemy_p;

  curr_p = P0;
  enemy_p = P1;

  Board B(boardSize, boardSize);

  // initial moves to avoid bug of returning on empty board
  Move m = S0->getMove(B, curr_p, enemy_p);
  B.makeMove(m, curr_p);
  m = S1->getMove(B, enemy_p, curr_p);
  B.makeMove(m, enemy_p);

  int round = 2;

  while (!B.gameIsOver() && (maxRounds < 0 || round < maxRounds)) {
    if (verbose) {
      std::cout << B.toString();
    }

    if (curr_p == P0) {
      m = S0->getMove(B, curr_p, enemy_p);
    } else {
      m = S1->getMove(B, curr_p, enemy_p);
    }
    B.makeMove(m, curr_p);

    curr_p = (curr_p == P0) ? P1 : P0;
    enemy_p = (enemy_p == P1) ? P0 : P1;
    round++;
  }

  if (B.getWinner() == P0) {
    return 0;
  } else {
    return 1;
  }
}

// returns a random int in the given range, inclusive of the
// lower bound and exclusive of the upper bound
int Game::randint(int lower, int upper) {
  double sample = uni(rng);
  int result = static_cast<int>(sample * (upper - lower));
  return result;
}

void Game::runTournament(const std::vector<Strategy*>& strategies,
                         int boardSize, int maxGameRounds,
                         int maxTournamentRounds) {
  assert(strategies.size() > 1);

  int round = 0;
  while (maxTournamentRounds < 0 || round < maxTournamentRounds) {
    int i = randint(0, strategies.size());
    int j = randint(0, strategies.size());
    while (i == j) {
      // make sure i and j are different
      j = randint(0, strategies.size());
    }

    Strategy* S0 = strategies[i];
    std::string S0Name = std::to_string(i) + ":" + S0->toString();
    Strategy* S1 = strategies[j];
    std::string S1Name = std::to_string(j) + ":" + S1->toString();

    int winner = runGame(S0, S1, boardSize, maxGameRounds, false);
    if (winner == 0) {
      std::cout << S0Name << " defeated " << S1Name << std::endl;
    } else {
      std::cout << S1Name << " defeated " << S0Name << std::endl;
    }

    winner = runGame(S1, S0, boardSize, maxGameRounds, false);
    if (winner == 0) {
      std::cout << S1Name << " defeated " << S0Name << std::endl;
    } else {
      std::cout << S0Name << " defeated " << S1Name << std::endl;
    }

    round++;
  }
}
