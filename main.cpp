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

struct MazeCell {
    bool isWall;
    bool isStart;
    bool isEnd;
    int reward;
    
};

struct Agent {
    int strat;
    
};

std::vector<std::string> split(std::string strToSplit, char delimiter);
std::vector<std::vector<MazeCell>> initialize_maze();
void print_maze(int size, std::vector<std::vector<MazeCell>> maze);

int main(int argc, const char * argv[]) {
    char m [5][5] =
    {   {'-','-','-','-','-'},
        {'.','.','-','.','.'},
        {'-','.','.','.','-'},
        {'-','.','-','.','-'},
        {'-','-','-','-','-'}
    };
    
    int size = 5;
    std::vector<std::vector<MazeCell>> maze;
    
    maze = initialize_maze();
    
    print_maze(size, maze);
    
    
    // Make an agent
    
    // Allow agent to  interact with the maze
    
    // Make agent learn
    
    // Evaluate agent
    return 0;
}

std::vector<std::vector<MazeCell>> initialize_maze() {
    // Get the size of the maze.
    std::ifstream inFile("maze-generator/maze_export");
    int count = std::count(std::istreambuf_iterator<char>(inFile),
               std::istreambuf_iterator<char>(), '\n');
    std::cout << count;

    std::vector<std::vector<MazeCell>> maze(std::sqrt(count),
                                            std::vector<MazeCell>(std::sqrt(count)));

    std::ifstream in("maze-generator/maze_export");
    std::string str;

    while(std::getline(in, str)) {
        if (str.size() > 0) {
            // Initialise maze.
            std::vector<std::string> splittedString = split(str, ',');

            // Create a MazeCell
            MazeCell cell;
            if (std::stoi(splittedString[2]) == 1) {
                cell.isWall = true;
            } else {
                cell.isWall = false;
            }

            if (std::stoi(splittedString[3]) == 1) {
                cell.isStart = true;
            } else {
                cell.isStart = false;
            }

            if (std::stoi(splittedString[4]) == 1) {
                cell.isEnd = true;
            } else {
                cell.isEnd = false;
            }

            maze[std::stoi(splittedString[0])][std::stoi(splittedString[1])]
                    = cell;
        }
    }

    return maze;
}

void print_maze(int size, std::vector<std::vector<MazeCell>> maze) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            std::cout << (maze[i][j].isWall?'-':'.');
        }
        std::cout <<'\n';
    }
    return;
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

