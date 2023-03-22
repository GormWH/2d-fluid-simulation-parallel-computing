/*
 * test_Params.cpp
 */

#include <TestBase.h>
#include <Params.h>

class TestParams : public TestBase {

    // テスト対象
    Params par_;

public:
    void run();
    void setup();
    void test();
};

void TestParams::setup()
{
    par_.init(0, 4, "testdata/params/case.txt");
}

void TestParams::test()
{
    // investigate results.
    dbl_equals(par_.re_, 10);
}

void TestParams::run()
{
    setup();
    test();
}


int main(int argc, char *argv[])
{
    TestParams test;
    try {
        test.run();
    } catch (IoException &exp) {
        std::cout << exp << std::endl;
    } catch (DataException &exp) {
        std::cout << exp << std::endl;
    }
    return test.report();
}
