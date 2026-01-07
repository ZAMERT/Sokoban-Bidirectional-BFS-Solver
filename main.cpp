#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include "sokoban.hpp"

void input_find(int size, char *arr[], std::string &argv_input)
{
    for (int i = 0; i < size; ++i)
    {
        std::string argv_str = arr[i];
        std::cout << argv_str << std::endl;
        if(argv_str == "<")
        {
            argv_input = arr[i+1];
            return;
        }
    }

    return;
}

void output_find(int size, char *arr[], std::string &argv_output)
{
    for (int i = 0; i < size; ++i)
    {
        std::string argv_str = arr[i];
        if(argv_str == ">")
        {
            argv_output = arr[i+1];
            return;
        }
    }

    return;
}

int main(int argc, char *argv[]) {
    std::vector<std::string> grid;


    // std::string argv_input;
    // input_find(argc, argv, argv_input);

    // std::cout << argv_input << std::endl;

    // Read the map
    // if(!argv_input.empty())
    // {
        // std::ifstream inputFile(argv_input);
        read_map(std::cin,grid);
        // for(auto &it:grid){
        //     std::cout << it << std::endl;
        // }
    // }
    // Solve the Sokoban puzzle
    std::string result = solve(grid);

    // std::string argv_output;
    // output_find(argc, argv, argv_output);

    // Output the result
    // if (!result.empty()) {
    //     std::cout << result << std::endl;
    // } else {
    //     std::cout << "No solution!" << std::endl;
    // }

    // if(!argv_output.empty())
    // {
        // std::ofstream outputFile(argv_input);
        writeMap(grid, result, std::cout);
    // }



    return 0;
}