#include "UnitTesting.h"
int main() {
    UnitTesting UnitTest;
    return UnitTest.runTests() == 0 ? 0 : 1;
}
