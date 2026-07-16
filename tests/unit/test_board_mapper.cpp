#include <cassert>
#include <iostream>
#include "../../include/input/board_mapper.h"
#include "../../include/model/Position.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

void test_pixel_to_cell_basic_mapping() {
    Position pos = BoardMapper::pixelToCell(0, 0);
    assert(pos.row == 0 && pos.col == 0);
    log_test("test_pixel_to_cell_basic_mapping");
}

void test_pixel_to_cell_middle_of_cell() {
    Position pos = BoardMapper::pixelToCell(150, 250);
    assert(pos.row == 2 && pos.col == 1);
    log_test("test_pixel_to_cell_middle_of_cell");
}

void test_pixel_to_cell_boundary_conditions() {
    Position pos1 = BoardMapper::pixelToCell(100, 100);
    assert(pos1.row == 1 && pos1.col == 1);

    Position pos2 = BoardMapper::pixelToCell(99, 99);
    assert(pos2.row == 0 && pos2.col == 0);

    log_test("test_pixel_to_cell_boundary_conditions");
}

void test_pixel_to_cell_negative_values() {
    Position pos = BoardMapper::pixelToCell(-1, -1);
    assert(pos.row < 0 && pos.col < 0);
    log_test("test_pixel_to_cell_negative_values");
}

void test_pixel_to_cell_negative_exact_multiple() {
    Position pos = BoardMapper::pixelToCell(-100, -200);
    assert(pos.row == -2 && pos.col == -1);
    log_test("test_pixel_to_cell_negative_exact_multiple");
}

int main() {
    std::cout << "--- Starting BoardMapper Unit Tests ---" << std::endl;

    test_pixel_to_cell_basic_mapping();
    test_pixel_to_cell_middle_of_cell();
    test_pixel_to_cell_boundary_conditions();
    test_pixel_to_cell_negative_values();
    test_pixel_to_cell_negative_exact_multiple();

    std::cout << "--- All BoardMapper tests passed successfully ---" << std::endl;
    return 0;
}
