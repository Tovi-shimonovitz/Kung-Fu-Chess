#pragma once
#include "../../include/texttests/script_runner.h"
#include <stdexcept>
#include <string>
#include <utility>
#include "../../include/texttests/script_data.h"

ScriptRunner::ScriptRunner(GameEngine& engine, Controller& controller, BoardMapper& boardMapper, BoardPrinter& printer)
    : m_engine(engine), m_controller(controller), m_boardMapper(boardMapper), m_printer(printer) {
    m_commandMap = {
        {"click", [this](const std::vector<std::string>& args) { handleClick(args); }},
        {"wait", [this](const std::vector<std::string>& args) { handleWait(args); }},
        {"print", [this](const std::vector<std::string>& args) { handlePrint(args); }}
    };
}

void ScriptRunner::run(const std::vector<Command>& commands) {
    for (const auto& cmd : commands) {
        auto it = m_commandMap.find(cmd.command);
        if (it != m_commandMap.end()) {
            it->second(cmd.args);
        } else {
            throw std::runtime_error("Unknown command in ScriptRunner: " + cmd.command);
        }
    }
}

void ScriptRunner::handleClick(const std::vector<std::string>& args) {
    int x = std::stoi(args[0]);
    int y = std::stoi(args[1]);
    m_controller.handleInput(m_boardMapper.pixelWindowToCell(x, y));
}

void ScriptRunner::handleWait(const std::vector<std::string>& args) {
    int duration = std::stoi(args[0]);
    m_engine.wait(duration);
}

void ScriptRunner::handlePrint(const std::vector<std::string>& args) {
    if (args[0] == "board") {
        m_printer.print(m_engine.getSnapshot());
    }
}