// This program learns how to find the maze's exit.
// Possible RL algorithms for that are Sarsa and Q-learing.
// by Nathan van Beelen and Lesnic Galina

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
#include <time.h>


using namespace std;

struct MazeCell {
    bool isStart;
    bool isEnd;
    bool up;
    bool down;
    bool left;
    bool right;
    double reward;
    bool isHelper = false;

    // Location in matrix.
    int x;
    int y;
};

struct CellValue {
    // Indexing: 0 = up, 1 = down, left = 2, right = 3.
    double actions[4];
};

struct CellValueVector {
    // Indexing: 0 = up, 1 = down, left = 2, right = 3.
    vector<double> actions[4];
};

// RL algorithms
void sarsa(vector<vector<MazeCell> > maze, bool withHelpers);
void qlearning(vector<vector<MazeCell> > maze, bool withHelpers);
void montecarlo(vector<vector<MazeCell> > maze, bool withHelpers);

// Maze initialization
vector<vector<MazeCell> > initialize_maze(bool breakDown, bool withHelpers);
void random_action_init(vector<vector<CellValue> > &mazeValues, vector<vector<MazeCell> > maze, MazeCell &startCell, MazeCell &endCell, bool terminalZero);

// Choosing action
int chooseAction(MazeCell cell, CellValue values, int episode, double greedyEpsilon);
int findOptimalAction(double values[], MazeCell c, int length);

// Printing functions
void print_maze(int size, vector<vector<MazeCell> > maze);
void print_optimal_actions(int size, vector<vector<MazeCell> > maze, vector<vector<CellValue> > mazeValues);

// Utility functions
MazeCell index2NewCell(int actionIndex, vector<vector<MazeCell> > &maze, MazeCell currentCell);
vector<string> split(string strToSplit, char delimiter);
void parameterSweep();

int seed = 0;
int episodes = 10000;

double alpha = 0.1;
double ygamma = 0.95;
double greedyEpsilon = 0.4;

double defaultReward = -0.98;
double finalReward = 100;
double helperReward = 0.1;

bool pSweep = false;
ofstream parameterPerformance;

bool breakDown = true; // Remove some walls from the maze to allow for more paths.

int run = 10;

int main(int argc, const char * argv[]) {
    bool withHelpers = false;
    
    vector<vector<MazeCell> > maze;

    //parameterSweep();
    
    maze = initialize_maze(breakDown, withHelpers);
    print_maze(maze.size(), maze);
    
    cout << "Sarsa\n";
    sarsa(maze, withHelpers);
    
    // RNG seed is the same for both algos
    maze = initialize_maze(breakDown, withHelpers);
    cout << "Q-Learning\n";
    qlearning(maze, withHelpers);

    //maze = initialize_maze(breakDown);
    //cout << "Monte Carlo\n";
    //montecarlo(maze, 10000, greedyEpsilon, withHelpers);

    // With helpers
    withHelpers = true;

    maze = initialize_maze(breakDown, withHelpers);
    cout << "Sarsa\n";
    sarsa(maze, withHelpers);
    
    maze = initialize_maze(breakDown, withHelpers);
    cout << "Q-Learning\n";
    qlearning(maze, withHelpers);

    //maze = initialize_maze(breakDown);
    //cout << "Monte Carlo\n";
    //montecarlo(maze, 10000, greedyEpsilon, withHelpers);
    
    return 0;
}

void sarsa(vector<vector<MazeCell> > maze, bool withHelpers) {
    MazeCell startCell, endCell, currentCell;

    // Create a matrix containing random state-action values.
    vector<vector<CellValue> > mazeValues(maze.size(),
            vector<CellValue>(maze.size()));
    
    random_action_init(mazeValues, maze, startCell, endCell, true);

    // Keep track of best and worst performance.
    int maxSteps = 0;
    int minSteps = 276447231; // Highest possible int without overflow.
    int maxReward = 0;
    int maxRewardSteps = 0;
    double finalReward = 0;

    // Open file to write performance to.
    ofstream sarsaPerformance;
    string breakDownString = "";
    if (breakDown) {
        breakDownString = "_broken";
    }
    if (withHelpers) {
        sarsaPerformance.open("sarsa_performance_help" + breakDownString + to_string(run) + ".csv");
    } else {
        sarsaPerformance.open("sarsa_performance" + breakDownString + to_string(run) + ".csv");
    }
    sarsaPerformance << "episode, reward\n";

    time_t startTime = time(0);
    bool overTime = false;
    double totalRewardEpisodes = 0;

    for (int i = 0; i < episodes; i++) {
        double totalReward = 0;
        int step = 0;

        // Choose initial cell and action.
        currentCell = startCell;
        while (currentCell.x != endCell.x || currentCell.y != endCell.y) {
            //cout << "time: " << difftime(time(0), startTime) << '\n';
            if (pSweep && difftime(time(0), startTime) > 5) {
                // Stop if we have to wait for longer than 10 seconds.
                overTime = true;
                break;
            }

            step++;
            MazeCell newCell;
            
            // Find the best action.
            int actionIndex = chooseAction(currentCell, mazeValues[currentCell.x][currentCell.y], i, greedyEpsilon);
            newCell = index2NewCell(actionIndex, maze, currentCell);
            
            // Find the next best action on-policy.
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

        if (overTime) {
            // Do not record this take.
            break;
        }

        totalRewardEpisodes += totalReward;
        
        // Record performance over time.
        sarsaPerformance << i << ", " << totalReward << '\n';

        if (!pSweep) {
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
    }

    sarsaPerformance.close();

    if (!pSweep) {
        cout << '\n';
        print_optimal_actions(maze.size(), maze, mazeValues);
        print_maze(maze.size(), maze);

        cout << "\nMax steps: " << maxSteps << '\n';
        cout << "Min steps: " << minSteps << '\n';
        cout << "Max reward: " << maxReward << " and steps: " << maxRewardSteps << '\n';
        cout << "Final reward: " << finalReward << '\n';
    } else {
        double averageReward = totalRewardEpisodes / (double) episodes;
        parameterPerformance << "sarsa," << alpha << "," << ygamma << "," << greedyEpsilon << "," << defaultReward << "," << averageReward << "\n";
    }
}

void qlearning(vector<vector<MazeCell> > maze, bool withHelpers) {
    MazeCell startCell, endCell, currentCell;

    // Create a matrix containing random state-action values.
    vector<vector<CellValue> > mazeValues(maze.size(), vector<CellValue>(maze.size()));
    
    random_action_init(mazeValues, maze, startCell, endCell, true);

    // Keep track of best and worst performance.
    int maxSteps = 0;
    int minSteps = 276447231; // Highest possible int without overflow.
    int size = maze.size();
    int maxReward = 0;
    int maxRewardSteps = 0;
    int finalReward = 0;

    // Open file to write performance to.
    ofstream qlearningPerformance;
    string breakDownString = "";
    if (breakDown) {
        breakDownString = "_broken";
    }
    if (withHelpers) {
        qlearningPerformance.open("qlearning_performance_help" + breakDownString + to_string(run) + ".csv");
    } else {
        qlearningPerformance.open("qlearning_performance" + breakDownString + to_string(run) + ".csv");
    }
    qlearningPerformance << "episode, reward\n";

    time_t startTime = time(0);
    bool overTime = false;
    double totalRewardEpisodes = 0;

    for (int i = 0; i < episodes; i++) {
        double totalReward = 0;
        int step = 0;
        
        // Choose initial cell and action.
        currentCell = startCell;

        while (currentCell.x != endCell.x || currentCell.y != endCell.y) {
            if (pSweep && difftime(time(0), startTime) > 5) {
                // Stop if we have to wait for longer than 10 seconds.
                overTime = true;
                break;
            }

            step++;
            MazeCell newCell;
            
            // Find the best action.
            int actionIndex = chooseAction(currentCell, mazeValues[currentCell.x][currentCell.y], i, greedyEpsilon);
            newCell = index2NewCell(actionIndex, maze, currentCell);

            // Find the next best action off-policy.
            int newActionIndex = chooseAction(newCell, mazeValues[newCell.x][newCell.y], i, 0);

            // Update the state-action value.
            mazeValues[currentCell.x][currentCell.y].actions[actionIndex]
                    += alpha * (newCell.reward
                    + (ygamma * mazeValues[newCell.x][newCell.y].actions[newActionIndex])
                    - mazeValues[currentCell.x][currentCell.y].actions[actionIndex]);

            currentCell = newCell;

            totalReward += newCell.reward;
        }

        if (overTime) {
            // Do not record this take.
            break;
        }

        totalRewardEpisodes += totalReward;
        
        // Record performance over time.
        qlearningPerformance << i << ", " << totalReward << '\n';

        if (!pSweep) {
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
    }

    qlearningPerformance.close();

    if (!pSweep) {
        cout << '\n';
        print_optimal_actions(maze.size(), maze, mazeValues);
        print_maze(maze.size(), maze);

        cout << "\nMax steps: " << maxSteps << '\n';
        cout << "Min steps: " << minSteps << '\n';
        cout << "Max reward: " << maxReward << " and steps: " << maxRewardSteps << '\n';
        cout << "Final reward: " << finalReward << '\n';
    } else {
        double averageReward = totalRewardEpisodes / (double) episodes;
        parameterPerformance << "qlearning," << alpha << "," << ygamma << "," << greedyEpsilon << "," << defaultReward << "," << averageReward << "\n";
    }
}

void montecarlo(vector<vector<MazeCell> > maze, int episodes, double greedyEpsilon, bool withHelpers) {
    MazeCell startCell, endCell, currentCell;

    // Create a matrix containing random state-action values.
    vector<vector<CellValue> > mazeValues(maze.size(), vector<CellValue>(maze.size()));
    vector<vector<CellValueVector> > returns(maze.size(), vector<CellValueVector>(maze.size()));
    random_action_init(mazeValues, maze, startCell, endCell, false);

    // Keep track of best and worst performance.
    int maxSteps = 0;
    int minSteps = 276447231;
    int size = maze.size();
    int maxReward = 0;
    int maxRewardSteps = 0;
    int finalReward = 0;

    // Open file to write performance to.
    ofstream montecarloPerformance;
    if (withHelpers) {
        montecarloPerformance.open("montecarlo_performance_help.csv");
    } else {
        montecarloPerformance.open("montecarlo_performance.csv");
    }
    montecarloPerformance << "episode, reward\n";

    for (int i = 0; i < episodes; i++) {
        double totalReward = 0;
        int step = 0;

        // Choose initial cell and action.
        currentCell = startCell;

        // Keep track of states and actions.
        vector<MazeCell> states;
        vector<int> actions; // As always: 0 = up, 1 = down, 2 = left, 3 = right.

        states.push_back(startCell);

        // Generate episode following the policy.
        while (currentCell.x != endCell.x || currentCell.y != endCell.y) {
            step++;
            MazeCell newCell;

            int actionIndex = chooseAction(currentCell, 
                    mazeValues[currentCell.x][currentCell.y], i, greedyEpsilon);

            newCell = index2NewCell(actionIndex, maze, currentCell);

            currentCell = newCell;
            totalReward += newCell.reward;

            states.push_back(currentCell);
            actions.push_back(actionIndex);
        }

        int previousReward = 0;
        // states.size() - 1 because we don't need to do this for the terminal state.
        for (int j = 0; j < states.size() - 1; j++) {
            previousReward += ygamma * states[j].reward;
            bool encounteredPreviously = false;
            for (int k = 0; k < j; k++) {
                if (states[k].x == states[j].x && states[k].y == states[j].y && actions[k] == actions[j]) {
                    // Same state-action pair, don't give a reward.
                    encounteredPreviously = true;
                }
            }

            if (!encounteredPreviously) {
                returns[states[j].x][states[j].y].actions[actions[j]].push_back(previousReward);
                double averageReward = 0;
                vector<double> currentPair = returns[states[j].x][states[j].y].actions[actions[j]];
                for (int k = 0; k < currentPair.size(); k++) {
                    averageReward += currentPair[k];
                }

                averageReward /= currentPair.size();
                mazeValues[states[j].x][states[j].y].actions[actions[j]] = averageReward;

                double highestActionValue = 0;
                int highestActionValueIndex = 0;
                for (int k = 0; k < 4; k++) {
                    double currentValue = mazeValues[states[j].x][states[j].y].actions[k];
                    if (currentValue > highestActionValue) {
                        highestActionValue = currentValue;
                        highestActionValueIndex = k;
                    }
                }

                for (int k = 0; k < 4; k++) {
                    if (k == highestActionValueIndex) {
                        mazeValues[states[j].x][states[j].y].actions[k] 
                                = 1 - greedyEpsilon + (greedyEpsilon / abs(highestActionValue));
                    } else {
                        mazeValues[states[j].x][states[j].y].actions[k] 
                                = greedyEpsilon / abs(highestActionValue);
                    }
                }
            }
        }
    }
}

vector<vector<MazeCell> > initialize_maze(bool breakDown, bool withHelpers) {
    // Get the size of the maze.
    string inputPath;
    if (breakDown) {
        inputPath = "maze-generator/maze_export_broken";
    } else {
        inputPath = "maze-generator/maze_export";
    }
    ifstream inFile(inputPath);
    int count = std::count(istreambuf_iterator<char>(inFile),
               istreambuf_iterator<char>(), '\n');
    int size = sqrt(count);

    vector<vector<MazeCell> > maze(size, vector<MazeCell>(size));

    ifstream in(inputPath);
    string str;

    // Skip the first line.
    getline(in, str);

    // Parsing file line.
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
                if (withHelpers) {
                    // Set isHelper to true.
                    cell.isHelper = true;
                    cell.reward = helperReward;
                } else {
                    cell.isHelper = 0;
                }
            }

            cell.x = stoi(splittedString[0]);
            cell.y = stoi(splittedString[1]);

            maze[cell.x][cell.y] = cell;
        }
    }

    return maze;
}

void random_action_init(vector<vector<CellValue> > &mazeValues, vector<vector<MazeCell> > maze, MazeCell &startCell, MazeCell &endCell, bool terminalZero) {
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

    if (terminalZero) {
        // Set all terminal actions to zero value.
        mazeValues[endCell.x][endCell.y].actions[0] = 0;
        mazeValues[endCell.x][endCell.y].actions[1] = 0;
        mazeValues[endCell.x][endCell.y].actions[2] = 0;
        mazeValues[endCell.x][endCell.y].actions[3] = 0;
    }

    return;
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
    // Start with first possible action value.
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
    
    // Find the highest action value.
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

    return optimalAction;
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
            if (maze[i][j].isEnd) {
                if (maze[i][j].down == 0) {
                    // E with underscore
                    cout << "\033[4mE\033[0m";
                } else {
                    cout << "E";
                }
            } else if (maze[i][j].isStart) {
                if (maze[i][j].down == 0) {
                    // S with underscore
                    cout << "\033[4mS\033[0m";
                } else {
                    cout << "S";
                }
            } else if (maze[i][j].isHelper) {
                if (maze[i][j].down == 0) {
                    // x with underscore
                    cout << "\033[4mx\033[0m";
                } else {
                    cout << "x";
                }
            } else if (maze[i][j].down == 0) {
                // Color black, with underscore
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
                // Color green
                if (maze[i][j].down == 0) {
                    // Draw with underscore
                    arrow = "\033[4;32m" + arrow + "\033[0m";
                } else {
                    arrow = "\033[32m" + arrow + "\033[0m";
                }
            } else if (maze[i][j].isStart) {
                // Color blue
                if (maze[i][j].down == 0) {
                    // Draw with underscore
                    arrow = "\033[4;34m" + arrow + "\033[0m";
                } else {
                    arrow = "\033[34m" + arrow + "\033[0m";
                }
            } else if (maze[i][j].isHelper) {
                // Color purple
                if (maze[i][j].down == 0) {
                    // Draw with underscore
                    arrow = "\033[4;35m" + arrow + "\033[0m";
                } else {
                    arrow = "\033[35m" + arrow + "\033[0m";
                }
            } else if (maze[i][j].down == 0) {
                // Color black, with underscore
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

vector<string> split(string strToSplit, char delimiter) {
    stringstream ss(strToSplit);
    string item;
    vector<string> splittedStrings;
    while (getline(ss, item, delimiter)) {
        splittedStrings.push_back(item);
    }
    return splittedStrings;
}

void parameterSweep() {
    pSweep = true;
    parameterPerformance.open("parameter_performance.csv");
    parameterPerformance << "algorithm,alpha,ygamma,greedyEpsilon,defaultReward,averageReward\n";

    vector<vector<MazeCell> > maze;

    int i = 0;
    for (alpha = 0.1; alpha < 0.7; alpha += 0.1) {
        for (ygamma = 0.95; ygamma < 1; ygamma += 0.01) {
            for(greedyEpsilon = 0.1; greedyEpsilon < 0.5; greedyEpsilon += 0.1) {
                for (defaultReward = -0.9; defaultReward > -1; defaultReward -= 0.01) {
                    maze = initialize_maze(true, false);
                    sarsa(maze, false);

                    maze = initialize_maze(true, false);
                    qlearning(maze, false);
                    cout << "Done!" << i << "\n";
                    i++;
                }
            }
        }
    }

    parameterPerformance.close();
    pSweep = false;
}
