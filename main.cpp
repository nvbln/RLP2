//
//  main.cpp
//  mazes
//
//  Created by Galina on 10/12/2019.
//  Copyright © 2019 Galina and Nathan. All rights reserved.
//

#include <iostream>
#include <vector>

struct MazeCell {
    bool isWall;
    int reward;
    
};

struct Agent {
    int strat;
    
};

void initialize_maze(std::vector<std::vector<MazeCell>> maze);
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
    
    initialize_maze(maze);
    
    print_maze(size, maze);
    
    
    // Make an agent
    
    // Allow agent to  interact with the maze
    
    // Make agent learn
    
    // Evaluate agent
    return 0;
}

void ininitialize_maze(int size, std::vector<std::vector<MazeCell>> maze) {
    ;
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

