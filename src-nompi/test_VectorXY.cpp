/*
 * test_VectorXY.cpp
 */

#include <TestBase.h>
#include <VectorXY.h>

class TestVectorXY : public TestBase {

    // テスト対象オブジェクトがシンプルなのでメンバ変数にはせず、テストメソッド内で作成する。

public:
    void run();
};

void TestVectorXY::run()
{
    VectorXY v1(1,2);
    dbl_equals(v1.x_, 1.0);
    dbl_equals(v1.y_, 2.0);
}

int main(int argc, char *argv[])
{
    TestVectorXY test;
    test.run();
    return test.report();
}
