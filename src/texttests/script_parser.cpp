#pragma once
#include "../../include/texttests/script_parser.h"
#include "../../include/texttests/script_data.h"
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../include/io/BoardParser.h"

bool ScriptParser::isCommand(const std::string& line) {
        return (line.find("click") == 0 || line.find("wait") == 0 || line.find("print") == 0);
    }

Command ScriptParser::parseCommand(const std::string& line) {
        std::stringstream ss(line);
        std::string cmdName;
        ss >> cmdName;

        if (cmdName != "click" && cmdName != "wait" && cmdName != "print") {
            throw std::invalid_argument("Unknown command: " + cmdName);
        }

        Command cmd;
        cmd.command = cmdName;
        std::string arg;
        
        while (ss >> arg) {
            cmd.args.push_back(arg);
        }

        if (cmdName == "click" && cmd.args.size() != 2) {
            throw std::runtime_error("Command 'click' requires exactly 2 arguments.");
        }
        if (cmdName == "wait" && cmd.args.size() != 1) {
            throw std::runtime_error("Command 'wait' requires exactly 1 argument.");
        }
        if (cmdName == "print" && (cmd.args.size() != 1 || cmd.args[0] != "board")) {
            throw std::runtime_error("Command 'print' requires argument 'board'.");
        }

        return cmd;
    }


ScriptData ScriptParser::parse(std::istream& input) {
        ScriptData data;
        std::string line;
        std::stringstream boardBuffer;
        bool boardFound = false;

       while (std::getline(input, line)) {
            if (line.find("Board") != std::string::npos) {
                boardFound = true;
                break;
            }
        }

        if (!boardFound) throw std::runtime_error("Script error: 'Board' keyword not found.");
        //Enter the board to buffer until the commands
        while (std::getline(input, line)) {
            if (line.empty()) continue;

            if (line == "Commands:") {
                break;
            }

            if (isCommand(line)) {
                data.commands.push_back(parseCommand(line));
                break;
            } else {
                boardBuffer << line << "\n";
            }
        }

        data.board = std::make_unique<Board>(BoardParser::parseFromTextLines(boardBuffer));

        //Enter commands to the vector
        while (std::getline(input, line)) {
            if (line.empty()) continue;
            data.commands.push_back(parseCommand(line));
        }
        return data;
    }


