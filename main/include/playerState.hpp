#ifndef PLAYER_STATE_HPP
#define PLAYER_STATE_HPP

#include <iostream>
#include <nlohmann/json.hpp>

#include "data_structures/utils.hpp"

//! Player cell state
struct playerState {
    bool has_player;        // tracks whether there is a player on cell
    bool has_ball;          // tracks whether there is a ball on cell
    bool has_obstacle;      // tracks whether there is a stationary obstacle on cell
    bool near_obstacle;     // tracks whether cell is near an obstacle
    double mental;          // tracks player mental level [0, 100]  -> 100 indicates high confidence
    double fatigue;         // tracks player fatigue level [0, 100] -> 100 indicates high exhaustion
    Action action;          // tracks player performed actions
    Direction direction;    // tracks direction of action
    ZoneType zone_type;     // tracks type of zone player belongs to [NONE, DEFENSE, MIDFIELD, ATTACK]
    int initial_row;        // track original row for player cells         
    int inactive_time;      // tracks how long cell was inactive

    //! Default constructor function
    playerState(): 
        has_player(false), 
        has_ball(false),
        has_obstacle(false),
        near_obstacle(false),
        mental(50.0), 
        fatigue(0.0), 
        action(Action::NONE), 
        direction(Direction::NONE), 
        zone_type(ZoneType::NONE),
        initial_row(0),
        inactive_time(0) {}
};

//! It prints the state variables of the cell in an output stream
std::ostream& operator<<(std::ostream& os, const playerState& x) {
    os << "{has_player: " << ((x.has_player) ? 1 : 0) << ", has_ball: " << ((x.has_ball) ? 1 : 0) << ", has_obstacle: " << ((x.has_obstacle) ? 1 : 0) << ", near_obstacle: " << ((x.near_obstacle) ? 1 : 0) << ", mental: " << x.mental << ", fatigue: " << x.fatigue << ", action: " << x.action << ", direction: " << x.direction << ", zone_type: " << x.zone_type << ", initial_row: " << x.initial_row << ", inactive_time: " << x.inactive_time << "}"; // use this output if you want clarity on the grid log csv file
    // os << "<" << ((x.has_player) ? 1 : 0) << "," << ((x.has_ball) ? 1 : 0) << "," << ((x.has_obstacle) ? 1 : 0) << "," << ((x.near_obstacle) ? 1 : 0) << "," << x.mental << "," << x.fatigue << "," << x.action << "," << x.direction << "," << x.zone_type << "," << x.initial_row << "," << x.inactive_time << ">";  // use this output when you need to use the Cell-DEVS viewer
    return os;
}

//! The simulator must be able to compare the equality of two state objects
bool operator!=(const playerState& x, const playerState& y) {
    return (
        (x.has_player != y.has_player) || 
        (x.has_ball != y.has_ball) || 
        (x.has_obstacle != y.has_obstacle) || 
        (x.near_obstacle != y.near_obstacle) || 
        (x.mental != y.mental) || 
        (x.fatigue != y.fatigue) || 
        (x.action != y.action) || 
        (x.direction != y.direction) || 
        (x.zone_type != y.zone_type)
    );
}

//! The simulator must be able to sort messages somehow (priority queue) and required for transport delay
bool operator<(const playerState& lhs, const playerState& rhs){ 
    return true; 
}

//! It parses a JSON file and generates the corresponding playerState object
void from_json(const nlohmann::json& j, playerState& s) {
    j.at("has_player").get_to(s.has_player);
    j.at("has_ball").get_to(s.has_ball);
    j.at("has_obstacle").get_to(s.has_obstacle);
    j.at("near_obstacle").get_to(s.near_obstacle);
    j.at("mental").get_to(s.mental);
    j.at("fatigue").get_to(s.fatigue);

    // initialize action and direction to NONE always (mental and fatigue threshold is what changes these attributes)
    s.action = Action::NONE;
    s.direction = Direction::NONE;
    
    j.at("zone_type").get_to(s.zone_type);
    j.at("initial_row").get_to(s.initial_row);
    j.at("inactive_time").get_to(s.inactive_time);
}

#endif // PLAYER_STATE_HPP