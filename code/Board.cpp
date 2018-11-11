
#include "Board.h"
#include <stdio.h>
#include <cstring>

using namespace std;

Board::Board() {
    width = 19;
    height = 19;
    board = (char**)malloc(height * sizeof(char*));
    for (int i = 0; i < height; i++) {
        board[i] = (char*)malloc(width * sizeof(char));
    }
}

Board::Board(int w, int h) {
    width = w;
    height = h;
    board = (char**)malloc(height * sizeof(char*));
    for (int i = 0; i < height; i++) {
        board[i] = (char*)malloc(width * sizeof(char));
    }
}

Board Board::getCopy() const {
    Board result(width, height);
    for (int i = 0; i < height; i++) {
        std::memcpy(&result.board[i], &board[i], width * sizeof(char));
    }
    return result;
}

void Board::copyInto(Board &result) const {
    return; // TODO
}


inline bool Board::gameIsOver() const {
    char init = '-';
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (init == '-' && (board[i][j]=='B' || board[i][j]=='W')) {
                init = board[i][j];
            }
            if (board[i][j] != '-' && board[i][j] != init) {
                return false;
            }
        }
    }
    return true;
}

inline int Board::getWidth() const {
    return width;
}

inline int Board::getHeight() const {
    return height;
}

bool Board::isLegal(const Move &move, Player playerID) const {
    return true; // TODO
}

void Board::makeMove(const Move &move, Player playerID) {
    return; // TODO
}

string Board::toString() const {
    return string(); // TODO
}

