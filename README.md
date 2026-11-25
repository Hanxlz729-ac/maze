             ___    ___         ___        ________     _______
            |   \  |   \       |   \      |_____  /    |  _____|
            | |\ \ | |\ \      | |\ \          / /     | |_____
            | | \ \| | \ \     | |_\ \        / /      |  _____|
            | |  \ | |  \ \    |  ___ \      / /___    |  _____
            |_|   \__|   \ \   |_|   \_\    /______|   |_______|


Maze Program
============

Description
-----------
This C program generates a randomized maze, adds shortcuts and rooms, and
automatically moves a player from the entrance to the exit. The maze is
displayed in a graphics window; the cyan square shows the player's position.

Requirements
------------
- GCC
- Java

Configuration
-------------
Edit the configuration values near the top of `maze.c`, then recompile the
program for the changes to take effect.

- `CELL_X` and `CELL_Y` set the maze width and height in cells.
- `CELL_SIZE` sets the size of each displayed tile in pixels. The window size
  is calculated from the maze dimensions and this value.
- `drawHeatMap` enables or disables the display of the player's memory map.
- `shortcuts` enables or disables extra passages (attempted with a 20% chance
  per cell).
- `rooms` enables or disables rooms (attempted with a 5% chance per cell).
- `randomStart` chooses a random starting cell when enabled. When disabled,
  the player starts at the entrance.
- `randomEnd` chooses a random exit cell when enabled. When disabled, the exit
  is at the default position on the bottom edge.
- `playSpeed` controls the delay between player moves; increase it to slow the
  animation down.

Compile and run
---------------
Run these commands from this directory:

    gcc -o maze maze.c graphics.c
    ./maze | java -jar drawapp-5.0.jar

The second command starts the DrawApp graphics runner and launches the
compiled maze program.
