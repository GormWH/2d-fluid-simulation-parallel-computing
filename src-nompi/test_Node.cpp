/*
 * test_Node.cpp
 */

#include <TestBase.h>
#include <Node.h>

class TestNode : public TestBase {

    /* test target */
    Node node_;

public:

    void run();
    void testRank();
    void testMass();
};

void TestNode::testRank()
{
    int_equals(node_.first_rank_, -1);
    size_equals(node_.ranks_.size(), 0);

    node_.addRank(1);
    int_equals(node_.first_rank_, 1);
    size_equals(node_.ranks_.size(), 0);
    test_true(node_.isOnRank(1));
    test_false(node_.isOnRank(2));
    test_false(node_.isOnBoundary());

    node_.addRank(2);
    int_equals(node_.first_rank_, 1);
    size_equals(node_.ranks_.size(),2);
    int_equals(node_.ranks_[0], 1);
    int_equals(node_.ranks_[1], 2);
    test_true(node_.isOnRank(1));
    test_true(node_.isOnRank(2));
    test_true(node_.isOnBoundary());

}

void TestNode::testMass()
{
    node_.m_ = 2.0;
    node_.calcInvMass();
    dbl_equals(node_.inv_m_, 1/2.0);
}

void TestNode::run()
{
    testRank();
    testMass();
}

int main(int argc, char *argv[])
{
    TestNode test;
    test.run();
    return test.report();
}
