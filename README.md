# Cell DEVS - Football Player Interaction Model (FPI)

## Introduction

This project implements a DEVS-based model for simulating football player interactions, including short passing, long passing, dribbling, and off-ball movement within a defined cell space. The model simulates the dynamic interaction between players and the ball, including state variables such as fatigue, mental state, and action decisions.

## Dependencies

This project assumes that you have **Cadmium** installed in a location accessible by the environment variable $CADMIUM.
_This dependency would be met by default if you are using the ARSLAB servers. To check, try `echo $CADMIUM` in the terminal_

## Build

To build this project, run:

```sh
source build_sim.sh
```

**NOTE**: Every time you run `build_sim.sh`, the contents of `build/` and `bin/` will be replaced.

## Running Simulations

### Full-Simulation (10x10 Grid)

To test the entire simulation on a 10x10 grid, run:

#### Without Obstacles

```sh
./bin/football_player_interaction config/without_obstacles/10x10_player_config.json
```

#### With Obstacles

```sh
./bin/football_player_interaction config/with_obstacles/10x10_player_config.json
```

#### With Obstacles and Zones

```sh
./bin/football_player_interaction config/with_obstacles/with_zones/10x10_player_config.json
```

#### With Obstacles, Zones and Roles

```sh
./bin/football_player_interaction config/with_obstacles/with_zones/with_roles/10x10_player_config.json
```

### Component Testing (3×3 Grid)

If you are interested in testing specific gameplay components (e.g. short pass, dribble, long pass, off-ball movement), use the 3×3 configuration files:

Files prefixed with `3x3_*` in the config/without_obstacles/ and config/with_obstacles/ folders target specific behaviors.

#### Example

- To test the short passing action on a 3x3 grid:

```sh
./bin/football_player_interaction config/without_obstacles/3x3_player_short_pass_config.json
```

## Output Files .csv

Any output generated during the simulation will be saved as a CSV file called `grid_log.csv`.

To view any output file visually, use the [Cell-DEVS Viewer](https://devssim.carleton.ca/cell-devs-viewer/). Simply upload the `grid_log.csv` file along with the corresponding configuration JSON file that was used to generate it.

## Video Files .webm

The recorded simulation videos demonstrate different scenarios using the Cell-DEVS Football Player Interaction Model. Each video corresponds to 10×10 grid gameplay under a specific configuration.

For better clarity, playback speed should be slowed down while viewing the videos.

### How to Interpret the Simulations

The Cell-DEVS Viewer displays a 4x3 grid, for the current implementation, where the most important cells to observe are:

- Row 1, Column 1: Represents the player (red when present).
- Row 1, Column 2: Represents the ball (blue when present).
- Row 1, Column 3: Represents the obstacle (green when present).
- Row 2, Column 1: Represents near-obstacle detection (cyan when present).
