#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <cmath>
#include <nlohmann/json.hpp>
#include <cadmium/modeling/celldevs/grid/cell.hpp>
#include <cadmium/modeling/celldevs/grid/config.hpp>
#include "playerState.hpp"
#include "data_structures/utils.hpp"

using namespace cadmium::celldevs;

struct NeighborFlags {
    // check for empty
    bool north_empty = false;                   // check if north cell (i-1, j) empty   
    bool south_empty = false;                   // check if south cell (i+1, j) empty
    bool east_empty = false;                    // check if east cell (i, j+1) empty
    bool west_empty = false;                    // check if west cell (i, j-1) empty

    // check for obstacle
    bool obstacle_interception_north = false;   // check if north neighbor is an obstacle (can intercept long pass)
    bool obstacle_interception_south = false;   // check if south neighbor is an obstacle (can intercept long pass)

    // check for neighbor being near an obstacle
    bool near_west_obstacle = false;
    bool near_east_obstacle = false;
    bool near_north_obstacle = false;
    bool near_south_obstacle = false;

    // check for teammate
    bool west_teammate = false;                 // check if west cell (i, j-1) has player
    bool east_teammate = false;                 // check if east cell (i, j+1) has player
    bool north_teammate = false;                // check if north cell (i-1, j) has player
    bool south_teammate = false;                // check if south cell (i+1, j) has player

    bool north_extended_teammate = false;       // check if north cell (i-2, j) has player (extended von Neumann)
    bool south_extended_teammate = false;       // check if south cell (i+2, j) has player (extended von Neumann)

    // track player action for MOVE
    bool north_dribble = false;                 // flag for neighbor (east or west) dribbles north
    bool south_dribble = false;                 // flag for neighbor (east or west) dribbles south

    // track incoming players from dribbling/moving and balls from passing for delayed propagation
    bool dribble_from_south = false;            // flag for south cell wants to dribble north
    bool move_from_south = false;               // flag for south cell wants to move north

    bool dribble_from_north = false;            // flag for north cell wants to dribble south
    bool move_from_north = false;               // flag for north cell wants to move south

    bool dribble_from_east = false;            // flag for east cell wants to dribble west
    bool move_from_east = false;               // flag for east cell wants to move west

    bool dribble_from_west = false;            // flag for west cell wants to dribble east
    bool move_from_west = false;               // flag for west cell wants to move east

    bool short_pass_from_east = false;          // flag for east cell wants to short pass west
    bool short_pass_from_west = false;          // flag for west cell wants to short pass east  
    
    bool long_pass_from_north = false;          // flag for north cell wants to long pass south
    bool long_pass_from_south = false;          // flag for south cell wants to long pass north
    bool extended_long_pass_from_north = false; // flag for extended north cell wants to long pass south
    bool extended_long_pass_from_south = false; // flag for extended south cell wants to long pass north
};

//! Player cell
class player : public GridCell<playerState, double> {
    private:
    std::vector<int> currentId; // current cell id
    public:
    player(const std::vector<int>& id, const std::shared_ptr<const GridCellConfig<playerState, double>>& config): GridCell<playerState, double>(id, config) { 
        currentId = id;
    }

    [[nodiscard]] playerState localComputation(playerState state, const std::unordered_map<std::vector<int>, NeighborData<playerState, double>>& neighborhood) const override {
        NeighborFlags flags;

        double source_mental = 50.0;
        double source_fatigue = 0.0;
        int source_initial_row = 0;
        ZoneType source_zone_type = ZoneType::NONE;

        // Relative Neighbor Constants
        const std::vector<int> NORTH = {-1, 0};             // North neighbor (i-1, j)
        const std::vector<int> SOUTH = {1, 0};              // South Neighbor (i+1, j)
        const std::vector<int> EAST  = {0, 1};              // East neighbor (i, j+1)
        const std::vector<int> WEST  = {0, -1};             // West neighbor (i, j-1)

        const std::vector<int> NORTH_EXTENDED = {-2, 0};    // North Neighbor extended (i-2, j)
        const std::vector<int> SOUTH_EXTENDED = {2, 0};     // South Neighbor extended (i+2, j)

        
        //////////////////////////////////////////////////////////////
        // Helper functions (for data collection)
        //////////////////////////////////////////////////////////////
        auto isEmpty = [](const std::shared_ptr<const playerState>& s) {
            return (!s->has_player) && (!s->has_ball) && (!s->has_obstacle);
        };
        
        auto isTeammate = [](const std::shared_ptr<const playerState>& s) {
            return (s->has_player) && (!s->has_ball);
        };

        auto isObstacle = [](const std::shared_ptr<const playerState>& s) {
            return (!s->has_player) && (!s->has_ball) && (s->has_obstacle);
        };

        auto isShortPassFromDirection = [](const std::shared_ptr<const playerState>& s, Direction d) {
            return (s->action == Action::SHORT_PASS) && (s->direction == d);
        };

        auto isLongPassFromDirection = [](const std::shared_ptr<const playerState>& s, Direction d) {
            return (s->action == Action::LONG_PASS) && (s->direction == d);
        };

        auto isDribbleFromDirection = [](const std::shared_ptr<const playerState>& s, Direction d) {
            return (s->action == Action::DRIBBLE) && (s->direction == d);
        };

        auto isMoveFromDirection = [](const std::shared_ptr<const playerState>& s, Direction d) {
            return (s->action == Action::MOVE) && (s->direction == d);
        };

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

            if (relativePos == NORTH) { 
                // check north cell emptiness (to dribble/move forward)
                flags.north_empty = isEmpty(nState);

                // check for direct north teammate
                flags.north_teammate = isTeammate(nState);
                // if my direct north neighbor has an action to long pass to south (me) then i should receive ball
                flags.long_pass_from_north = isLongPassFromDirection(nState, Direction::SOUTH);
                
                // record player action
                flags.dribble_from_north = isDribbleFromDirection(nState, Direction::SOUTH); // North cell (i-1, j) wants to dribble backward
                flags.move_from_north = isMoveFromDirection(nState, Direction::SOUTH);       // North cell (i-1, j) wants to move backward
                // get source player metrics for inheritance by new cell
                if (flags.dribble_from_north || flags.move_from_north) {
                    source_mental = nState->mental;   
                    source_fatigue = nState->fatigue;
                    source_initial_row = nState->initial_row;
                    source_zone_type = nState->zone_type;
                }
            }
            else if (relativePos == WEST) {
                // check north cell emptiness (to dribble/move left)
                flags.west_empty = isEmpty(nState);

                // check for teammates in same line (left)
                flags.west_teammate = isTeammate(nState);
                // if my direct west neighbor has an action to short pass to east (me) then i should receive ball
                flags.short_pass_from_west = isShortPassFromDirection(nState, Direction::EAST); // Neighbor cell performs a short pass

                // record player action
                flags.dribble_from_west = isDribbleFromDirection(nState, Direction::EAST); // West cell (i, j-1) wants to dribble right
                flags.move_from_west = isMoveFromDirection(nState, Direction::EAST);       // West cell (i, j-1) wants to move right
                // get source player metrics for inheritance by new cell
                if (flags.dribble_from_west || flags.move_from_west) {
                    source_mental = nState->mental;   
                    source_fatigue = nState->fatigue;
                    source_initial_row = nState->initial_row;
                    source_zone_type = nState->zone_type;
                }
            }
            else if (relativePos == EAST) {
                // check north cell emptiness (to dribble/move left)
                flags.east_empty = isEmpty(nState);

                // check for teammates in same line (right)
                flags.east_teammate = isTeammate(nState); 
                // if my direct east neighbor has an action to short pass to west (me) then i should receive ball
                flags.short_pass_from_east = isShortPassFromDirection(nState, Direction::WEST); // Neighbor cell performs a short pass

                flags.dribble_from_east = isDribbleFromDirection(nState, Direction::WEST); // East cell (i, j+1) wants to dribble left
                flags.move_from_east = isMoveFromDirection(nState, Direction::WEST);       // East cell (i, j+1) wants to move left
                // get source player metrics for inheritance by new cell
                if (flags.dribble_from_east || flags.move_from_east) {
                    source_mental = nState->mental;   
                    source_fatigue = nState->fatigue;
                    source_initial_row = nState->initial_row;
                    source_zone_type = nState->zone_type;
                }
            }
            else if (relativePos == SOUTH) {
                // check north cell emptiness (to dribble/move backward)
                flags.south_empty = isEmpty(nState);
                // check for direct south teammate
                flags.south_teammate = isTeammate(nState);
                // if my direct south neighbor has an action to long pass to north (me) then i should receive ball
                flags.long_pass_from_south = isLongPassFromDirection(nState, Direction::NORTH);

                // record player action
                flags.dribble_from_south = isDribbleFromDirection(nState, Direction::NORTH);      // South cell (i+1, j) wants to dribble forward
                flags.move_from_south = isMoveFromDirection(nState, Direction::NORTH);            // South cell (i+1, j) wants to move forward
                // get source player metrics for inheritance by new cell
                if (flags.dribble_from_south || flags.move_from_south) {
                    source_mental = nState->mental;   
                    source_fatigue = nState->fatigue;
                    source_initial_row = nState->initial_row;
                    source_zone_type = nState->zone_type;  
                }
            }
            else if (relativePos == NORTH_EXTENDED) {
                // check for extended north teammate
                flags.north_extended_teammate = isTeammate(nState);
                // if my extended north neighbor has an action to long pass to south (me) then i should receive ball
                flags.extended_long_pass_from_north = isLongPassFromDirection(nState, Direction::SOUTH);    // Neighbor cell performs long pass
            }
            else if (relativePos == SOUTH_EXTENDED) {
                // check for extended south teammate
                flags.south_extended_teammate = isTeammate(nState);
                // if my extended south neighbor has an action to long pass to north (me) then i should receive ball
                flags.extended_long_pass_from_south = isLongPassFromDirection(nState, Direction::NORTH);    // Neighbor cell performs long pass
            }

            // East or West neighbor performs a dribble (to allow for move action)
            if (relativePos == EAST || relativePos == WEST) {
                flags.north_dribble = isDribbleFromDirection(nState, Direction::NORTH);       // Neighbor cell (west or east - same line) performs a dribble north
                flags.south_dribble = isDribbleFromDirection(nState, Direction::SOUTH);       // Neighbor cell (west or east - same line) performs a dribble south
            }

            // North neighbor has an obstacle (can intercept long pass)
            if (relativePos == NORTH) {
                flags.obstacle_interception_north = isObstacle(nState);
            }
            // South neighbor has an obstacle (can intercept long pass)
            else if (relativePos == SOUTH) {
                flags.obstacle_interception_south = isObstacle(nState);
            }

            // North or South or East or West Neighbor is an obstacle
            if (relativePos == NORTH || relativePos == SOUTH || relativePos == EAST || relativePos == WEST) {
                // if any direct neighbor has an obstacle, we toggle state flag to broadcast that we are near an obstacle
                if (nState->has_obstacle) {
                    state.near_obstacle = true;
                }
            }

            // Record if neighbors are near obstacles (for off-ball movement)
            if (relativePos == NORTH && nState->near_obstacle) { 
                flags.near_north_obstacle = true;
            }
            else if (relativePos == WEST && nState->near_obstacle) {
                flags.near_west_obstacle = true;
            }
            else if (relativePos == EAST && nState->near_obstacle) {
                flags.near_east_obstacle = true;
            }
            else if (relativePos == SOUTH && nState->near_obstacle) {
                flags.near_south_obstacle = true;
            }
        }

        //////////////////////////////////////////////////////////////
        // Helper functions (for local computation rules)
        //////////////////////////////////////////////////////////////
        auto applyHoldActionPlusCost = [&state]() {      // use lambda to capture reference to playerState
            state.action = Action::HOLD;
            state.direction = Direction::NONE;

            state.fatigue += 2.0;
            state.mental -= 1.0;
        };

        auto applyShortPassActionPlusCost = [&state](Direction direction) {    // take in parameter and capture reference to playerState
            state.action = Action::SHORT_PASS;
            state.direction = direction;
            // ball transfered to target cell after delay
            state.has_ball = false;
            // action cost -> mental/fatigue fluctuations
            state.fatigue += 3.0;
            state.mental -= 1.5;
        };

        auto applyLongPassActionPlusCost = [&state](Direction direction) {    // take in parameter and capture reference to playerState
            state.action = Action::LONG_PASS;
            state.direction = direction;
            // ball transfered to target extended cell after delay
            state.has_ball = false;
            // action cost -> mental/fatigue fluctuations
            state.fatigue += 4.0;
            state.mental -= 2.5;
        };

        auto applyDribbleAction = [&state](Direction direction) {    // take in parameter and capture reference to playerState
            state.action = Action::DRIBBLE;
            state.direction = direction;
            // ball and player transfered to target cell after delay
            state.has_player = false;
            state.has_ball = false;
        };

        auto applyMoveAction = [&state](Direction direction) {    // take in parameter and capture reference to playerState
            state.action = Action::MOVE;
            state.direction = direction;
            // player transfered to target cell after delay
            state.has_player = false;
        };

        auto applyBecomePlayerFromDribblePlusCost = [&state, source_mental, source_fatigue, source_initial_row, source_zone_type]() { // use lambda to capture reference to playerState and source attributes
            state.has_player = true;
            state.has_ball = true;
            // Inherit player attributes from source and apply action cost (mental/fatigue fluctuations)
            state.mental = source_mental - 3.0;
            state.fatigue = source_fatigue + 7.0;
            state.initial_row = source_initial_row;
            state.zone_type = source_zone_type;
        };

        auto applyBecomePlayerFromMovePlusCost = [&state, source_mental, source_fatigue, source_initial_row, source_zone_type]() { // use lambda to capture reference to playerState
            state.has_player = true;
            // inherit player attributes from source and apply action cost (mental/fatigue fluctuations)
            state.mental = source_mental - 2.0;
            state.fatigue = source_fatigue + 5.0;
            state.initial_row = source_initial_row;
            state.zone_type = source_zone_type;
        };

        auto applyGetBallFromShortPassPlusCost = [&state]() { // use lambda to capture reference to playerState
            state.has_ball = true;
            // action cost -> mental/fatigue fluctuations
            state.mental -= 1.0;
            state.fatigue += 2.5;
        };

        auto applyGetBallFromLongPassPlusCost = [&state]() { // use lambda to capture reference to playerState
            state.has_ball = true;
            // action cost -> mental/fatigue fluctuations
            state.mental -= 2.0;
            state.fatigue += 3.5;
        };

        auto applyMentalFatigueRecovery = [&state]() { // use lambda to capture reference to playerState
            state.action = Action::NONE;               // Reset action
            state.direction = Direction::NONE;         // Reset direction
            state.mental += 1.0;
            state.fatigue -= 1.0;
        };

        auto resetAll = [&state]() {                // use lambda to capture reference to playerState
            state.mental = 50.0;
            state.fatigue = 0.0;
            state.action = Action::NONE;
            state.direction = Direction::NONE;
            state.inactive_time = 0;
        };

        auto resetActionAndDirection = [&state]() { // use lambda to capture reference to playerState
            state.action = Action::NONE;            // Reset action
            state.direction = Direction::NONE;      // Reset direction
        };

        //////////////////////////////////////////////////////////////
        // Perform Actions (Cell logic when having ball)
        //////////////////////////////////////////////////////////////
        if (state.has_player && state.has_ball) {
            // Rule 1: Short pass to west or east teammate not near an obstacle
            if (state.fatigue > 30.0 && state.fatigue < 70.0 && state.mental < 60.0) {
                if (flags.east_teammate && !flags.near_east_obstacle) {
                    applyShortPassActionPlusCost(Direction::EAST);
                } 
                else if (flags.west_teammate && !flags.near_west_obstacle) {
                    applyShortPassActionPlusCost(Direction::WEST);
                }
                else {
                    // Rule 4: Can't perform action -> hold the ball
                    applyHoldActionPlusCost();
                }
            }
            // Rule 2: Long pass to north or south teammate (includes extended teammate)
            else if (state.fatigue > 25.0 && state.mental > 60.0 && state.mental < 70.0) {
                if ((flags.north_extended_teammate || flags.north_teammate) && !flags.obstacle_interception_north) {
                    applyLongPassActionPlusCost(Direction::NORTH);
                }
                else if ((flags.south_extended_teammate || flags.south_teammate) && !flags.obstacle_interception_south) {
                    applyLongPassActionPlusCost(Direction::SOUTH);
                }
                else {
                    // Rule 4: Can't perform action -> hold the ball
                    applyHoldActionPlusCost();
                }
            }
            // Rule 3: Dribble north/south if possible
            else if (state.fatigue < 50.0 && state.mental >= 70.0) {
                if (flags.north_empty) {
                    applyDribbleAction(Direction::NORTH);
                }
                else if (flags.south_empty) {
                    applyDribbleAction(Direction::SOUTH);
                }
                else {
                    // Rule 4: Can't perform action
                    applyHoldActionPlusCost();
                }
            }
            // Rule 4: Hold the ball
            else {
                // action cost -> mental/fatigue fluctuations
                applyHoldActionPlusCost();
            }

        }
        else if (state.has_player && !state.has_ball) {
            // Rule 5: Off-ball movement
            bool moved = false;

            /*
            Defenders: Off-ball movement is specific to tracking back. 
                - Any defender who is out of position should attempt to return to their defensive line (initial_row)
                - If by any chance original location is blocked by an obstacle, defender will drop back to original 
                - Defenders will not be moving when detecting their neighbor dribbled

                Goal: Hold defensive line 
            */
            if (!moved && state.zone_type == ZoneType::DEFENSE) {
                bool isDisplaced = currentId[0] != state.initial_row;

                if (isDisplaced) {
                    // Defender is above initial row => should move south
                    if (flags.south_empty && currentId[0] < state.initial_row) {
                        applyMoveAction(Direction::SOUTH);
                        moved = true; 
                    } 
                    // Defender is below initial row => should move north
                    else if (flags.north_empty && currentId[0] > state.initial_row) {
                        applyMoveAction(Direction::NORTH); 
                        moved = true;
                    }
                    // Try moving left since north/south not possible
                    else if (flags.west_empty) {
                        applyMoveAction(Direction::WEST);
                        moved = true; 
                    } 
                    // Try moving right since north/south not possible
                    else if (flags.east_empty) {
                        applyMoveAction(Direction::EAST);
                        moved = true; 
                    } 
                    // Try again to move toward original row
                    else if (flags.south_empty && currentId[0] < state.initial_row) {
                        applyMoveAction(Direction::SOUTH); 
                        moved = true;
                    } 
                    else if (flags.north_empty && currentId[0] > state.initial_row) {
                        applyMoveAction(Direction::NORTH); 
                        moved = true;
                    }
                    else {
                        resetActionAndDirection(); // No valid repositioning
                    }
                }
            }
            /*
            Midfielders: Off-ball movement is specific to being open for a pass. 
                - If detecting nearby neighbor dribbles, will move up/down the pitch too
                - If midfielder didn't move but sees that he is near obstacle, will try to reposition himself
                to an open cell (without obstacles)
            */
            else if (!moved && state.zone_type == ZoneType::MIDFIELD) {
                // Move north/south if possible when neighbor dribbles (Follow neighbor movement)
                if (state.fatigue < 40.0 && state.mental > 50.0) {
                    if (flags.north_empty && flags.north_dribble) {
                        applyMoveAction(Direction::NORTH);
                        moved = true;
                    }
                    else if (flags.south_empty && flags.south_dribble) {
                        applyMoveAction(Direction::SOUTH);
                        moved = true;
                    }
                }

                // Unable to move -> try reposititioning if near obstacle
                if (!moved && state.near_obstacle) {
                    // try moving left/right first to open space
                    if (flags.west_empty && !flags.near_west_obstacle) {
                        applyMoveAction(Direction::WEST);
                        moved = true; 
                    }
                    else if (flags.east_empty && !flags.near_east_obstacle) {
                        applyMoveAction(Direction::EAST);
                        moved = true; 
                    }
                    // not successful, try moving up/down
                    else if (flags.north_empty && !flags.near_north_obstacle) {
                        applyMoveAction(Direction::NORTH);
                        moved = true; 
                    }
                    else if (flags.south_empty && !flags.near_south_obstacle) {
                        applyMoveAction(Direction::SOUTH);
                        moved = true;
                    }
                }
            }
            /*
            Attackers: Off-ball movement is tied to remaining forward and in open positions in attacking positions
                - If they by any chance drifted backwards, track back to initial row (similar to defender repositioning)
                - If there is an obstacle in front or cell in front is near obstacle, try to break away wide (move left-right)
            */
            else if (!moved && state.zone_type == ZoneType::ATTACK) {
                // Attacker below his initial row => should move north
                if (flags.north_empty && currentId[0] > state.initial_row) {
                    applyMoveAction(Direction::NORTH);
                    moved = true; 
                }
                // near an obstacle => try moving wide for a better attacking positioning
                else if (flags.near_north_obstacle || flags.obstacle_interception_north) {
                    // try move left or right first
                    if (flags.west_empty) {
                        applyMoveAction(Direction::WEST);
                        moved = true; 
                    }
                    else if (flags.east_empty) {
                        applyMoveAction(Direction::EAST);
                        moved = true; 
                    }
                    // not possible -> drop back and try from there
                    else if (flags.south_empty) {
                        applyMoveAction(Direction::SOUTH); 
                        moved = true;
                    } 
                }
            }

            // Rule 6: Player Recovers Mental/Fatigue if he performs no actions
            if (!moved) {
                applyMentalFatigueRecovery();
            }
        }

        //////////////////////////////////////////////////////////////
        // Receive Actions (Delayed Cell Neighbor Inputs)
        //////////////////////////////////////////////////////////////
        if (!state.has_player && !state.has_ball && !state.has_obstacle) {  // empty cell (no player, ball, or obstacle)
            // Case 1: Become a player w/ ball if south/north/west/east neighbor wants to dribble to your location
            if (flags.dribble_from_south || flags.dribble_from_north || flags.dribble_from_west || flags.dribble_from_east) {
                applyBecomePlayerFromDribblePlusCost();
            }
            // Case 2: Become a player w/o ball if west/east neighbor dribbles (move north/south with player dribbling) or for off-ball movement
            else if (flags.move_from_south || flags.move_from_north || flags.move_from_west || flags.move_from_east) {
                applyBecomePlayerFromMovePlusCost();
            }
        }
        else if (state.has_player && !state.has_ball) {     // player cell
            // Case 3: Become a player w/ ball when neighbor performs a short pass
            if (flags.short_pass_from_west || flags.short_pass_from_east) {
                applyGetBallFromShortPassPlusCost();
            }
            // Case 4: Become a player w/ ball when neighbor performs long pass
            else if (flags.long_pass_from_north || flags.long_pass_from_south) {
                applyGetBallFromLongPassPlusCost();
            }
            else if (flags.extended_long_pass_from_north || flags.extended_long_pass_from_south) {
                applyGetBallFromLongPassPlusCost();
            }
        }

        //////////////////////////////////////////////////////////////
        // Cleanup of cells that moved (action = dribble or move)
        //////////////////////////////////////////////////////////////
        if (!state.has_player && !state.has_ball && !state.has_obstacle) {
            // Only track cells that had actions
            if (state.action != Action::NONE) {
                state.inactive_time += 1;
    
                // Reset after 1 inactive timestep with action
                if (state.inactive_time >= 2) {
                    resetAll();
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