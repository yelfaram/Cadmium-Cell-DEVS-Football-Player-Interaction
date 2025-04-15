#include <iostream>
#include <fstream>
#include <string>

#include "utils.hpp"

/***************************************************/
/************* Input stream ************************/
/***************************************************/

istream& operator>>(istream& is, Action& action) {
    int input;
    is >> input;
    
    switch (input) {
        case 0:  action = Action::NONE; break;
        case 1:  action = Action::SHORT_PASS; break;
        case 2:  action = Action::LONG_PASS; break;
        case 3:  action = Action::DRIBBLE; break;
        case 4:  action = Action::MOVE; break;
        case 5:  action = Action::HOLD; break;
        default: action = Action::NONE; break;
    }
    return is;
}

istream& operator>>(istream& is, Direction& direction) {
    int input;
    is >> input;
    
    switch (input) {
        case 0:  direction = Direction::NONE; break;
        case 1:  direction = Direction::NORTH; break;
        case 2:  direction = Direction::SOUTH; break;
        case 3:  direction = Direction::EAST; break;
        case 4:  direction = Direction::WEST; break;
        default: direction = Direction::NONE; break;
    }
    return is;
}

istream& operator>>(istream& is, ZoneType& zone_type) {
    int input;
    is >> input;
    
    switch (input) {
        case 0:  zone_type = ZoneType::NONE; break;
        case 1:  zone_type = ZoneType::DEFENSE; break;
        case 2:  zone_type = ZoneType::MIDFIELD; break;
        case 3:  zone_type = ZoneType::ATTACK; break;
        default: zone_type = ZoneType::NONE; break;
    }
    return is;
}

/***************************************************/
/************* Output stream ************************/
/***************************************************/
ostream& operator<<(ostream &out, const Action& action) {  
    switch (action) {  
        case Action::NONE:        out << "NONE"; break;  
        case Action::SHORT_PASS:  out << "SHORT_PASS"; break;
        case Action::LONG_PASS:   out << "LONG_PASS"; break;
        case Action::DRIBBLE:     out << "DRIBBLE"; break;
        case Action::MOVE:        out << "MOVE"; break;
        case Action::HOLD:        out << "HOLD"; break;
        default: out << "ERROR"; break;
    }
    return out;  
}

ostream& operator<<(ostream &out, const Direction& direction) {  
    switch (direction) {  
        case Direction::NONE:   out << "NONE"; break;  
        case Direction::NORTH:  out << "NORTH"; break;
        case Direction::SOUTH:  out << "SOUTH"; break;
        case Direction::EAST:   out << "EAST"; break;
        case Direction::WEST:   out << "WEST"; break;
        default: out << "ERROR"; break;
    }
    return out;  
}

ostream& operator<<(ostream &out, const ZoneType& zone_type) {  
    switch (zone_type) {  
        case ZoneType::NONE:     out << "NONE"; break;  
        case ZoneType::DEFENSE:  out << "DEFENSE"; break;
        case ZoneType::MIDFIELD: out << "MIDFIELD"; break;
        case ZoneType::ATTACK:   out << "ATTACK"; break;
        default: out << "ERROR"; break;
    }
    return out;  
}