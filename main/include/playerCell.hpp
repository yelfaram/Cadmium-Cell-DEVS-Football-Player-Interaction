#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <cmath>
#include <nlohmann/json.hpp>
#include <cadmium/modeling/celldevs/grid/cell.hpp>
#include <cadmium/modeling/celldevs/grid/config.hpp>
#include "playerState.hpp"

using namespace cadmium::celldevs;

//! Player cell
class player : public GridCell<playerState, double> {
    private:
    std::vector<int> currentId; // current cell id
    public:
    player(const std::vector<int>& id, const std::shared_ptr<const GridCellConfig<playerState, double>>& config): GridCell<playerState, double>(id, config) { 
        currentId = id;
    }

    [[nodiscard]] playerState localComputation(playerState state, const std::unordered_map<std::vector<int>, NeighborData<playerState, double>>& neighborhood) const override {
        // track neighbor states (has_player and/or has_ball)
        bool north_cell_empty = false;              // check if north cell (i-1, j) empty   
        bool south_cell_empty = false;              // check if south cell (i+1, j) empty

        bool west_teammate = false;                 // check if west cell (i, j-1) has player
        bool east_teammate = false;                 // check if east cell (i, j+1) has player
        bool north_teammate = false;                // check if north cell (i-1, j) has player
        bool south_teammate = false;                // check if south cell (i+1, j) has player

        bool north_extended_teammate = false;       // check if north cell (i-2, j) has player (extended von Neumann)
        bool south_extended_teammate = false;       // check if south cell (i+2, j) has player (extended von Neumann)

        // track neighbor states (actions and/or mental & fatigue attributes)
        bool received_dribble_from_south = false;   // flag for south cell wants to dribble north
        bool received_move_from_south = false;      // flag for south cell wants to move north

        bool received_dribble_from_north = false;   // flag for north cell wants to move south
        bool received_move_from_north = false;      // flag for north cell wants to move south

        bool received_north_dribble = false;        // flag for neighbor (east or west) dribbles north
        bool received_south_dribble = false;        // flag for neighbor (east or west) dribbles south

        bool received_short_pass_from_east = false; // flag for east cell wants to short pass west
        bool received_short_pass_from_west = false; // flag for west cell wants to short pass east  
        
        bool received_long_pass_from_north = false; // flag for north cell wants to long pass south
        bool received_long_pass_from_south = false; // flag for south cell wants to long pass north

        bool received_extended_long_pass_from_north = false; // flag for extended north cell wants to long pass south
        bool received_extended_long_pass_from_south = false; // flag for extended south cell wants to long pass north

        double source_mental = 50.0;
        double source_fatigue = 0.0;
       
        //////////////////////////////////////////////////////////////
        // Data Collection (loop through neighborhood - von Neumann)
        //////////////////////////////////////////////////////////////
        for(const auto& [neighborId, neighborData]: neighborhood) {
            // skip self neighbor
            if (neighborId == currentId) continue;

            // compute neighbor relative coordinate
            std::vector<int> relativePos(neighborId.size());
            // transform must 
            // => specify beginning and end first input range 
            // => specify begginning of second input range 
            //    (no need to give end as it will assume second input is as long as first input)
            // => specify output range
            // => specify operation to apply element wise at each iteration
            std::transform(neighborId.begin(), neighborId.end(), currentId.begin(), relativePos.begin(), std::minus<int>());

            const auto nState = neighborData.state;

            // North neighbor (i-1, j)
            if (relativePos == std::vector<int>{-1, 0}) { 
                // check north cell emptiness (to dribble/move forward)
                north_cell_empty = !nState->has_player;

                // check for direct north teammate
                north_teammate = (nState->has_player && !nState->has_ball);
                // if my direct north neighbor has an action to long pass to south (me) then i should receive ball
                received_long_pass_from_north = (nState->action == "long_pass" && nState->direction == "south");
                
                // record player action
                received_dribble_from_north = (nState->action == "dribble" && nState->direction == "south"); // North cell (i-1, j) wants to dribble backward
                received_move_from_north = (nState->action == "move" && nState->direction == "south");       // North cell (i-1, j) wants to move backward
                // get source player metrics for inheritance by new cell
                if (received_dribble_from_north || received_move_from_north) {
                    source_mental = nState->mental;   
                    source_fatigue = nState->fatigue;  
                }
            }
            // West neighbor (i, j-1)
            else if (relativePos == std::vector<int>{0, -1}) {
                // check for teammates in same line (left)
                west_teammate = (nState->has_player && !nState->has_ball);
                // if my direct west neighbor has an action to short pass to east (me) then i should receive ball
                received_short_pass_from_west = (nState->action == "short_pass" && nState->direction == "east"); // Neighbor cell performs a short pass
            }
            // East neighbor (i, j+1)
            else if (relativePos == std::vector<int>{0, 1}) {
                // check for teammates in same line (right)
                east_teammate = (nState->has_player && !nState->has_ball); 
                // if my direct east neighbor has an action to short pass to west (me) then i should receive ball
                received_short_pass_from_east = (nState->action == "short_pass" && nState->direction == "west"); // Neighbor cell performs a short pass
            }
            // South Neighbor (i+1, j)
            else if (relativePos == std::vector<int>{1, 0}) {
                // check north cell emptiness (to dribble/move backward)
                south_cell_empty = !nState->has_player;

                // check for direct south teammate
                south_teammate = (nState->has_player && !nState->has_ball);
                // if my direct south neighbor has an action to long pass to north (me) then i should receive ball
                received_long_pass_from_south = (nState->action == "long_pass" && nState->direction == "north");

                // record player action
                received_dribble_from_south = (nState->action == "dribble" && nState->direction == "north");      // South cell (i+1, j) wants to dribble forward
                received_move_from_south = (nState->action == "move" && nState->direction == "north");            // South cell (i+1, j) wants to move forward
                // get source player metrics for inheritance by new cell
                if (received_dribble_from_south || received_move_from_south) {
                    source_mental = nState->mental;   
                    source_fatigue = nState->fatigue;  
                }
            }
            // North Neighbor extended (i-2, j)
            else if (relativePos == std::vector<int>{-2, 0}) {
                // check for extended north teammate
                north_extended_teammate = (nState->has_player && !nState->has_ball);
                // if my extended north neighbor has an action to long pass to south (me) then i should receive ball
                received_extended_long_pass_from_north = (nState->action == "long_pass" && nState->direction == "south");    // Neighbor cell performs long pass
            }
            // South Neighbor extended (i+2, j)
            else if (relativePos == std::vector<int>{2, 0}) {
                // check for extended south teammate
                south_extended_teammate = (nState->has_player && !nState->has_ball);\
                // if my extended south neighbor has an action to long pass to north (me) then i should receive ball
                received_extended_long_pass_from_south = (nState->action == "long_pass" && nState->direction == "north");    // Neighbor cell performs long pass
            }

            // East or west neighbor performs a dribble (to allow for move action)
            if (relativePos == std::vector<int>{0, -1} || relativePos == std::vector<int>{0, 1}) {
                received_north_dribble = (nState->action == "dribble" && nState->direction == "north");       // Neighbor cell (west or east - same line) performs a dribble north
                received_south_dribble = (nState->action == "dribble" && nState->direction == "south");       // Neighbor cell (west or east - same line) performs a dribble south
            }
        }

        //////////////////////////////////////////////////////////////
        // Perform Actions (Cell logic when having ball)
        //////////////////////////////////////////////////////////////
        if (state.has_player && state.has_ball) {
            // Rule 1: Short pass to west or east teammate
            if (state.fatigue > 30.0 && state.fatigue < 70.0 && state.mental < 60.0) {
                if (east_teammate) {
                    state.action = "short_pass";
                    state.direction = "east";
                    // ball transfered to east cell after delay
                    state.has_ball = false;
                    // action cost -> mental/fatigue fluctuations
                    state.fatigue += 3.0;
                    state.mental -= 1.5;
                } 
                else if (west_teammate) {
                    state.action = "short_pass";
                    state.direction = "west";
                    // ball transfered to west cell after delay
                    state.has_ball = false;
                    // action cost -> mental/fatigue fluctuations
                    state.fatigue += 3.0;
                    state.mental -= 1.5;
                }
                else {
                    // Rule 4: Can't perform action
                    state.action = "hold";
                    state.direction = "None";

                    state.mental -= 1.0;
                    state.fatigue += 2.0;
                }
            }
            // Rule 2: Long pass to north or south teammate (includes extended teammate)
            else if (state.fatigue > 25.0 && state.mental > 60.0 && state.mental < 70.0) {
                if (north_extended_teammate || north_teammate) {
                    state.action = "long_pass";
                    state.direction = "north";
                    // ball transfered to north extended cell after delay
                    state.has_ball = false;
                    // action cost -> mental/fatigue fluctuations
                    state.mental -= 2.5;
                    state.fatigue += 4.0;
                }
                else if (south_extended_teammate || south_teammate) {
                    state.action = "long_pass";
                    state.direction = "south";
                    // ball transfered to north extended cell after delay
                    state.has_ball = false;
                    // action cost -> mental/fatigue fluctuations
                    state.mental -= 2.5;
                    state.fatigue += 4.0;
                }
                else {
                    // Rule 4: Can't perform action
                    state.action = "hold";
                    state.direction = "None";

                    state.mental -= 1.0;
                    state.fatigue += 2.0;
                }
            }
            // Rule 3: Dribble north/south if possible
            else if (state.fatigue < 50.0 && state.mental >= 70.0) {
                if (north_cell_empty) {
                    state.action = "dribble";
                    state.direction = "north";
                    // ball and player transfered to north cell after delay
                    state.has_player = false;
                    state.has_ball = false;
                }
                else if (south_cell_empty) {
                    state.action = "dribble";
                    state.direction = "south";
                    // ball and player transfered to south cell after delay
                    state.has_player = false;
                    state.has_ball = false;
                }
                else {
                    // Rule 4: Can't perform action
                    state.action = "hold";
                    state.direction = "None";

                    state.mental -= 1.0;
                    state.fatigue += 2.0;
                }
            }
            // Rule 4: Hold the ball
            else {
                // action cost -> mental/fatigue fluctuations
                state.action = "hold";
                state.mental -= 1.0;
                state.fatigue += 2.0;
            }

        }
        else if (state.has_player && !state.has_ball) {
            // Rule 5: Move north/south if possible when neighbor dribbles
            if (state.fatigue < 40.0 && state.mental > 50.0) {
                if (north_cell_empty && received_north_dribble) {
                    state.action = "move";
                    state.direction = "north";
                    // player transfered to north cell after delay
                    state.has_player = false;
                }
                else if (south_cell_empty && received_south_dribble) {
                    state.action = "move";
                    state.direction = "south";
                    // player transfered to south cell after delay
                    state.has_player = false;
                }
                else {
                    // Rule 4: Can't perform action
                    state.action = "None";      // Reset action
                    state.direction = "None";   // Reset direction
                }
            } 
            // Rule 6: Player Recovers Mental/Fatigue if he performs no actions
            else {
                state.action = "None";      // Reset action
                state.direction = "None";   // Reset direction
                state.mental += 1.0;
                state.fatigue -= 1.0;
            }
        }

        //////////////////////////////////////////////////////////////
        // Receive Actions (Delayed Cell Neighbor Inputs)
        //////////////////////////////////////////////////////////////
        if (!state.has_player && !state.has_ball) {     // empty cell
            // Case 1: Become a player w/ ball if south/north neighbor wants to dribble to your location
            if (received_dribble_from_south || received_dribble_from_north) {
                state.has_player = true;
                state.has_ball = true;
                // Inherit player attributes from source and apply action cost (mental/fatigue fluctuations)
                state.mental = source_mental - 3.0;
                state.fatigue = source_fatigue + 7.0;
            }
            // Case 2: Become a player w/o ball if west/east neighbor dribbles (move north/south with player dribbling)
            else if (received_move_from_south || received_move_from_north) {
                state.has_player = true;
                // inherit player attributes from source and apply action cost (mental/fatigue fluctuations)
                state.mental = source_mental - 2.0;
                state.fatigue = source_fatigue + 5.0;
            }
        }
        else if (state.has_player && !state.has_ball) {     // player cell
            // Case 3: Become a player w/ ball when neighbor performs a short pass
            if (received_short_pass_from_west || received_short_pass_from_east) {
                state.has_ball = true;
                // action cost -> mental/fatigue fluctuations
                state.mental -= 1.0;
                state.fatigue += 2.5;
            }
            // Case 4: Become a player w/ ball when neighbor performs long pass
            else if (received_long_pass_from_north || received_long_pass_from_south) {
                state.has_ball = true;
                // action cost -> mental/fatigue fluctuations
                state.mental -= 2.0;
                state.fatigue += 3.5;
            }
            else if (received_extended_long_pass_from_north || received_extended_long_pass_from_south) {
                state.has_ball = true;
                // action cost -> mental/fatigue fluctuations
                state.mental -= 2.0;
                state.fatigue += 3.5;
            }
        }

        //////////////////////////////////////////////////////////////
        // Cleanup of cells that moved (action = dribble or move)
        //////////////////////////////////////////////////////////////
        if (!state.has_player && !state.has_ball) {
            // Only track cells that had actions
            if (state.action != "None") {
                state.inactive_time += 1;
    
                // Reset after 1 inactive timestep with action
                if (state.inactive_time >= 2) {
                    state.mental = 50.0;
                    state.fatigue = 0.0;
                    state.action = "None";
                    state.direction = "None";
                    state.inactive_time = 0;
                }
            }
        } else {
            state.inactive_time = 0; // Reset for active cells
        }

        // Clamp metrics
        state.fatigue = std::clamp(state.fatigue, 0.0, 100.0);
        state.mental = std::clamp(state.mental, 0.0, 100.0);

        return state;
    }

    [[nodiscard]] double outputDelay(const playerState& state) const override {
		return 1.0;
	}
};

#endif // PLAYER_HPP