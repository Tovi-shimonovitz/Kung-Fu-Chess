#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "../io/BoardParser.h" 

struct Command {
    std::string command;
    std::vector<std::string> args;
};

struct ScriptData {
    std::unique_ptr<Board> board;
    std::vector<Command> commands;
    ScriptData() = default;
};