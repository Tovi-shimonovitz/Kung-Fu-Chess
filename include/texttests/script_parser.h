#pragma once
#include <istream>
#include <string>
#include <vector>
#include "script_data.h" 

class ScriptParser {
public:
    static ScriptData parse(std::istream& input);

private:
    static bool isCommand(const std::string& line);
    static Command parseCommand(const std::string& line);
};
