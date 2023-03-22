/*
 * abmac2d.cpp
 */
#include <CfdDriver.h>
#include <iostream>

#include <mpi.h>

/*
 * ABMAC法のmain関数
 *
 * MPIの制御下で実行する。直接シェルから起動することはできない。
 * run command
 * mpirun -np 16 abmac2d <casefile>
 * MPIの起動時のプロセス数は、計算対象データと整合している必要がある。
 *
 * 引数は計算条件ファイル名。
 */
int main(int argc, char *argv[]) {

    // シミュレータの最上位クラス
    CfdDriver driver;
    int rank;
    int num_procs;
    double real_time_start;

    if (argc != 2) {
        std::cerr << "Usage : abmac2d casefile\n";
        exit(1);
    }

    const char *filename = argv[1];

    // CfdDriverが上げる例外に備えて try 節で囲む
    try {
        // MPIを初期化する
        MPI_Init(&argc, &argv);

        real_time_start = MPI_Wtime();

        // 自身のrankを取得する
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        // 総プロセス数を取得する
        MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

        /*
         * 準備処理
         */
        // (1) ドライバーの初期化。計算条件ファイルの読み込みとrankの記録。
        driver.init(num_procs, rank, filename);

        driver.tryMain(real_time_start);

        // MPIの終了処理
        MPI_Finalize();
    } catch (DataException &exp) {
        std::cerr << exp << std::endl;
        exit(1);
    } catch (IoException &exp) {
        std::cerr << exp << std::endl;
        exit(1);
    }

    // 正常終了
    return 0;
}
