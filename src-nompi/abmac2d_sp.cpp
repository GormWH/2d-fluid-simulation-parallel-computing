/*
 * abmac2d_sp.cpp
 */
#include <CfdDriver_sp.h>
#include <iostream>

/*
 * ABMAC法のmain関数 シングルプロセス版
 * MPI版にチャレンジする前にこちらを完成させてください。
 *
 * 引数は計算条件ファイル名。
 */
int main(int argc, char *argv[]) {

    // シミュレータの最上位クラス
    CfdDriver_sp driver_sp;
    int rank = 0;
    int num_procs = 1;

    if (argc != 2) {
        std::cerr << "Usage : abmac2d_sp casefile\n";
        exit(1);
    }

    const char *filename = argv[1];

    // CfdDriver_spが上げる例外に備えて try 節で囲む
    try {

        /*
         * 準備処理
         */
        // (1) ドライバーの初期化。計算条件ファイルの読み込みとrankの記録。
        driver_sp.init(num_procs, rank, filename);
        // (2) データファイルの読み込み。
        driver_sp.readDataFile();
        driver_sp.initVariables();

        // (3) ループ不変量の計算
        driver_sp.calcInvariants();

        /*
         * 時間発展ループ
         */
        // 初期状態の設定
        // driver_sp.initVariables();
        // ループ
        driver_sp.timeLoop();

        /*
         * 後処理
         */
         // リスタート用
        driver_sp.outputVariables();
        // ドライバーの後処理。ログファイルを閉じる。
        driver_sp.finalize();
        // MPIの終了処理
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
