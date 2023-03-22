/*
 * test_QuadElement.cpp
 */

#include <TestBase.h>
#include <QuadElement.h>
#include <Matrix4.h>
#include <Vector4.h>

class TestQuadElement : public TestBase {

    /*
     * Test target
     */
    QuadElement elem_;

    /*
     * Requisite objects to make test target operate.
     */
    Node nodes_[4];

public:
    void setup();
    // test calcInvariants1
    void testMass();
    void testDiffusionMatrix();
    void testHxy();
    void testSize();

    // test calcInvariants2
    void testDtHxByM();
    void testLambda();

    void run();
};

void TestQuadElement::setup()
{
    /* setup */
    nodes_[0].pos_.set(0,-2);
    nodes_[1].pos_.set(2,-2);
    nodes_[2].pos_.set(2,0);
    nodes_[3].pos_.set(0,0);

    elem_.nodes_[0] = &nodes_[0];
    elem_.nodes_[1] = &nodes_[1];
    elem_.nodes_[2] = &nodes_[2];
    elem_.nodes_[3] = &nodes_[3];

}


void TestQuadElement::testMass()
{
    dbl_equals(nodes_[0].m_, 1.0);
    dbl_equals(nodes_[1].m_, 1.0);
    dbl_equals(nodes_[2].m_, 1.0);
    dbl_equals(nodes_[3].m_, 1.0);
}

void TestQuadElement::testDiffusionMatrix()
{
    dbl_equals(elem_.d_.get(0,0), 2.0/3.0);
    dbl_equals(elem_.d_.get(0,1), -1.0/6.0);
    dbl_equals(elem_.d_.get(0,2), -1.0/3.0);
    dbl_equals(elem_.d_.get(0,3), -1.0/6.0);
}

void TestQuadElement::testHxy()
{
    Vector4 hx, hy;
    int i;
    hx.set(0,-1);
    hx.set(1,1);
    hx.set(2,1);
    hx.set(3,-1);
    hy.set(0,-1);
    hy.set(1,-1);
    hy.set(2,1);
    hy.set(3,1);
    for(int i = 0; i < 4; i++){
        dbl_equals(elem_.hx_.get(i), hx.get(i));
        dbl_equals(elem_.hy_.get(i), hy.get(i));
    }
}

void TestQuadElement::testSize(){
    double size = 4;
    dbl_equals(elem_.size_, size);
}

void TestQuadElement::run()
{
    double Re = 1;
    double delta_t = 0.1;
    double relaxation = 0.5;
    setup();
    elem_.calcInvariants1(Re);
    elem_.calcInvariants2(delta_t,relaxation);
    testMass();
    testDiffusionMatrix();
    testHxy();
    testSize();
}

int main(int argc, char *argv[])
{
    TestQuadElement test;
    test.run();
    return test.report();
}
