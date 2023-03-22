/*
 * TestBase.h
 */

#ifndef TESTBASE_H_
#define TESTBASE_H_

#include <iostream>
#include <VectorXY.h>

/*
 * 単体テスト用のクラスの親クラス
 *
 * このクラスを継承して単体テスト用クラスを作成することにより、
 * テストの実装が多少楽になる。利用例としては test_VectorXY.cppを参照。
 *
 * 本クラスでは値の比較メソッド（int_equals(計算値,期待値)など）を提供するほか
 * テストの成功・失敗の個数を記録し、テスト成績をレポート（表示）する機能を提供する。
 *
 * 派生クラスにおいては、最後に report()メソッドを呼ぶこと。
 *
 * 比較メソッドの成功・失敗などの出力は、デフォルトでは標準出力に向けられるが、
 * setOutメソッドにより、ファイルを指定することもできる。
 *
 * double値の一致比較には許容誤差を用いた範囲比較を行う。許容誤差はsetToleranceで変更できる。
 * デフォルトの許容誤差は 1.0e-9
 */

#define TESTBASE_DEFAULT_TOLERANCE 1.0e-9

class TestBase {

    // テスト結果の出力先ストリーム
    std::ostream *pout_;

    // 総テスト数
    int test_count_;

    // 成功ケース数
    int test_passed_count_;

    // 失敗ケース数
    int test_failed_count_;

    // 許容誤差
    double tolerance_;

public:

    ~TestBase();

    TestBase();

    // 出力先ストリームを設定する。指定しなければ std::cout が用いられる
    void setOut(std::ostream *pout);

    // 許容誤差を設定する
    void setTolerance(double tolerance);

    /*
     * テストレポートを出力する。テスト何件中何件成功したか表示される。
     */
    int report();

    /*
     * テスト用マクロから呼ばれるメソッド群
     * これらの関数を直接呼び出さないこと。
     */
    void testTrueFunc(bool expression, const char *expression_source,
            const char *file, int line);

    void testFalseFunc(bool expression, const char *expression_source,
            const char *file, int line);

    void testNotNullFunc(void *expression, const char *expression_source,
            const char *file, int line);

    void testNullFunc(void *expression, const char *expression_source,
            const char *file, int line);

    void doubleEqualsFunc(double calculated_value, const char *calculated_source,
            double expected_value, const char *file, int line);

    void intEqualsFunc(int calculated_value, const char *calculated_source,
            int expected_value, const char *file, int line);

    void sizeEqualsFunc(size_t calculated_value, const char *calculated_source,
            size_t expected_value, const char *file, int line);

    void ptrEqualsFunc(void *calculated_value, const char *calculated_source,
            void *expected_value, const char *file, int line);

    void xyEqualsFunc(const VectorXY &calculated_value, const char *calculated_source,
            const VectorXY &expected_value, const char *file, int line);
};

/*
 * 派生クラスで利用できるマクロの定義
 *
 */

/*
 * test_true(式) 式がtrue(non zero)であることを確認する
 */
#define test_true(EXPRESSION) \
        testTrueFunc(EXPRESSION, #EXPRESSION, __FILE__, __LINE__)

/*
 * test_false(式) 式がfalse(zero)であることを確認する
 */
#define test_false(EXPRESSION) \
        testFalseFunc(EXPRESSION, #EXPRESSION, __FILE__, __LINE__)

/*
 * test_non_null(ポインタ式) ポインタ式が非NULLであることを確認する
 */
#define test_not_null(EXPRESSION) \
        testNotNullFunc(EXPRESSION, #EXPRESSION, __FILE__, __LINE__)

/*
 * test_null(ポインタ式) ポインタ式がNULLであることを確認する
 */
#define test_null(EXPRESSION) \
        testNullFunc(EXPRESSION, #EXPRESSION, __FILE__, __LINE__)

/*
 * dbl_equals(計算値,期待値) doubleの計算値が期待値（正解の値）に、十分近いことを確認する
 */
#define dbl_equals(EXPRESSION, EXPECTED_VALUE) \
        doubleEqualsFunc(EXPRESSION, #EXPRESSION, EXPECTED_VALUE, __FILE__, __LINE__)

/*
 * int_equals(計算値,期待値) intの計算値が期待値（正解の値）と一致することを確認する
 */
#define int_equals(EXPRESSION, EXPECTED_VALUE) \
        intEqualsFunc(EXPRESSION, #EXPRESSION, EXPECTED_VALUE, __FILE__, __LINE__)

/*
 * size_equals(計算値,期待値) size_tの計算値が期待値（正解の値）と一致することを確認する
 */
#define size_equals(EXPRESSION, EXPECTED_VALUE) \
        sizeEqualsFunc(EXPRESSION, #EXPRESSION, EXPECTED_VALUE, __FILE__, __LINE__)

/*
 * ptr_equals(計算値,期待値) ポインタの計算値が期待値（正解の値）と一致することを確認する
 */
#define ptr_equals(EXPRESSION, EXPECTED_VALUE) \
        ptrEqualsFunc(EXPRESSION, #EXPRESSION, EXPECTED_VALUE, __FILE__, __LINE__)

/*
 * xy_equals(計算値,期待値) VectorXY型の計算値が期待値（正解の値）と一致することを確認する
 */
#define xy_equals(EXPRESSION, EXPECTED_VALUE) \
        xyEqualsFunc(EXPRESSION, #EXPRESSION, EXPECTED_VALUE, __FILE__, __LINE__)

#endif /* TESTBASE_H_ */
