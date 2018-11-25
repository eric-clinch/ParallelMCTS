
#include "UserPlayer.h"
#include <iostream>
#include <string>

UserPlayer::UserPlayer() {}

const Move UserPlayer::getMove(Board &board, Player playerID, Player enemyID) {
  const char stone = playerID == P0 ? Board::P0STONE : Board::P1STONE;
  std::cout << stone << "'s move" << std::endl;
  std::cout << board.toString() << std::endl;

  while (true) {
    std::cout << "Move (row col): ";
    int row, col;
    bool validInput = false;
    if (std::cin >> row) {
      if (row < 0) {
        exit(0);
      }

      if (std::cin >> col) {
        validInput = true;
        const Move move(row, col);
        if (board.isLegal(move, playerID)) {
          return move;
        } else {
          std::cout << "the given move was illegal, try again\n";
        }
      }
      if (!validInput) {
        std::cout << "the input you gave was invalid, try again\n";
      }
    }
  }
}