#include "board.h"

using namespace std;

class Board {

    static inline void deleteBoard() {
        for (int i = 0; i < height; i++) {
            free(board[i]);
        }
        free(board);
    }

    virtual inline void copyBoard(Board &blankBoard) {
        for (int i = 0; i < height; i++) {
            memcpy(blankBoard[i], board[i], width * sizeof(char));
        }
    }


}
