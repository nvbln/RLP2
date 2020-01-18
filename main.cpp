//
//  main.cpp
//  mazes
//
//  Created by Galina on 10/12/2019.
//  Copyright © 2019 Galina and Nathan. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>


using namespace std;

struct MazeCell {
    bool isStart;
    bool isEnd;
    bool up;
    bool down;
    bool left;
    bool right;
    double reward;

    // Location in matrix.
    int x;
    int y;
};

struct CellValue {
    // Indexing: 0 = up, 1 = down, left = 2, right = 3.
    double actions[4];
};

void sarsa(vector<vector<MazeCell> > maze, int episodes, double greedyEpsilon);
void qlearning(vector<vector<MazeCell> > maze, int episodes, double greedyEpsilon);
int chooseAction(MazeCell cell, CellValue values, int episode, double greedyEpsilon);
int findOptimalAction(double values[], MazeCell c, int length);
void reset_distractors(vector<vector<MazeCell> > &maze);
MazeCell index2NewCell(int actionIndex, vector<vector<MazeCell> > &maze, MazeCell currentCell);
vector<string> split(string strToSplit, char delimiter);
void random_action_init(vector<vector<CellValue> > &mazeValues, vector<vector<MazeCell> > maze, MazeCell &startCell, MazeCell &endCell);
vector<vector<MazeCell> > initialize_maze();
void print_maze(int size, vector<vector<MazeCell> > maze);
void print_optimal_actions(int size, vector<vector<MazeCell> > maze, vector<vector<CellValue> > mazeValues);

int seed = 0;

double alpha = 0.1;
double ygamma = 0.98;
double defaultReward = -0.98;
double finalReward = 100;

int main(int argc, const char * argv[]) {
    double greedyEpsilon = 0.4;
    
    vector<vector<MazeCell> > maze;
    
    maze = initialize_maze();
    
    print_maze(maze.size(), maze);
    
    cout << "Sarsa\n";
    sarsa(maze, 10000, greedyEpsilon);
    
    // RNG seed is the same for both algos
    
    maze = initialize_maze();
    cout << "Q-Learning\n";
    qlearning(maze, 10000, greedyEpsilon);
    
    return 0;
}

void sarsa(vector<vector<MazeCell> > maze, int episodes, double greedyEpsilon) {
    MazeCell startCell, endCell, currentCell;

    // Create a matrix containing random state-action values.
    vector<vector<CellValue> > mazeValues(maze.size(),
            vector<CellValue>(maze.size()));
    
    random_action_init(mazeValues, maze, startCell, endCell);

    // Keep track of best and worst performance.
    int maxSteps = 0;
    int minSteps = 276447231; // Highest possible int without overflow.
    int maxReward = 0;
    int maxRewardSteps = 0;
    double finalReward = 0;

    // Open file to write performance to.
    ofstream sarsaPerformance;
    sarsaPerformance.open("sarsa_performance.csv");
    sarsaPerformance << "episode, reward\n";

    for (int i = 0; i < episodes; i++) {
        double totalReward = 0;
        int step = 0;

        // Choose initial cell and action.
        currentCell = startCell;
        while (currentCell.x != endCell.x || currentCell.y != endCell.y) {
            step++;
            MazeCell newCell;
            
            int actionIndex = chooseAction(currentCell, mazeValues[currentCell.x][currentCell.y], i, greedyEpsilon);
            
            newCell = index2NewCell(actionIndex, maze, currentCell);

            int newActionIndex = chooseAction(newCell, mazeValues[newCell.x][newCell.y], i, greedyEpsilon);

            // Update the state-action value.
            mazeValues[currentCell.x][currentCell.y].actions[actionIndex] 
                    += alpha * (newCell.reward 
                    + (ygamma * mazeValues[newCell.x][newCell.y].actions[newActionIndex])
                    - mazeValues[currentCell.x][currentCell.y].actions[actionIndex]);

            currentCell = newCell;
            actionIndex = newActionIndex;

            totalReward += newCell.reward;

        }

        sarsaPerformance << i << ", " << totalReward << '\n';

        if (step > maxSteps) {
            maxSteps = step;
        } else if (step < minSteps) {
            minSteps = step;
        }

        if (totalReward >= maxReward) {
            maxReward = totalReward;
            if (maxRewardSteps > step || maxRewardSteps == 0) {
                maxRewardSteps = step;
            }
        }
        
        finalReward += totalReward;
        
        if (i + 1 == episodes) {
            // Report the total reward.
            cout << "Total reward: " << totalReward << '\n';

            // Report number of steps.
            cout << "Number of steps: " << step << '\n';
        }
    }

    sarsaPerformance.close();

    cout << '\n';
    print_optimal_actions(maze.size(), maze, mazeValues);
    print_maze(maze.size(), maze);

    cout << "\nMax steps: " << maxSteps << '\n';
    cout << "Min steps: " << minSteps << '\n';
    cout << "Max reward: " << maxReward << " and steps: " << maxRewardSteps << '\n';
    cout << "Final reward: " << finalReward << '\n';
}

void qlearning(vector<vector<MazeCell> > maze, int episodes, double greedyEpsilon) {
    MazeCell startCell, endCell, currentCell;

    // Create a matrix containing random state-action values.
    vector<vector<CellValue> > mazeValues(maze.size(), vector<CellValue>(maze.size()));
    
    random_action_init(mazeValues, maze, startCell, endCell);

    // Keep track of best and worst performance.
    int maxSteps = 0;
    int minSteps = 276447231; // Highest possible int without overflow.
    int size = maze.size();
    int maxReward = 0;
    int maxRewardSteps = 0;
    int finalReward = 0;

    // Open file to write performance to.
    ofstream qlearningPerformance;
    qlearningPerformance.open("qlearning_performance.csv");
    qlearningPerformance << "episode, reward\n";

    for (int i = 0; i < episodes; i++) {
        double totalReward = 0;
        int step = 0;
        
        // Choose initial cell and action.
        currentCell = startCell;

        while (currentCell.x != endCell.x || currentCell.y != endCell.y) {
            step++;
            MazeCell newCell;
            
            int actionIndex = chooseAction(currentCell, mazeValues[currentCell.x][currentCell.y], i, greedyEpsilon);
            
            newCell = index2NewCell(actionIndex, maze, currentCell);

            int newActionIndex = chooseAction(newCell, mazeValues[newCell.x][newCell.y], i, 0);

            // Update the state-action value.
            mazeValues[currentCell.x][currentCell.y].actions[actionIndex]
                    += alpha * (newCell.reward
                    + (ygamma * mazeValues[newCell.x][newCell.y].actions[newActionIndex])
                    - mazeValues[currentCell.x][currentCell.y].actions[actionIndex]);

            currentCell = newCell;

            totalReward += newCell.reward;
        }

        qlearningPerformance << i << ", " << totalReward << '\n';

        if (step > maxSteps) {
            maxSteps = step;
        } else if (step < minSteps) {
            minSteps = step;
        }

        if (totalReward >= maxReward) {
            maxReward = totalReward;
            if (maxRewardSteps > step || maxRewardSteps == 0) {
                maxRewardSteps = step;
            }
        }
        
        finalReward += totalReward;

        if (i + 1 == episodes) {
            // Report the total reward.
            cout << "Total reward: " << totalReward << '\n';

            // Report number of steps.
            cout << "Number of steps: " << step << '\n';
        }
    }

    qlearningPerformance.close();

    cout << '\n';
    print_optimal_actions(maze.size(), maze, mazeValues);
    print_maze(maze.size(), maze);

    cout << "\nMax steps: " << maxSteps << '\n';
    cout << "Min steps: " << minSteps << '\n';
    cout << "Max reward: " << maxReward << " and steps: " << maxRewardSteps << '\n';
    cout << "Final reward: " << finalReward << '\n';
}

int chooseAction(MazeCell cell, CellValue values, int episode, double greedyEpsilon) {
    int optimalAction = findOptimalAction(values.actions, cell, 4);
    
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
                } else if (cell.down) {
                    return 1;
                }  else if (cell.left) {
                   return 2;
                }  else if (cell.right) {
                   return 3;
                }
            case 1:
                if (cell.down) {
                    return 1;
                }  else if (cell.left) {
                   return 2;
                }  else if (cell.right) {
                   return 3;
                }
            case 2:
                if (cell.left) {
                    return 2;
                }  else if (cell.right) {
                   return 3;
                }
            case 3:
                return 3;
            default: return optimalAction;
        }

        cout << '\n';
    }
}

int findOptimalAction(double values[], MazeCell c, int length) {
    int optimalAction;
    int highestValue;
    if (c.up) {
        optimalAction = 0;
        highestValue = values[0];
    } else if (c.down) {
        optimalAction = 1;
        highestValue = values[1];
    } else if (c.left) {
        optimalAction = 2;
        highestValue = values[2];
    } else if (c.right) {
        optimalAction = 3;
        highestValue = values[3];
    }
    
    if (values[0] > highestValue && c.up) {
        highestValue = values[0];
        optimalAction = 0;
    }
    if (values[1] > highestValue && c.down) {
        highestValue = values[1];
        optimalAction = 1;
    }
    if (values[2] > highestValue && c.left) {
        highestValue = values[2];
        optimalAction = 2;
    }
    if (values[3] > highestValue && c.right) {
        highestValue = values[3];
        optimalAction = 3;
    }
   // for (int i = 0; i < length; i++) {
   //     if (values[i] > highestValue && c) {
   //         highestValue = values[i];
   //         optimalAction = i;
   //     }
   // }

    return optimalAction;
}

MazeCell index2NewCell(int actionIndex, vector<vector<MazeCell> > &maze, MazeCell currentCell) {
    switch(actionIndex) {
        case 0:
            // Up
            return maze[currentCell.x - 1][currentCell.y];
            break;
        case 1:
            // Down
            return maze[currentCell.x + 1][currentCell.y];
            break;
        case 2:
            // Left
            return maze[currentCell.x][currentCell.y - 1];
            break;
        case 3:
            // Right
            return maze[currentCell.x][currentCell.y + 1];
            break;
        default:
            // This should never happen.
            cout << "Error - Wrong action, see function move(..)\n";
    }
    cout << "Error - Wrong action, see function move(..)\n";
    return currentCell;
}

vector<vector<MazeCell> > initialize_maze() {
    // Get the size of the maze.
    ifstream inFile("maze-generator/maze_export");
    int count = std::count(istreambuf_iterator<char>(inFile),
               istreambuf_iterator<char>(), '\n');

    vector<vector<MazeCell> > maze(sqrt(count),
                                            vector<MazeCell>(sqrt(count)));

    ifstream in("maze-generator/maze_export");
    string str;

    // Skip the first line.
    getline(in, str);

    while(getline(in, str)) {
        if (str.size() > 0) {
            // Initialise maze.
            vector<string> splittedString = split(str, ',');

            // Create a MazeCell
            MazeCell cell;

            // Set a default reward.
            cell.reward = defaultReward;
            if (stoi(splittedString[2]) == 1) {
                cell.up = true;
            } else {
                cell.up = false;
            }

            if (stoi(splittedString[3]) == 1) {
                cell.down = true;
            } else {
                cell.down = false;
            }

            if (stoi(splittedString[4]) == 1) {
                cell.left = true;
            } else {
                cell.left = false;
            }

            if (stoi(splittedString[5]) == 1) {
                cell.right = true;
            } else {
                cell.right = false;
            }

            if (stoi(splittedString[6]) == 1) {
                cell.isStart = true;
            } else {
                cell.isStart = false;
            }

            if (stoi(splittedString[7]) == 1) {
                cell.isEnd = true;

                // Set a finish reward.
                cell.reward = finalReward;
            } else {
                cell.isEnd = false;
            }

            cell.x = stoi(splittedString[0]);
            cell.y = stoi(splittedString[1]);

            maze[cell.x][cell.y] = cell;
        }
    }

    return maze;
}

void random_action_init(vector<vector<CellValue> > &mazeValues, vector<vector<MazeCell> > maze, MazeCell &startCell, MazeCell &endCell) {
    // Random generator
    default_random_engine generator(seed);
    uniform_real_distribution<double> distribution(0.0,1.0);
    
    for (int i = 0, n = maze.size(); i < n; i++) {
        for (int j = 0; j < n; j++) {
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
    return;
}


void print_maze(int size, vector<vector<MazeCell> > maze) {
    int iStart, jStart, iEnd, jEnd;

    cout << ".";
    for (int i = 0; i < size; i++) {
        cout << "_.";
    }
    cout << '\n';

    for (int i = 0; i < size; i++) {
        cout << "|";
        for (int j = 0; j < size; j++) {
            if (maze[i][j].down == 0) {
                cout << "_";
            } else {
                cout << " ";
            }

            if (maze[i][j].right == 0) {
                cout << "|";
            } else {
                cout << ".";
            }

            if (maze[i][j].isStart) {
                iStart = i;
                jStart = j;
            } else if (maze[i][j].isEnd) {
                iEnd = i;
                jEnd = j;
            }
        }
        cout << '\n';
    }

    cout << "\nStart coordinates: (" << iStart << ", " << jStart << ").\n";
    cout << "End coordinates:   (" << iEnd << ", " << jEnd << ").\n";
    return;
}

// Makes use of unicode, might not work in every terminal.
void print_optimal_actions(int size, vector<vector<MazeCell> > maze, vector<vector<CellValue> > mazeValues) {
    cout << ".";
    for (int i = 0; i < size; i++) {
        cout << "_.";
    }
    cout << '\n';
    
    string color_start = "\033[4;31m"; //\031[4m"; // black
    string color_end = "\033[0m"; //\031[0m";
    
    for (int i = 0; i < mazeValues.size(); i++) {
        cout << "|";
        for (int j = 0; j < mazeValues.size(); j++) {
            string arrow;
            int index = findOptimalAction(mazeValues[i][j].actions, maze[i][j], 4);
            switch(index) {
                case 0:
                    arrow = "↑";
                    break;
                case 1:
                    arrow = "↓";
                    break;
                case 2:
                    arrow = "←";
                    break;
                case 3:
                    arrow = "→";
                    break;
                default:
                    arrow = ".";
            }

            if (maze[i][j].isEnd) {
                if (maze[i][j].down == 0) {
                    arrow = "\033[4;32m" + arrow + "\033[0m";
                } else {
                    arrow = "\033[32m" + arrow + "\033[0m";
                }
            } else if (maze[i][j].isStart) {
                if (maze[i][j].down == 0) {
                    arrow = "\033[4;34m" + arrow + "\033[0m";
                } else {
                    arrow = "\033[34m" + arrow + "\033[0m";
                }
            } else if (maze[i][j].down == 0) {
                arrow = "\033[4m" + arrow + "\033[0m";
            }
            
            cout << arrow;
            
            if (maze[i][j].right == 0) {
                cout << "|";
            } else {
                cout << ".";
            }
        }
        cout << '\n';
    }

    for (int i = 0; i < mazeValues.size(); i++) {
        for (int j = 0; j < mazeValues.size(); j++) {
            int index = findOptimalAction(mazeValues[i][j].actions, maze[i][j], 4);
            //std::cout << mazeValues[i][j].actions[index] << " ";
        }
        //std::cout << '\n';
    }
}

vector<string> split(string strToSplit, char delimiter) {
    stringstream ss(strToSplit);
    string item;
    vector<string> splittedStrings;
    while (getline(ss, item, delimiter)) {
        splittedStrings.push_back(item);
    }
    return splittedStrings;
}

