/*
 * TestBase.cpp
 */

#include <TestBase.h>
#include <cmath>

TestBase::~TestBase()
{
}

TestBase::TestBase()
{
    // テストの成績を記録する変数を初期化する
    test_count_ = 0;
    test_passed_count_ = 0;
    test_failed_count_ = 0;

    // 浮動小数点数の一致比較のしきい値を設定する。
    // デフォルト値が用途にそぐわない場合には、setToleranceで変えることもできる。
    tolerance_ = TESTBASE_DEFAULT_TOLERANCE; /* default tolerance */

    setOut(&std::cout);
}

void TestBase::setOut(std::ostream *pout)
{
    // 出力先のストリームを記録する。
    pout_ = pout;
}

void TestBase::setTolerance(double tolerance)
{
    tolerance_ = tolerance;
}

int TestBase::report()
{
    /*
     * テスト成績のレポートを出力する。
     */
    (*pout_) << "Test results : ";
    if (test_count_ == 0) {
        (*pout_) << "No tests were done." << std::endl;
        return 0;
    } else if (test_failed_count_ == 0) {
        (*pout_) << "All " << test_passed_count_ << " tests passed." << std::endl;
        return 0;
    } else {
        (*pout_) << test_count_ << " tests done, ";
        (*pout_) << test_passed_count_ << " passed, ";
        (*pout_) << test_failed_count_ << " failed." << std::endl;
        return 1;
    }
}

/*
 * Functions called from the above macros.
 * Do not call these functions directly.
 */
void TestBase::testTrueFunc(bool expression, const char *expression_source,
        const char *file, int line)
{
    test_count_++;
    (*pout_) << "[" << test_count_ <<"]";
    if (expression) {
        /* test passed */
        test_passed_count_++;
        (*pout_) << "PASSED : ";
    } else {
        /* test failed */
        test_failed_count_++;
        (*pout_) << "FAILED, true expected : ";
    }
    (*pout_) << "\"" << expression_source << "\" = " << expression << " at \"";
    (*pout_) << file << "\", (" << line << ")" << std::endl;
}

void TestBase::testFalseFunc(bool expression, const char *expression_source,
        const char *file, int line)
{
    test_count_++;
    (*pout_) << "[" << test_count_ <<"]";
    if (! expression) {
        /* test passed */
        test_passed_count_++;
        (*pout_) << "PASSED : ";
    } else {
        /* test failed */
        test_failed_count_++;
        (*pout_) << "FAILED, false expected : ";
    }
    (*pout_) << "\"" << expression_source << "\" = " << expression << " at \"";
    (*pout_) << file << "\", (" << line << ")" << std::endl;
}

void TestBase::testNotNullFunc(void *expression, const char *expression_source,
        const char *file, int line)
{
    test_count_++;
    (*pout_) << "[" << test_count_ <<"]";
    if (expression != NULL) {
        /* test passed */
        test_passed_count_++;
        (*pout_) << "PASSED : ";
    } else {
        /* test failed */
        test_failed_count_++;
        (*pout_) << "FAILED, non NULL expected : ";
    }
    (*pout_) << "\"" << expression_source << "\" = " << expression << " at \"";
    (*pout_) << file << "\", (" << line << ")" << std::endl;
}

void TestBase::testNullFunc(void *expression, const char *expression_source,
        const char *file, int line)
{
    test_count_++;
    (*pout_) << "[" << test_count_ <<"]";
    if (expression == NULL) {
        /* test passed */
        test_passed_count_++;
        (*pout_) << "PASSED : ";
    } else {
        /* test failed */
        test_failed_count_++;
        (*pout_) << "FAILED, NULL expected : ";
    }
    (*pout_) << "\"" << expression_source << "\" = " << expression << " at \"";
    (*pout_) << file << "\", (" << line << ")" << std::endl;
}

/* double version */
void TestBase::doubleEqualsFunc(double calculated_value, const char *calculated_source,
        double expected_value, const char *file, int line)
{
    test_count_++;
    (*pout_) << "[" << test_count_ <<"]";
    double error = fabs(calculated_value - expected_value);

    if (error < tolerance_) {
        /* test passed */
        test_passed_count_++;
        (*pout_) << "PASSED : ";
    } else {
        /* test failed */
        test_failed_count_++;
        (*pout_) << "FAILED, " << expected_value << " expected : ";
    }
    (*pout_) << "\"" << calculated_source << "\" = " << calculated_value;
    (*pout_) << " at \"" << file << "\", (" << line << ")" << std::endl;
}

/* int version */
void TestBase::intEqualsFunc(int calculated_value, const char *calculated_source,
        int expected_value, const char *file, int line)
{
    test_count_++;
    (*pout_) << "[" << test_count_ <<"]";

    if (calculated_value == expected_value) {
        /* test passed */
        test_passed_count_++;
        (*pout_) << "PASSED : ";
    } else {
        /* test failed */
        test_failed_count_++;
        (*pout_) << "FAILED, " << expected_value << " expected : ";
    }
    (*pout_) << "\"" << calculated_source << "\" = " << calculated_value;
    (*pout_) << " at \"" << file << "\", (" << line << ")" << std::endl;
}

/* size_t version */
void TestBase::sizeEqualsFunc(size_t calculated_value, const char *calculated_source,
        size_t expected_value, const char *file, int line)
{
    test_count_++;
    (*pout_) << "[" << test_count_ <<"]";

    if (calculated_value == expected_value) {
        /* test passed */
        test_passed_count_++;
        (*pout_) << "PASSED : ";
    } else {
        /* test failed */
        test_failed_count_++;
        (*pout_) << "FAILED, " << expected_value << " expected : ";
    }
    (*pout_) << "\"" << calculated_source << "\" = " << calculated_value;
    (*pout_) << " at \"" << file << "\", (" << line << ")" << std::endl;
}

/* pointer version */
void TestBase::ptrEqualsFunc(void *calculated_value, const char *calculated_source,
        void *expected_value, const char *file, int line)
{
    test_count_++;
    (*pout_) << "[" << test_count_ <<"]";

    if (calculated_value == expected_value) {
        /* test passed */
        test_passed_count_++;
        (*pout_) << "PASSED : ";
    } else {
        /* test failed */
        test_failed_count_++;
        (*pout_) << "FAILED, " << std::hex << (unsigned long) expected_value << " expected : ";
    }
    (*pout_) << "\"" << calculated_source << "\" = ";
    (*pout_) << std::hex << (unsigned long) calculated_value << std::dec;
    (*pout_) << " at \"" << file << "\", (" << line << ")" << std::endl;
}

/* VectorXY version */
void TestBase::xyEqualsFunc(const VectorXY &calculated_value, const char *calculated_source,
        const VectorXY &expected_value, const char *file, int line)
{
    test_count_++;
    (*pout_) << "[" << test_count_ <<"]";

    VectorXY diff = calculated_value - expected_value;
    if (diff.norm() < tolerance_) {
        /* test passed */
        test_passed_count_++;
        (*pout_) << "PASSED : ";
    } else {
        /* test failed */
        test_failed_count_++;
        (*pout_) << "FAILED, " << expected_value << " expected : ";
    }
    (*pout_) << "\"" << calculated_source << "\" = ";
    (*pout_) << calculated_value << " at \"" << file << "\", (" << line << ")" << std::endl;
}
