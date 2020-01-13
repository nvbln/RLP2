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
    bool isDistractor;
    bool rewardTaken;
    bool up;
    bool down;
    bool left;
    bool right;
    double reward;
    double distractionReward;

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

void sarsa(vector<vector<MazeCell> > maze, int episodes, double greedyEpsilon);
void qlearning(vector<vector<MazeCell> > maze, int episodes, double greedyEpsilon);
int chooseAction(MazeCell cell, CellValue values, int episode, double greedyEpsilon);
int findOptimalAction(double values[], int length);
vector<string> split(string strToSplit, char delimiter);
vector<vector<MazeCell> > initialize_maze();
void print_maze(int size, vector<vector<MazeCell> > maze);
void print_optimal_actions(int size, vector<vector<MazeCell> > maze, vector<vector<CellValue> > mazeValues);

double alpha = 0.1;
double ygamma = 0.9;
double defaultReward = 0;
double defaultDistractionReward = 0;
double penaltyFactor = 0;
double finalReward = 100;

int main(int argc, const char * argv[]) {
    double greedyEpsilon = 0.4;
    
    vector<vector<MazeCell> > maze;
    
    maze = initialize_maze();
    
    print_maze(maze.size(), maze);
    
    cout << "Sarsa\n";
    sarsa(maze, 10000, greedyEpsilon);
    
    maze = initialize_maze();
    cout << "Q-Learning\n";
    qlearning(maze, 10000, greedyEpsilon);
    
    return 0;
}

void sarsa(vector<vector<MazeCell> > maze, int episodes, double greedyEpsilon) {
    MazeCell startCell, endCell, currentCell;

    // Random generator
    default_random_engine generator;
    uniform_real_distribution<double> distribution(0.0,1.0);

    // Create a matrix containing random state-action values.
    vector<vector<CellValue>> mazeValues(maze.size(),
            vector<CellValue>(maze.size()));
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

    cout << "\nInitial (random) optimal actions:\n";
    print_optimal_actions(maze.size(), maze, mazeValues);
    cout << '\n';

    // Keep track of best and worst performance.
    int maxSteps = 0;
    int minSteps = 276447231; // Highest possible int without overflow.
    int maxReward = 0;
    int maxRewardSteps = 0;

    for (int i = 0; i < episodes; i++) {
        int totalReward = 0;
        int rewardTaken = 0;
        int step = 0;

        // Set all distractors to false again.
        for (int j = 0; j < maze.size(); j++) {
            for (int k = 0; k < maze.size(); k++) {
                maze[j][k].rewardTaken = false;
            }
        }

        //cout << "Greedy epsilon: " << greedyEpsilon * (1 / exp((0.0001 * i))) << '\n';

        // Choose initial cell and action.
        currentCell = startCell;
        while (currentCell.x != endCell.x || currentCell.y != endCell.y) {
            step++;
            MazeCell newCell;
            
            int actionIndex = chooseAction(currentCell,
            mazeValues[currentCell.x][currentCell.y],
            i, greedyEpsilon);
            
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
                    newCell = maze[currentCell.x][currentCell.y - 1];
                    break;
                case 3:
                    // Right
                    newCell = maze[currentCell.x][currentCell.y + 1];
                    break;
                default:
                    // This should never happen.
                    newCell = currentCell;
            }

            int newActionIndex = chooseAction(newCell, mazeValues[newCell.x][newCell.y], i, greedyEpsilon);

            // Update the state-action value.
            double reward;
            if (newCell.isDistractor && !(newCell.rewardTaken)) {
                reward = newCell.distractionReward;
                maze[newCell.x][newCell.y].rewardTaken = true;
                rewardTaken = 1;
                //std::cout << "here?" << i;
            } else if (newCell.x == endCell.x && newCell.y == endCell.y) {
                // The next cell is the terminal state.
                // Deduct the reward times penalty factor.
                if (rewardTaken == 0) {
                    //std::cout << "Got the full reward!\n";
                }
                reward = newCell.reward - (penaltyFactor * defaultDistractionReward * rewardTaken);
            } else {
                reward = newCell.reward;
            }
            mazeValues[currentCell.x][currentCell.y].actions[actionIndex] 
                    += alpha * (reward 
                    + (ygamma * mazeValues[newCell.x][newCell.y].actions[newActionIndex])
                    - mazeValues[currentCell.x][currentCell.y].actions[actionIndex]);

            currentCell = newCell;
            actionIndex = newActionIndex;

            totalReward += reward;

            if (currentCell.x == 9 && currentCell.y == 2) {
                //print_optimal_actions(maze.size(), maze, mazeValues);
                //print_maze(maze.size(), maze);
                //return;
            }
        }

        if (rewardTaken == 0) {
            //std::cout << "No reward taken!\n";
        }

        if (step > maxSteps) {
            maxSteps = step;
        } else if (step < minSteps) {
            minSteps = step;
        }

        if (totalReward > maxReward) {
            maxReward = totalReward;
            maxRewardSteps = step;
        }


        // Report the total reward.
        //cout << "Total reward: " << totalReward << '\n';

        // Report whether the distraction reward was taken.
        //cout << "Reward taken: " << rewardTaken << '\n';

        // Report number of steps.
        //cout << "Number of steps: " << step << '\n';
    }

    cout << '\n';
    print_optimal_actions(maze.size(), maze, mazeValues);
    print_maze(maze.size(), maze);

    cout << "\nMax steps: " << maxSteps << '\n';
    cout << "Min steps: " << minSteps << '\n';
    //cout << "Max reward: " << maxReward << " and steps: " << maxRewardSteps << '\n';
}

void qlearning(vector<vector<MazeCell> > maze, int episodes, double greedyEpsilon) {
    MazeCell startCell, endCell, currentCell;

    // Random generator
    default_random_engine generator;
    uniform_real_distribution<double> distribution(0.0,1.0);

    // Create a matrix containing random state-action values.
    vector<vector<CellValue> > mazeValues(maze.size(),
            vector<CellValue>(maze.size()));
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

    cout << "\nInitial (random) optimal actions:\n";
    print_optimal_actions(maze.size(), maze, mazeValues);
    cout << '\n';

    // Keep track of best and worst performance.
    int maxSteps = 0;
    int minSteps = 276447231; // Highest possible int without overflow.
    int size = maze.size();
    int maxReward = 0;
    int maxRewardSteps = 0;

    for (int i = 0; i < episodes; i++) {
        int totalReward = 0;
        int rewardTaken = 0;
        int step = 0;

        // Set all distractors to false again.
        for (int j = 0; j < maze.size(); j++) {
            for (int k = 0; k < maze.size(); k++) {
                maze[j][k].rewardTaken = false;
            }
        }
        
        //cout << "Greedy epsilon: " << greedyEpsilon * (1 / exp((0.0001 * i))) << '\n';
        
        // Choose initial cell and action.
        currentCell = startCell;

        while (currentCell.x != endCell.x || currentCell.y != endCell.y) {
            step++;
            MazeCell newCell;
            
            int actionIndex = chooseAction(currentCell,
                              mazeValues[currentCell.x][currentCell.y],
                              i, greedyEpsilon);

            switch(actionIndex) {
                case 0:
                    // Up
                    newCell = maze[currentCell.x-1][currentCell.y];
                    break;
                case 1:
                    // Down
                    newCell = maze[currentCell.x+1][currentCell.y];
                    break;
                case 2:
                    // Left
                    newCell = maze[currentCell.x][currentCell.y-1];
                    break;
                case 3:
                    // Right
                    newCell = maze[currentCell.x][currentCell.y+1];
                    break;
                default:
                    // This should never happen.
                    newCell = currentCell;
            }

            int newActionIndex = chooseAction(newCell, mazeValues[newCell.x][newCell.y], i, 0);

            // Update the state-action value.
            double reward;
            if (newCell.isDistractor && !(newCell.rewardTaken)) {
                reward = newCell.distractionReward;
                maze[newCell.x][newCell.y].rewardTaken = true;
                rewardTaken = 1;
            } else if (newCell.x == endCell.x && newCell.y == endCell.y) {
                // The next cell is the terminal state.
                // Deduct the reward times penalty factor.
                reward = newCell.reward - (penaltyFactor * defaultDistractionReward * rewardTaken);
            } else {
                reward = newCell.reward;
            }
            mazeValues[currentCell.x][currentCell.y].actions[actionIndex]
                    += alpha * (reward
                    + (ygamma * mazeValues[newCell.x][newCell.y].actions[newActionIndex])
                    - mazeValues[currentCell.x][currentCell.y].actions[actionIndex]);

            currentCell = newCell;

            totalReward += reward;
        }

        if (rewardTaken == 0) {
            //std::cout << "No reward taken!\n";
        }

        if (step > maxSteps) {
            maxSteps = step;
        } else if (step < minSteps) {
            minSteps = step;
        }

        if (totalReward > maxReward) {
            maxReward = totalReward;
            maxRewardSteps = step;
        }

        // Report the total reward.
        // cout << "Total reward: " << totalReward << '\n';

        // Report whether the distraction reward was taken.
        //cout << "Reward taken: " << rewardTaken << '\n';

        // Report number of steps.
        //cout << "Number of steps: " << step << '\n';
    }

    cout << '\n';
    print_optimal_actions(maze.size(), maze, mazeValues);
    print_maze(maze.size(), maze);

    cout << "\nMax steps: " << maxSteps << '\n';
    cout << "Min steps: " << minSteps << '\n';
    //cout << "Max reward: " << maxReward << "and steps: " << maxRewardSteps << '\n';
}

int chooseAction(MazeCell cell, CellValue values, int episode, double greedyEpsilon) {
    int optimalAction = findOptimalAction(values.actions, 4);
    
    if (!cell.up && optimalAction == 0) {
        optimalAction = 1;
    }
    if (!cell.down && optimalAction == 1) {
        optimalAction = 2;
    }
    if (!cell.left && optimalAction == 2) {
        optimalAction = 3;
    }
    if (!cell.right && optimalAction == 3) {
        optimalAction = 0;
    }
    if (!cell.up && optimalAction == 0) {
        optimalAction = 1;
    }
    if (!cell.down && optimalAction == 1) {
        optimalAction = 2;
    }
    
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

            if (stoi(splittedString[8]) == 1) {
                cell.isDistractor = true;
                cell.rewardTaken = false;

                // Set a distractor reward.
                cell.distractionReward = defaultDistractionReward;
            } else {
                cell.isDistractor = false;
            }

            cell.x = stoi(splittedString[0]);
            cell.y = stoi(splittedString[1]);

            maze[cell.x][cell.y] = cell;
        }
    }

    return maze;
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
            } else if (maze[i][j].isDistractor) {
                cout << "x";
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
            int index = findOptimalAction(mazeValues[i][j].actions, 4);
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
            
            if (maze[i][j].isDistractor) {
                if (maze[i][j].down == 0) {
                    arrow = "\033[4;31m" + arrow + "\033[0m";
                } else {
                    arrow = "\033[31m" + arrow + "\033[0m";
                }
            } else if (maze[i][j].isEnd) {
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
            int index = findOptimalAction(mazeValues[i][j].actions, 4);
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

