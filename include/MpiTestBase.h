/*
 * MpiTestBase.h
 */

#ifndef MPITESTBASE_H_
#define MPITESTBASE_H_

#include <TestBase.h>
#include <Logger.h>

#include <mpi.h>

/*
 * MPIを利用する並列の単体テスト用の親クラス。
 *
 * 単体テストプログラムでは、テスト対象のクラスごとにこの派生クラスを作成し、
 * そのクラスの変数（オブジェクト）をmain関数の中で作成して、そのメソッドを
 * 呼ぶことでテストを行う。
 *
 * 本クラスはTestBaseから継承するテスト用のメソッド dbl_equals などに加えて
 * MPIの初期化・終了処理と、Loggerクラスによるログファイルの初期化と終了処理を
 * 実装する。
 *
 * 使用例としては test_CfdCommunicator.cpp を参照。
 */
class MpiTestBase : public TestBase { // 親クラスがTestBaseであることに注意


public:
    // 自身のrank。 initMpiの中で設定される。
    int my_rank_;

    // 総プロセス数
    int num_procs_;

    // デストラクタ。この中でMPI_Finalizeとログファイルのクローズをする。
    ~MpiTestBase();

    MpiTestBase();

    // 初期化処理。この中でMPI_Initやrankの取得、ログファイルのオープンをする。
    void initMpi(int argc, char *argv[]);
};



#endif /* MPITESTBASE_H_ */
