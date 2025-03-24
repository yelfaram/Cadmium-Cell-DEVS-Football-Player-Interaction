#ifndef PLAYER_STATE_HPP
#define PLAYER_STATE_HPP

#include <iostream>
#include <nlohmann/json.hpp>

//! Player cell state
struct playerState {
    bool has_player;        // tracks whether there is a player on cell
    bool has_ball;          // tracks whether there is a ball on cell
    double mental;          // tracks player mental level [0, 100]  -> 100 indicates high confidence
    double fatigue;         // tracks player fatigue level [0, 100] -> 100 indicates high exhaustion
    std::string action;     // tracks player performed actions
    std::string direction;  // tracks direction of action
    int inactive_time;      // tracks how long cell was inactive

    //! Default constructor function
    playerState(): has_player(false), has_ball(false), mental(50.0), fatigue(0.0), action("None"), direction("None"), inactive_time(0) {}
};

//! It prints the state variables of the cell in an output stream
std::ostream& operator<<(std::ostream& os, const playerState& x) {
    // os << "{has_player: " << ((x.has_player) ? 1 : 0) << ", has_ball: " << ((x.has_ball) ? 1 : 0) << ", mental: " << x.mental << ", fatigue: " << x.fatigue << ", action: " << x.action << ", direction: " << x.direction << ", inactive_time: " << x.inactive_time << "}";
    os << "<" << ((x.has_player) ? 1 : 0) << "," << ((x.has_ball) ? 1 : 0) << "," << x.mental << "," << x.fatigue << "," << x.action << "," << x.direction << "," << x.inactive_time << ">";
    return os;
}

//! The simulator must be able to compare the equality of two state objects
bool operator!=(const playerState& x, const playerState& y) {
    return ((x.has_player != y.has_player) || (x.has_ball != y.has_ball) || (x.mental != y.mental) || (x.fatigue != y.fatigue) || (x.action != y.action) || (x.direction != y.direction));
}

//! The simulator must be able to sort messages somehow (priority queue) and required for transport delay
bool operator<(const playerState& lhs, const playerState& rhs){ 
    return true; 
}

//! It parses a JSON file and generates the corresponding playerState object
void from_json(const nlohmann::json& j, playerState& s) {
    j.at("has_player").get_to(s.has_player);
    j.at("has_ball").get_to(s.has_ball);
    j.at("mental").get_to(s.mental);
    j.at("fatigue").get_to(s.fatigue);
    j.at("action").get_to(s.action);
}

#endif // PLAYER_STATE_HPP