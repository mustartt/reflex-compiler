//
// Created by henry on 2022-03-25.
//

#include <gtest/gtest.h>

TEST(SanityTest, SanityUnitTest) {
    double res = 1.0 + 2.0;
    ASSERT_NEAR(res, 3.0, 1.0e-11);
}
