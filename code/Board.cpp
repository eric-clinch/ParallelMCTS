#include "board.h"

using namespace std;

class Board {

    private:
    int width, height;
    char **board;

    public:
    Board() {
        int width = 19;
        int height = 19;
        char **Board = malloc(height * sizeof(*char));
        for (int i = 0; i < height; i++) {
            Board[i] = malloc(width * sizeof(char));
        }
        return Board;
    }

    Board(int w, int h) {
        int width = w;
        int height = h;
        char **Board = malloc(height * sizeof(char*));
        for (int i = 0; i < height; i++) {
            Board[i] = malloc(width * sizeof(char));
        }
        return Board;

    }

    virtual Board *getCopy() {
        Board blankBoard = malloc(sizeof(char*) * height);
        for (int i = 0; i < height; i++) {
            blankBoard[i] = malloc(width * sizeof(char));
            memcpy(blankBoard[i], board[i], width * sizeof(char));
        }
        return blankBoard;
    }

    virtual inline bool gameIsOver() {
        char init = '-';
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (init == '-' && (board[i][j]=='B' || board[i][j]=='W')) {
                    init = board[i][j];
                }
                if (board[i][j] != '-' && board[i][j] != init):
                    return false;
            }
        }
        return true;
    }

    virtual inline int getWidth() {
        return width;
    }

    virtual inline int getHeight() {
        return height;
    }



    static inline void deleteBoard() {
        for (int i = 0; i < height; i++) {
            free(board[i]);
        }
        free(board);
    }
}
