# Cadmium-Cell-DEVS-Football-Player-Interaction

## Introduction
This project implements a Cell-DEVS-based model for simulating player interactions, including short passing, long passing, and dribbling within a defined cell space. The model simulates the dynamic interaction between players and the ball, including state variables such as fatigue, mental state, and action decisions.

## Dependencies
This project assumes that you have **Cadmium** installed in a location accessible by the environment variable $CADMIUM.
_This dependency would be met by default if you are using the ARSLAB servers. To check, try `echo $CADMIUM` in the terminal_

## Build
To build this project, run:
```sh
source build_sim.sh
```
__NOTE__: Every time you run `build_sim.sh`, the contents of `build/` and `bin/` will be replaced

## Execute
To run the project, run:
```sh
./bin/football_pitch config/10x10_player_config.json
```

## Testing Individual Actions
1. To test the short passing action on a 3x3 grid, run:
```sh
./bin/football_pitch config/3x3_player_short_pass_config.json
```
2. To test the dribbling action on a 3x3 grid, run:
```sh
./bin/football_pitch config/3x3_player_dribble_config.json
```
3. To test the long passing action on a 3x3 grid, run:
```sh
./bin/football_pitch config/3x3_player_long_pass_config.json
```

## Output Files
Any output generated during the simulation will be saved as a CSV file called grid_log.csv.

To view any output file visually, use the [Cell-DEVS Viewer](https://devssim.carleton.ca/cell-devs-viewer/). Simply upload the grid_log.csv file along with the corresponding configuration JSON file that was used to generate it.
