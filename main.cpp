//
//  main.cpp
//  mazes
//
//  Created by Galina on 10/12/2019.
//  Copyright © 2019 Galina and Nathan. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>

struct MazeCell {
    bool isStart;
    bool isEnd;
    bool up;
    bool down;
    bool left;
    bool right;
    int reward;

    // Location in matrix.
    int x;
    int y;
};

struct CellValue {
    // Indexing: 0 = up, 1 = down, left = 2, right = 3.
    double actions[4];
};

struct Agent {
    int strat;
    
};

void sarsa(std::vector<std::vector<MazeCell>> maze, int episodes);
int chooseAction(MazeCell cell, CellValue values, int episode);
int findOptimalAction(double values[], int length);
std::vector<std::string> split(std::string strToSplit, char delimiter);
std::vector<std::vector<MazeCell>> initialize_maze();
void print_maze(int size, std::vector<std::vector<MazeCell>> maze);
void print_optimal_actions(std::vector<std::vector<CellValue>> mazeValues);

double alpha = 0.1;
double ygamma = 0.7;
double greedyEpsilon = 0.4;

int main(int argc, const char * argv[]) {
    std::vector<std::vector<MazeCell>> maze;
    maze = initialize_maze();
    
    print_maze(maze.size(), maze);
    
    sarsa(maze, 10000);
    
    return 0;
}

void sarsa(std::vector<std::vector<MazeCell>> maze, int episodes) {
    MazeCell startCell, endCell, currentCell;

    // Random generator
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0,1.0);

    // Create a matrix containing random state-action values.
    std::vector<std::vector<CellValue>> mazeValues(maze.size(), 
            std::vector<CellValue>(maze.size()));
    for (int i = 0; i < maze.size(); i++) {
        for (int j = 0; j < maze.size(); j++) {
            // Give every action a random value.
            if (maze[i][j].up) {
                mazeValues[i][j].actions[0] = distribution(generator);
            }
            if (maze[i][j].down) {
                mazeValues[i][j].actions[1] = distribution(generator);
            }
            if (maze[i][j].left) {
                mazeValues[i][j].actions[2] = distribution(generator);
            }
            if (maze[i][j].right) {
                mazeValues[i][j].actions[3] = distribution(generator);
            }

            // Store the coordinates of the terminal states.
            if (maze[i][j].isStart) {
                startCell = maze[i][j];
            } else if (maze[i][j].isEnd) {
                endCell = maze[i][j];
            }
        }
    }

    std::cout << "\nInitial (random) optimal actions:\n";
    print_optimal_actions(mazeValues);
    std::cout << '\n';

    // Keep track of best and worst performance.
    int maxSteps = 0;
    int minSteps = 276447231; // Highest possible int without overflow.

    for (int i = 0; i < episodes; i++) {
        int step = 0;

        std::cout << "Greedy epsilon: " << greedyEpsilon * (1 / exp((0.0001 * i))) << '\n';

        // Choose initial cell and action.
        currentCell = startCell;
        int actionIndex = chooseAction(currentCell, 
                          mazeValues[currentCell.x][currentCell.y],
                          i);
        while (currentCell.x != endCell.x || currentCell.y != endCell.y) {
            step++;
            MazeCell newCell;
            switch(actionIndex) {
                case 0:
                    // Up     
                    newCell = maze[currentCell.x - 1][currentCell.y];
                    break;
                case 1:
                    // Down
                    newCell = maze[currentCell.x + 1][currentCell.y];
                    break;
                case 2:
                    // Left
                    if (currentCell.y != 0) {
                        newCell = maze[currentCell.x][currentCell.y - 1];
                    }
                    break;
                case 3:
                    // Right
                    newCell = maze[currentCell.x][currentCell.y + 1];
                    break;
                default:
                    // This should never happen.
                    newCell = currentCell;
            }

            int newActionIndex = chooseAction(newCell, mazeValues[newCell.x][newCell.y], i);

            // Update the state-action value.
            mazeValues[currentCell.x][currentCell.y].actions[actionIndex] 
                    += alpha * (newCell.reward 
                    + (ygamma * mazeValues[newCell.x][newCell.y].actions[newActionIndex])
                    - mazeValues[currentCell.x][currentCell.y].actions[actionIndex]);

            currentCell = newCell;
            actionIndex = newActionIndex;
        }

        if (step > maxSteps) {
            maxSteps = step;
        } else if (step < minSteps) {
            minSteps = step;
        }

        // Report number of steps.
        std::cout << "Number of steps: " << step << '\n';
    }

    std::cout << '\n';
    print_optimal_actions(mazeValues);
    print_maze(maze.size(), maze);

    std::cout << "\nMax steps: " << maxSteps << '\n';
    std::cout << "Min steps: " << minSteps << '\n';
}

int chooseAction(MazeCell cell, CellValue values, int episode) {
    int optimalAction = findOptimalAction(values.actions, 4);

    int possibleActions = 0;

    if (cell.up) {
       possibleActions++;
    }

    if (cell.down) {
       possibleActions++;
    }

    if (cell.left) {
       possibleActions++;
    }

    if (cell.right) {
       possibleActions++;
    }

    double number = (double) rand() / (double) RAND_MAX;
    if (number <= 1 - (greedyEpsilon * (1 / exp(0.0001 * episode)))) {
        return optimalAction;
    } else {
        int action = floor(possibleActions * ((double) rand() / (double) RAND_MAX));
        switch(action) {
            case 0:
                if (cell.up) {
                    return 0;
                }
            case 1:
                if (cell.down) {
                    return 1;
                }
            case 2:
                if (cell.left) {
                    return 2;
                }
            case 3:
                return 3;
            default: return optimalAction;
        }

        std::cout << '\n';
    }
}

int findOptimalAction(double values[], int length) {
    int optimalAction = 0;
    int highestValue = 0;
    for (int i = 0; i < length; i++) {
        if (values[i] > highestValue) {
            highestValue = values[i];
            optimalAction = i;
        }
    }

    return optimalAction;
}

std::vector<std::vector<MazeCell>> initialize_maze() {
    // Get the size of the maze.
    std::ifstream inFile("maze-generator/maze_export");
    int count = std::count(std::istreambuf_iterator<char>(inFile),
               std::istreambuf_iterator<char>(), '\n');

    std::vector<std::vector<MazeCell>> maze(std::sqrt(count),
                                            std::vector<MazeCell>(std::sqrt(count)));

    std::ifstream in("maze-generator/maze_export");
    std::string str;

    // Skip the first line.
    std::getline(in, str);

    while(std::getline(in, str)) {
        if (str.size() > 0) {
            // Initialise maze.
            std::vector<std::string> splittedString = split(str, ',');

            // Create a MazeCell
            MazeCell cell;

            // Set a default reward.
            cell.reward = -0.01;
            if (std::stoi(splittedString[2]) == 1) {
                cell.up = true;
            } else {
                cell.up = false;
            }

            if (std::stoi(splittedString[3]) == 1) {
                cell.down = true;
            } else {
                cell.down = false;
            }

            if (std::stoi(splittedString[4]) == 1) {
                cell.left = true;
            } else {
                cell.left = false;
            }

            if (std::stoi(splittedString[5]) == 1) {
                cell.right = true;
            } else {
                cell.right = false;
            }

            if (std::stoi(splittedString[6]) == 1) {
                cell.isStart = true;
            } else {
                cell.isStart = false;
            }

            if (std::stoi(splittedString[7]) == 1) {
                cell.isEnd = true;

                // Set a finish reward.
                cell.reward = 1000;
            } else {
                cell.isEnd = false;
            }

            cell.x = std::stoi(splittedString[0]);
            cell.y = std::stoi(splittedString[1]);

            maze[cell.x][cell.y] = cell;
        }
    }

    return maze;
}

void print_maze(int size, std::vector<std::vector<MazeCell>> maze) {
    int iStart, jStart, iEnd, jEnd;

    std::cout << ".";
    for (int i = 0; i < size; i++) {
        std::cout << "_.";
    }
    std::cout << '\n';

    for (int i = 0; i < size; i++) {
        std::cout << "|";
        for (int j = 0; j < size; j++) {
            if (maze[i][j].down == 0) {
                std::cout << "_";
            } else {
                std::cout << " ";
            }

            if (maze[i][j].right == 0) {
                std::cout << "|";
            } else {
                std::cout << ".";
            }

            if (maze[i][j].isStart) {
                iStart = i;
                jStart = j;
            } else if (maze[i][j].isEnd) {
                iEnd = i;
                jEnd = j;
            }
        }
        std::cout << '\n';
    }

    std::cout << "\nStart coordinates: (" << iStart << ", " << jStart << ").\n";
    std::cout << "End coordinates:   (" << iEnd << ", " << jEnd << ").\n";
    return;
}

// Makes use of unicode, might not work in every terminal.
void print_optimal_actions(std::vector<std::vector<CellValue>> mazeValues) {
    for (int i = 0; i < mazeValues.size(); i++) {
        for (int j = 0; j < mazeValues.size(); j++) {
            int index = findOptimalAction(mazeValues[i][j].actions, 4);
            switch(index) {
                case 0:
                    std::cout << " ↑";
                    break;
                case 1:
                    std::cout << " ↓";
                    break;
                case 2:
                    std::cout << " ←";
                    break;
                case 3:
                    std::cout << " →";
                    break;
                default:
                    std::cout << " .";
            }
        }
        std::cout << '\n';
    }
}

std::vector<std::string> split(std::string strToSplit, char delimiter) {
    std::stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while (std::getline(ss, item, delimiter)) {
        splittedStrings.push_back(item);
    }
    return splittedStrings;
}

