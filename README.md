# Cell DEVS - Football Player Interaction Model (FPI)

## Introduction

This project implements a DEVS-based model for simulating football player interactions, including short passing, long passing, and dribbling within a defined cell space. The model simulates the dynamic interaction between players and the ball, including state variables such as fatigue, mental state, and action decisions.

## Dependencies

This project assumes that you have **Cadmium** installed in a location accessible by the environment variable $CADMIUM.
_This dependency would be met by default if you are using the ARSLAB servers. To check, try `echo $CADMIUM` in the terminal_

## Build

To build this project, run:

```sh
source build_sim.sh
```

**NOTE**: Every time you run `build_sim.sh`, the contents of `build/` and `bin/` will be replaced.

## Testing Entire System

To test the entire simulation on a 10x10 grid, run:

```sh
./bin/football_player_interaction config/10x10_player_config.json
```

## Testing Actions

To test individual components of the system (such as substitution logic and player models), navigate to the `bin` directory and run:

- To test the short passing action on a 3x3 grid:

```sh
./bin/football_player_interaction config/3x3_player_short_pass_config.json
```

- To test the dribbling action on a 3x3 grid:

```sh
./bin/football_player_interaction config/3x3_player_dribble_config.json
```

- To test the long passing action on a 3x3 grid:

```sh
./bin/football_player_interaction config/3x3_player_long_pass_config.json
```

## Output Files .csv

Any output generated during the simulation will be saved as a CSV file called `grid_log.csv`.

To view any output file visually, use the [Cell-DEVS Viewer](https://devssim.carleton.ca/cell-devs-viewer/). Simply upload the `grid_log.csv` file along with the corresponding configuration JSON file that was used to generate it.

## Video Files .webm

The recorded simulation videos demonstrate different scenarios using the Cell-DEVS Football Player Interaction Model. Each video corresponds to a specific action: short pass, long pass, dribble, and full 10×10 grid gameplay.

### How to Interpret the Simulations

The Cell-DEVS Viewer displays a 3×3 grid, for the current implementation, where the most important cells to observe are:

- Top-left cell: Represents the player (red when present).
- Top-middle cell: Represents the ball (blue when present).

### Action-Specific Observations

- Short Pass: The ball moves left or right between positions.
- Long Pass: The ball moves up or down between positions.
- Dribble: The player and ball move together, with occasional movement from neighboring players in the same direction.
- 10×10 Simulation: Expands the same behaviors observed in the 3×3 grid to a larger field.

For better clarity, playback speed should be slowed down while viewing the videos.
