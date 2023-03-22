/*
 * test_CfdProcData.cpp
 */

#include <TestBase.h>
#include <CfdProcData.h>


class TestCfdProcData : public TestBase {

    // テスト対象のオブジェクト
    CfdProcData procData_;

    // テスト対象を動作させるために必要となるオブジェクト
    Params params_;
    State state_;
    CfdCommData commData_;

public:
    void setup();
    void test();
    void run();
};

void TestCfdProcData::setup()
{
    params_.init(4, 0, "testdata/cfdprocdata/case.txt");
    int_equals(params_.my_rank_, 0);
    state_.reset();
    commData_.init(&params_, &state_);
    procData_.init(&params_, &state_, &commData_);
    procData_.readMeshFile();
    procData_.readBoundaryFile();
    procData_.findOwnData();
}

void TestCfdProcData::test()
{
    // check that file is read correctly.
    size_equals(procData_.elements_.size(), 16);
    size_equals(procData_.my_elements_.size(), 4);
    size_equals(procData_.nodes_.size(), 25);
    size_equals(procData_.my_nodes_.size(), 9);
    xy_equals(procData_.my_nodes_[0]->pos_, VectorXY(0, 0));
    xy_equals(procData_.my_nodes_[1]->pos_, VectorXY(1, 0));

    procData_.calcInvariants1();
}

void TestCfdProcData::run()
{
    setup();
    test();
}

int main(int argc, char *argv[])
{
    TestCfdProcData test;
    try {
        test.run();
    } catch (IoException &exp) {
        std::cout << exp << std::endl;
    } catch (DataException &exp) {
        std::cout << exp << std::endl;
    }
    return test.report();
}
