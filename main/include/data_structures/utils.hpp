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

enum class PlayerRole {
    NONE,
    CENTERBACK,
    FULLBACK,
    PLAYMAKER,
    WINGER,
    TARGET_FORWARD,
    FALSE_NINE
};

istream& operator>>(istream& is, Action& action);
istream& operator>>(istream& is, Direction& direction);
istream& operator>>(istream& is, ZoneType& zone_type);
istream& operator>>(istream& is, PlayerRole& player_role);

ostream& operator<<(ostream& out, const Action& action);
ostream& operator<<(ostream& out, const Direction& direction);
ostream& operator<<(ostream& out, const ZoneType& zone_type);
ostream& operator<<(ostream& out, const PlayerRole& player_role);

#endif