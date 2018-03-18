#include <iostream>
#include "gtest/gtest.h"

GTEST_API_ int main(int argc, char **argv) {
    std::cout << "Start running tests" << std::endl;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
