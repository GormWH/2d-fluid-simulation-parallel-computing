/*
 * test_State.cpp
 */

#include <TestBase.h>
#include <State.h>

class TestState : public TestBase {

    // test target
    State st_;

public:
    void run();
};

void TestState::run()
{
    st_.round_ = 100;
    st_.t_ = 200.0;

    st_.reset();
    int_equals(st_.getRound(), 0);
    dbl_equals(st_.getT(), 0);
}

int main(int argc, char *argv[])
{
    TestState test;
    test.run();
    return test.report();
}
