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

enum class ZoneType {
    NONE,
    DEFENSE,
    MIDFIELD,
    ATTACK
};

istream& operator>>(istream& is, Action& action);
istream& operator>>(istream& is, Direction& direction);
istream& operator>>(istream& is, ZoneType& zone_type);

ostream& operator<<(ostream& out, const Action& action);
ostream& operator<<(ostream& out, const Direction& direction);
ostream& operator<<(ostream& out, const ZoneType& zone_type);

#endif