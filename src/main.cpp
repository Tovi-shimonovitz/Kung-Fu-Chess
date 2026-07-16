#pragma once
#include <iostream>
#include <memory>
#include "engine/GameEngine.h"
#include "input/controller.h"
#include "io/board_printer.h"
#include "texttests/script_parser.h"
#include "texttests/script_runner.h"

int main() {
    try {
        ScriptData script_data = ScriptParser::parse(std::cin);
        GameEngine engine; 
        Controller controller(engine); 
        BoardPrinter printer;

        engine.setBoard(std::move(script_data.board));

        ScriptRunner runner(engine, controller, printer);
        runner.run(script_data.commands);

      
    } catch (const std::exception& e) {
        std::cerr <<  e.what() << std::endl;
        return 1; 
    }

    return 0;
}