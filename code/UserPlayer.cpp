
#include "UserPlayer.h"
#include <iostream>
#include <string>

UserPlayer::UserPlayer() {}

void displayBoard(Board &board) {
  std::cout << "BOARD\n";
  std::cout << board.toString();
  std::cout << "END BOARD\n";
}

const Move UserPlayer::getMove(const Board &original, Player playerID,
                               Player enemyID) {
  Board board = original.getCopy();

  const char stone = playerID == P0 ? Board::P0STONE : Board::P1STONE;
  std::cout << stone << "'s move" << std::endl;
  displayBoard(board);
  Move result;

  while (true) {
    std::cout << "Move (row col):\n" << std::flush;

    int row, col;
    bool validInput = false;
    if (std::cin >> row) {
      if (std::cin >> col) {
        if (row < 0) {
          if (row == -1 && col == -1) {
            result = Move();  // the pass move
            break;
          } else {
            exit(0);
          }
        }

        validInput = true;
        Move move(row, col);
        if (board.isLegal(move)) {
          result = move;
          break;
        } else {
          std::cout << "the given move was illegal, try again\n";
        }
      }
      if (!validInput) {
        std::cout << "the input you gave was invalid, try again\n";
      }
    }
  }

  board.makeMove(result, playerID);
  displayBoard(board);

  std::cout << "**********************************\n" << std::flush;
  return result;
}
