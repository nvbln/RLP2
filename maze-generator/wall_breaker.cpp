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
    bool isHelper = false;

    // Location in matrix.
    int x;
    int y;
};

vector<vector<MazeCell> > initialize_maze();
vector<string> split(string strToSplit, char delimiter);
void write_maze(vector<vector<MazeCell> > maze);
vector<vector<MazeCell> > break_maze_down(vector<vector<MazeCell> > maze, int wallsToBreak);
void print_maze(int size, vector<vector<MazeCell> > maze);

int main(int argc, const char * argv[]) {
    int wallsToBreak = 4000;

    vector<vector<MazeCell> > maze;
    maze = initialize_maze();

    cout << "Regular maze:\n";
    print_maze(maze.size(), maze);

    // Break the maze down.
    vector<vector<MazeCell> > broken_maze;
    broken_maze = break_maze_down(maze, wallsToBreak);

    cout << "Broken maze\n";
    print_maze(maze.size(), broken_maze);

    // Write the maze to file.
    write_maze(broken_maze);
}

/** Copied from main.cpp **/
vector<vector<MazeCell> > initialize_maze() {
    // Get the size of the maze.
    ifstream inFile("maze_export");
    int count = std::count(istreambuf_iterator<char>(inFile),
               istreambuf_iterator<char>(), '\n');
    int size = sqrt(count);

    vector<vector<MazeCell> > maze(size, vector<MazeCell>(size));

    ifstream in("maze_export");
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
            //cell.reward = defaultReward;
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
                //cell.reward = finalReward;
            } else {
                cell.isEnd = false;
            }

            if (stoi(splittedString[8]) == 1) {
                // Set isHelper to true.
                cell.isHelper = true;
            }

            cell.x = stoi(splittedString[0]);
            cell.y = stoi(splittedString[1]);

            maze[cell.x][cell.y] = cell;
        }
    }

    return maze;
}

/** Copied from main.cpp **/
vector<string> split(string strToSplit, char delimiter) {
    stringstream ss(strToSplit);
    string item;
    vector<string> splittedStrings;
    while (getline(ss, item, delimiter)) {
        splittedStrings.push_back(item);
    }
    return splittedStrings;
}

vector<vector<MazeCell> > break_maze_down(vector<vector<MazeCell> > maze, int wallsToBreak) {
    // Initialise random generator.
    default_random_engine generator(123);

    int size = maze.size();

    for (int i = 0; i < wallsToBreak; i++) {
        uniform_real_distribution<double> distribution(0, 1.0);
        int x = (int) (size * distribution(generator));
        int y = (int) (size * distribution(generator));

        // Break down one of the walls.
        double randomDirection = distribution(generator);
        if (randomDirection >= 0 && randomDirection < 0.25 && !maze[x][y].up) {
            // You can't go further up either way.
            if (x != 0) {
                // Break the wall down.
                maze[x][y].up = true;

                // Also break the other side down.
                maze[x - 1][y].down = true;
            }
        } else if (randomDirection >= 0.25 && randomDirection < 0.5 && !maze[x][y].down) {
            if (x != size - 1) {
                // Break the wall down.
                maze[x][y].down = true;

                // Also break the other side down.
                maze[x + 1][y].up = true;
            }
        } else if (randomDirection >= 0.5 && randomDirection < 0.75 && !maze[x][y].left) {
            if (y != 0) {
                // Break the wall down.
                maze[x][y].left = true;

                // Also break the other side down.
                maze[x][y - 1].right = true;
            }
        } else if (randomDirection >= 0.75 && randomDirection <= 1 && !maze[x][y].right) {
            if (y != size - 1) {
                // Break the wall down.
                maze[x][y].right = true;

                // Also break the other side down.
                maze[x][y + 1].left = true;
            }
        }
    }
    return maze;
}

void write_maze(vector<vector<MazeCell> > maze) {
    ofstream of("maze_export_broken");
    
    // Specify columns
    of << "row,column,up,down,left,right,start,end,helper\n";

    for (int i = 0; i < maze.size(); i++) {
        for (int j = 0; j < maze.size(); j++) {
            // Convert booleans to integers.
            int up = maze[i][j].up ? 1 : 0;
            int down = maze[i][j].down ? 1 : 0;
            int left = maze[i][j].left ? 1 : 0;
            int right = maze[i][j].right ? 1 : 0;
            int start = maze[i][j].isStart ? 1 : 0;
            int end = maze[i][j].isEnd ? 1 : 0;
            int helper = maze[i][j].isHelper ? 1 : 0;

            // Write the tile to file.
            of << i << "," << j << "," << up << "," << down << "," << left << "," << right << "," << start << "," << end << "," << helper << '\n';
        }
    }

    of.close();
}

/** Copied from main.cpp **/
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
                if (maze[i][j].isHelper) {
                    cout << "\033[4mx\033[0m"; // x with underscore
                } else {
                    cout << "_";
                }
            } else if (maze[i][j].isHelper) {
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
