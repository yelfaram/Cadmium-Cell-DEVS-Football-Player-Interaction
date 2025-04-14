#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>

using namespace std;

enum class Action {
    NONE,
    SHORT_PASS,
    LONG_PASS,
    DRIBBLE,
    MOVE,
    HOLD
};

enum class Direction {
    NONE,
    NORTH,
    SOUTH,
    EAST,
    WEST
};

istream& operator>>(istream& is, Action& action);
istream& operator>>(istream& is, Direction& direction);

ostream& operator<<(ostream& out, const Action& action);
ostream& operator<<(ostream& out, const Direction& direction);

#endif