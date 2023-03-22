/*
 * test_Boundary.cpp
 */

#include <TestBase.h>
#include <Boundary.h>

/*
 * 境界条件クラス用の単体テストプログラム
 */
class TestBoundary : public TestBase {


    // テスト対象のオブジェクト
    Boundary bd_;
    // 動作させるために必要な最低限のオブジェクト
    Node nodes_[4];

public:
    void setup();
    void testApply();
    void run();
};

void TestBoundary::setup()
{
    // テスト用の座標を指定する
    nodes_[0].pos_.set(0,0);
    nodes_[1].pos_.set(1,0);
    nodes_[2].pos_.set(2,1);
    nodes_[3].pos_.set(3,1);
    bd_.addNode(&nodes_[0]);
    bd_.addNode(&nodes_[1]);
    bd_.addNode(&nodes_[2]);
    bd_.addNode(&nodes_[3]);
    // 速度を定める多項式の係数
    bd_.a0_ = 6.0;
    bd_.a1_ = 5.0;
    bd_.a2_ = 4.0;
    bd_.a3_ = 3.0;
    bd_.a4_ = 2.0;
    bd_.a5_ = 1.0;
    bd_.b0_ = 0.6;
    bd_.b1_ = 0.5;
    bd_.b2_ = 0.4;
    bd_.b3_ = 0.3;
    bd_.b4_ = 0.2;
    bd_.b5_ = 0.1;
}

void TestBoundary::testApply()
{
    bd_.apply(1.0, 1.0);
    // 計算された速度を確認する (TestBase.hを参照)
    xy_equals(nodes_[0].vel_, VectorXY(6.0, 0.6));
}

void TestBoundary::run()
{
    setup();
    testApply();
}

int main(int argc, char *argv[])
{
    TestBoundary test;
    test.run();
    return test.report();
}
