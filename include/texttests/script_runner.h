#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include "../engine/GameEngine.h"
#include "../input/Controller.h"
#include "../io/board_printer.h"
#include "script_data.h"


class ScriptRunner {
public:
    ScriptRunner(GameEngine& engine, Controller& controller, BoardPrinter& printer);

    void run(const std::vector<Command>& commands);

private:
    using CommandFunc = std::function<void(const std::vector<std::string>&)>;

    GameEngine& m_engine;
    Controller& m_controller;
    BoardPrinter& m_printer;
    std::unordered_map<std::string, CommandFunc> m_commandMap;

    void handleClick(const std::vector<std::string>& args);
    void handleWait(const std::vector<std::string>& args);
    void handlePrint(const std::vector<std::string>& args);
};
