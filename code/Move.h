#ifndef MOVE_h
#define MOVE_h

#include <string>
#include <assert.h>
class Move {
 private:
    char x;
    char y;

 public:
    Move() {
        x = 0;
        y = 0;
    }

    Move(char new_x, char new_y) {
        assert(new_x < 20 && new_y < 20);
        x = new_x;
        y = new_y;
    }
    virtual char getX() const {
        return x;
    }
    virtual char getY() const {
        return y;
    }
    virtual std::string toString() const;
    virtual bool operator==(const Move &other) const;
};

#endif
