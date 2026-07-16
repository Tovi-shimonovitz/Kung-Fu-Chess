#include <iostream>
#include <cassert>
#include "../../include/model/Position.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

void testPositionEquality() {
    Position pos1(3, 4);
    Position pos2(3, 4);
    assert(pos1 == pos2);
    log_test("testPositionEquality");
}

void testPositionInequality() {
    Position pos1(3, 4);
    Position pos2(5, 4);
    Position pos3(3, 1);
    assert(!(pos1 == pos2));
    assert(!(pos1 == pos3));
    log_test("testPositionInequality");
}

void testPositionToString() {
    Position pos(2, 5);
    std::string expected = "(row: 2, col: 5)";
    assert(pos.toString() == expected);
    log_test("testPositionToString");
}

void testPositionToStringNegative() {
    Position pos(-1, -2);
    std::string expected = "(row: -1, col: -2)";
    assert(pos.toString() == expected);
    log_test("testPositionToStringNegative");
}

void testPositionOrderingByRow() {
    Position lower(2, 9);
    Position higher(3, 0);
    assert(lower < higher);
    assert(!(higher < lower));
    log_test("testPositionOrderingByRow");
}

void testPositionOrderingByColWhenRowsEqual() {
    Position lower(4, 1);
    Position higher(4, 2);
    assert(lower < higher);
    assert(!(higher < lower));
    log_test("testPositionOrderingByColWhenRowsEqual");
}

void testPositionOrderingEqualIsNotLess() {
    Position a(4, 4);
    Position b(4, 4);
    assert(!(a < b));
    assert(!(b < a));
    log_test("testPositionOrderingEqualIsNotLess");
}

void testPositionDefaultConstructible() {
    Position pos;
    (void)pos;
    log_test("testPositionDefaultConstructible");
}

int main() {
    testPositionEquality();
    testPositionInequality();
    testPositionToString();
    testPositionToStringNegative();
    testPositionOrderingByRow();
    testPositionOrderingByColWhenRowsEqual();
    testPositionOrderingEqualIsNotLess();
    testPositionDefaultConstructible();
    std::cout << "All Position tests passed successfully!" << std::endl;
    return 0;
}
