#include "graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

/////////////////////////////////////////////////////////////////////////////////////////
//                ___    ___         ___        ________     _______                   //
//               |   \  |   \       |   \      |_____  /    |  _____|                  //
//               | |\ \ | |\ \      | |\ \          / /     | |_____                   //
//               | | \ \| | \ \     | |_\ \        / /      |  _____|                  //
//               | |  \ | |  \ \    |  ___ \      / /___    |  _____                   //
//               |_|   \__|   \ \   |_|   \_\    /______|   |_______|                  //
//                                                                                     //
/////////////////////////////////////////////////////////////////////////////////////////
// maze generation configuration edit theese values to change the settings of the maze //
/////////////////////////////////////////////////////////////////////////////////////////

#define CELL_X 20
#define CELL_Y 18
#define CELL_SIZE 24 // size of one tile in pixels

bool drawHeatMap = false; // toggle this to draw the heatmap of the player's memory map
bool shortcuts = true; // toggle this to add shortcuts to the maze
bool rooms = true; // toggle this to add rooms to the maze
bool randomStart = true; // toggle this to randomize the starting position of the player
bool randomEnd = true; // toggle this to randomize the exit position of the player

int playSpeed = 1;

// maze variables

#define MAZE_X (CELL_X * 2 + 1)
#define MAZE_Y (CELL_Y * 2 + 1)

bool maze[MAZE_X][MAZE_Y];
bool visited[CELL_X][CELL_Y];



// player variables

int startPos[2] = {1, 0}; // starting position of the player
int endPos[2] = {MAZE_X - 2, MAZE_Y - 1}; // exit position of the player
int playerPos[2] = {1, 0}; // starting position of the player
int playerMemoryMap[MAZE_X][MAZE_Y] = {0}; // weightmap - higher number means more visited, -1 means wall, 0 means unvisited path
bool markDeadEnds = false; // reaches to the first branch of the maze, and then it will stop to mark the dead ends in the hall



// color calculation with the weight

void getWeightColor(int x, int *r, int *g, int *b)
{
    if (x < 1) x = 1;
    if (x > 10) x = 10;

    if (x <= 5)
    {
        *r = 255 * (x - 1) / 4;
        *g = 255;
        *b = 0;
    }
    else
    {
        *r = 255;
        *g = 255 - 255 * (x - 5) / 5;
        *b = 0;
    }
}

void drawMaze()
{
    for (int i = 0; i < MAZE_Y; i++)
    {
        for (int j = 0; j < MAZE_X; j++)
        {
            int r, g, b;

            // set the color based on whether it's a wall or a path or the player

            if (maze[j][i]) setRGBColour(186, 130, 255);
            else setRGBColour(28, 0, 61);

            ///*
            if (playerMemoryMap[j][i] > 0 && drawHeatMap)
            {
                getWeightColor(playerMemoryMap[j][i], &r, &g, &b);
                setRGBColour(r, g, b); // color based on the weight of the path
            }
             
            if (playerMemoryMap[j][i] == -1 && drawHeatMap) setRGBColour(90, 20, 180); // purple for wall
            //*/

            if (j == startPos[0] && i == startPos[1]) setRGBColour(255, 255, 255); // starting position of the player
            if (j == endPos[0] && i == endPos[1]) setRGBColour(153, 59, 255); // exit position of the player
            if (j == playerPos[0] && i == playerPos[1]) setRGBColour(48, 214, 255);


            fillRect(
                j * CELL_SIZE,
                i * CELL_SIZE,
                CELL_SIZE,
                CELL_SIZE
            );
        }
    }
}


void movePlayer(int x, int y) // x and y are the coordinates of the cell that the player is in, and then we'll recursively use this function
{   
    int px = x;
    int py = y;
    playerPos[0] = px;
    playerPos[1] = py;

    playerMemoryMap[px][py]++; // mark as visited path

    if (px == endPos[0] && py == endPos[1]) // if the player has reached the exit
    {
        drawMaze();
        printf("You have reached the exit!\n");
        return;
    }

    int nextStep = 0; // 1 = down, 2 = right, 3 = up, 4 = left (minus numbers marks the visited paths, and 0 means no path found yet)
    int neighbourCount = 0; // count the number of neighbouring cells that are paths and not visited yet
    
    for (int i = 1; i < 5; i++) // touch all 4 neighbouring cells and mark them in memory map
    {
        int nx = px;
        int ny = py;

        switch (i)
        {
        case 1: ny++; break; // down
        case 2: nx++; break; // right
        case 3: ny--; break; // up
        case 4: nx--; break; // left
        }

        if (nx < 0 || nx >= MAZE_X || ny < 0 || ny >= MAZE_Y) continue;

        if (maze[nx][ny]) // if the neighbouring cell is a path and not a wall
        {
            neighbourCount++;
            if (playerMemoryMap[nx][ny] == 0) playerMemoryMap[nx][ny] = 1; // update memory map
        }
        else if (playerMemoryMap[nx][ny] == 0) playerMemoryMap[nx][ny] = -1; // update


        
    }
    int minWeight = 1000; // set a high value for the minimum weight
    int directions[4] = {1, 2, 3, 4};
    for (int i = 3; i > 0; i--)
    {
        int j = rand() % (i + 1);
        int temp = directions[i];
        directions[i] = directions[j];
        directions[j] = temp;
    }

    for (int i = 0; i < 4; i++) // decide the next move based on the neighbouring cells
    {
        int nx = px;
        int ny = py;

        switch (directions[i])
        {
        case 1: ny++; break; // down
        case 2: nx++; break; // right
        case 3: ny--; break; // up
        case 4: nx--; break; // left
        }

        if (nx < 0 || nx >= MAZE_X || ny < 0 || ny >= MAZE_Y) continue;

        // if you change the < to <= then it turn into a algorithm that walks enormous times in the maze to make its ticket worth ?????///////////////////////////////////////////////////////
        if (playerMemoryMap[nx][ny] < minWeight && playerMemoryMap[nx][ny] != -1) // if no unvisited path is found, move to the first visited path found
        {
            minWeight = playerMemoryMap[nx][ny];
            nextStep = directions[i];
        }
    }

    ///* // this is the part to prevent the player from checking a dead end twice and incrementing the weight of that path,
    if (neighbourCount == 1)
    {
        playerMemoryMap[px][py]++;
        markDeadEnds = true; 
    }
    else if (neighbourCount == 2 && markDeadEnds)
    {
        playerMemoryMap[px][py]++;
    }
    else if (neighbourCount > 2)
    {
        markDeadEnds = false;
    }

    if (markDeadEnds)
    {
        playerMemoryMap[px][py]++;
    }
    ///*

    drawMaze(); 
    sleep(playSpeed); 

    switch (nextStep)
    {
    case 1: 
        movePlayer(px, py + 1);
        break;
    case 2: 
        movePlayer(px + 1, py);
        break;
    case 3: 
        movePlayer(px, py - 1);
        break;
    case 4: 
        movePlayer(px - 1, py);
        break;
    default:
        printf("No path found!\n");
        return;
    
    }
}



void generateMaze(int x, int y)
{
    int cx = x;
    int cy = y;
    int order[4] = {0, 1, 2, 3}; // 0 = up, 1 = down, 2 = left, 3 = right

    if (cx < 0 || cx >= CELL_X || cy < 0 || cy >= CELL_Y)
    {
        return;
    }

    // set the current cell as visited and open it in the maze
    visited[cx][cy] = true;
    maze[cx * 2 + 1][cy * 2 + 1] = true;


        // randomise directions
        for (int i = 0; i < 4; i++)
        {
            int r = rand() % 4;
            int temp = order[i];
            order[i] = order[(i+r) % 4];
            order[(i+r) % 4] = temp;
        }


    // carve a path in the maze
    for (int i = 0; i < 4; i++)
    {
        switch (order[i])
        {
        case 0: // up
            if (cy > 0 && !visited[cx][cy - 1])
            {
                maze[cx * 2 + 1][cy * 2] = true;
                generateMaze(cx, cy - 1);
            }
            break;
        case 1: // down
            if (cy < CELL_Y - 1 && !visited[cx][cy + 1])
            {
                maze[cx * 2 + 1][cy * 2 + 2] = true;
                generateMaze(cx, cy + 1);
            }
            break;
        case 2: // left
            if (cx > 0 && !visited[cx - 1][cy])
            {
                maze[cx * 2][cy * 2 + 1] = true;
                generateMaze(cx - 1, cy);
            }
            break;
        case 3: // right
            if (cx < CELL_X - 1 && !visited[cx + 1][cy])
            {
                maze[cx * 2 + 2][cy * 2 + 1] = true;
                generateMaze(cx + 1, cy);
            }
            break;
        }
    }
}

void randomizeStart()
{
    int startX = rand() % CELL_X;
    int startY = rand() % CELL_Y;

    startPos[0] = startX * 2 + 1;
    startPos[1] = startY * 2 + 1;

    playerPos[0] = startPos[0];
    playerPos[1] = startPos[1];

    maze[startPos[0]][startPos[1]] = true; // ensure the starting position is a path
}

void randomizeEnd()
{
    int endX = rand() % CELL_X;
    int endY = rand() % CELL_Y;

    endPos[0] = endX * 2 + 1;
    endPos[1] = endY * 2 + 1;

    maze[endPos[0]][endPos[1]] = true; // ensure the exit position is a path
}

void addShortcuts()
{
    for (int i = 0; i < CELL_X; i++)
    {
        for (int j = 0; j < CELL_Y; j++)
        {
            if (rand() % 100 < 20) // 20% chance to add a shortcut
            {
                int direction = rand() % 4;
                switch (direction)
                {
                case 0: // up
                    if (j > 0 && !maze[i * 2 + 1][j * 2])
                    {
                        maze[i * 2 + 1][j * 2] = true;
                    }
                    break;
                case 1: // down
                    if (j < CELL_Y - 1 && !maze[i * 2 + 1][j * 2 + 2])
                    {
                        maze[i * 2 + 1][j * 2 + 2] = true;
                    }
                    break;
                case 2: // left
                    if (i > 0 && !maze[i * 2][j * 2 + 1])
                    {
                        maze[i * 2][j * 2 + 1] = true;
                    }
                    break;
                case 3: // right
                    if (i < CELL_X - 1 && !maze[i * 2 + 2][j * 2 + 1])
                    {
                        maze[i * 2 + 2][j * 2 + 1] = true;
                    }
                    break;
                }
            }
        }
    }
}

void addRooms()
{
    for (int i = 0; i < CELL_X; i++)
    {
        for (int j = 0; j < CELL_Y; j++)
        {
            if (rand() % 100 < 5) // 5% chance to add a room
            {
                if (i == 0 || j == 0 || i == CELL_X - 1 || j == CELL_Y - 1) continue; // don't add rooms on the sides
                for (int x = 0; x < 3; x++) {
                    for (int y = 0; y < 3; y++) {
                        maze[2 * i + x][2 * j + y] = true; // flip the walls to paths                 
                    }
                }
                maze[2 * i + 1][2 * j + 1] = !maze[2 * i + 1][2 * j + 1]; // flip the center cell's state
                
            }
        }
    }
}



void main()
{
    srand(time(NULL));
    setWindowSize(MAZE_X * CELL_SIZE, MAZE_Y * CELL_SIZE);

    // Initialise maze as walls
    for (int i = 0; i < MAZE_X; i++)
    {
        for (int j = 0; j < MAZE_Y; j++)
        {
            maze[i][j] = false;
        }
    }

    // Initialise visited array
    for (int i = 0; i < CELL_X; i++)
    {
        for (int j = 0; j < CELL_Y; j++)
        {
            visited[i][j] = false;
        }
    }

    // Generate maze starting at top-left
    generateMaze(0, 0);

    // Add funny shortcuts to the maze to make my life harder
    if (shortcuts) addShortcuts();

    // Add rooms to the maze to make my life even harder
    if (rooms) addRooms();

    if (randomStart) randomizeStart();
    if (randomEnd) randomizeEnd();

    // Create entrance and exit
    maze[startPos[0]][startPos[1]] = true;
    maze[endPos[0]][endPos[1]] = true;

    //place the player
    playerPos[0] = startPos[0];
    playerPos[1] = startPos[1];

    //launch the player
    movePlayer(playerPos[0], playerPos[1]);

    // Draw everything
    drawMaze();


    //player();
}