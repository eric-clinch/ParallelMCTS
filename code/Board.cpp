
#include "Board.h"
#include <stdio.h>
#include <cstring>
#include <assert.h>

using namespace std;

Board::Board() {
    width = 19;
    height = 19;
    board = (char**)malloc(height * sizeof(char*));
    for (int i = 0; i < height; i++) {
        board[i] = (char*)malloc(width * sizeof(char));
        
    }
    currPlayer = 0;
}

Board::Board(int w, int h) {
    width = w;
    height = h;
    board = (char**)malloc(height * sizeof(char*));
    for (int i = 0; i < height; i++) {
        board[i] = (char*)malloc(width * sizeof(char));
    }
    currPlayer = 0;
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
            if (init == '-' && board[i][j]!='-') {
                init = board[i][j];
            }
            if (init != '-' && board[i][j] != '-' && board[i][j] != init) {
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
    int x = (int)move.getX();
    int y = (int)move.getY();
    if (board[x][y] != '_')
        return false;
    return true;
}

/* vector<Move> getMoves() {
    return NULL;
}*/

int Board::makeMove(const Move &move, Player playerID) {
    char stone = '-';
    char enemyStone = '-';
    if (playerID == 0) {
        stone = 'W';
        enemyStone = 'B';
    } else {
        stone = 'B';
        enemyStone = 'W';
    }
    int x = (int)move.getX();
    int y = (int)move.getY();
    // adding stone to board
    board[x][y] = stone;

    // remove surrounded islands
    bool *seenGrid = (bool*)malloc(height *width * sizeof(bool));
    for (int i = 0; i < height * width; i++) {
        seenGrid[i] = false;
    }
    int points = 0;
    if (capture(x, y, stone, enemyStone, seenGrid)) {
        points = removeStones(x, y, stone);
    }
    currPlayer = !playerID;
    return points;
}

// check if current location is in an enclosed mass
bool Board::capture(int i, int j, char stone, char enemyStone, bool *seenGrid) {
    // std::cout << i << j << std::endl;
    if (seenGrid[i*width + j]) 
        return true;
    seenGrid[i*width + j] = true;
    if (board[i][j] != 'W' && board[i][j] != 'B') {
        return false;
    }
    else if (board[i][j] == enemyStone)
        return true;
    if (i - 1 >= 0) {
        bool up_flag = capture(i - 1, j, stone, enemyStone, seenGrid);
        if (up_flag == false) return false;
    } if (j + 1 < width) {
        bool right_flag = capture(i, j + 1, stone, enemyStone, seenGrid);
        if (right_flag == false) return false;
    } if (i + 1 < height) {
        bool down_flag = capture(i + 1, j, stone, enemyStone, seenGrid);
        if (down_flag == false) return false;
    } if (j - 1 >= 0) {
        bool left_flag = capture(i, j - 1, stone, enemyStone, seenGrid);
        if (left_flag == false) return false;
    }
    return true;
}

// if current location is currently surrounded, remove all stones inside circle
int Board::removeStones(int x, int y, char stone) {
    if (board[x][y] != stone)
        return 0;
    board[x][y] = '-';
    int new_count = 1;
    if (x-1 >= 0) new_count+=removeStones(x - 1, y, stone);
    if (x+1 < height) new_count+=removeStones(x + 1, y, stone);
    std::cout << new_count << std::endl;
    if (y-1 >= 0) new_count+=removeStones(x, y - 1, stone);
    if (y+1 < width) new_count+=removeStones(x, y + 1, stone);
    return new_count;
}

void Board::update(int i, int j, char stone) {
    board[i][j] = stone;
}

void Board::toString() const {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
    return;
}

