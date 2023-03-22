/*
 * CfdDriver.h
 */

#ifndef CFDDRIVER_H_
#define CFDDRIVER_H_

#include <Params.h>
#include <State.h>
#include <CfdProcData.h>
#include <CfdCommunicator.h>
#include <IoException.h>
#include <DataException.h>

/*
 * ABMAC法(流速圧力同時緩和法)に基づく時間発展計算の進行を制御するクラス
 *
 * 当クラスのインスタンスを作成し、メソッドを順番に呼び出すことにより、
 * シミュレーション計算を進める。
 */
class CfdDriver {
    // 計算条件クラス
    Params params_;

    // 計算経過クラス
    State state_;

    // 1プロセス分の計算データを保持するクラス
    CfdProcData procData_;

    // 通信処理を実行するクラス
    CfdCommunicator communicator_;

    // 通信バッファクラス
    CfdCommData commData_;

    int correctVelocityCounter_;

public:

    ~CfdDriver();

    CfdDriver();

    Params *getParams() {
        return &params_;
    }

    State *getState() {
        return &state_;
    }

    // 初期化処理。
    // ログファイルを開き、計算条件ファイルを読み込む。
    // MPIの初期化はこの手前で済ませること。
    // np : 総プロセス数
    // rank : 自身のrank番号
    // filename : 計算条件ファイルのパス名
    // 例外:
    //   IoException : ファイルが読めない場合
    //   DataException : ファイルの内容に問題がある場合
    void init(int np, int rank, const char *filename);

    // データファイルを読み込む。
    // 計算条件ファイルに記載された、メッシュファイルと境界条件ファイルを読み込む。
    // 例外：
    //   IoException : ファイルが読めない場合
    //   DataException : ファイルの内容に問題がある場合
    void readDataFile();

    // リスタートファイルがあれば変数に読み込み，なければ0で初期化
    void initVariables();

    // // 時間発展
    // // 速度、圧力を0に初期化する
    // void clearFieldData();

    // ループ不変量を計算する
    void calcInvariants();

    // 時間発展ループ
    void timeLoop(double real_time_start);

    // リスタートファイルの書き出し
    void outputVariables();

    // 速度、圧力を更新し、周期的に状態を出力ファイルへ書き出す
    void doStep();

    // シミュレーション終了時の後処理。
    // ログファイルのクローズなど。
    // MPIのfinalizeはこの後で呼ぶこと。
    void finalize();

    // abmac2d main 内　try
    void tryMain(double real_time_start);

private:

    // 速度の補正ループ
    void correctVelocity();

};

#endif /* CFDDRIVER_H_ */
