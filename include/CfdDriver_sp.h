/*
 * CfdDriver_sp.h
 */

#ifndef CFDDRIVER_SP_H_
#define CFDDRIVER_SP_H_

#include <Params.h>
#include <State.h>
#include <CfdProcData.h>
#include <IoException.h>
#include <DataException.h>

/*
 * ABMAC法(流速圧力同時緩和法)に基づく時間発展計算の進行を制御するクラス
 *
 * 当クラスのインスタンスを作成し、メソッドを順番に呼び出すことにより、
 * シミュレーション計算を進める。
 */
class CfdDriver_sp {
    // 計算条件クラス
    Params params_;

    // 計算経過クラス
    State state_;

    // 1プロセス分の計算データを保持するクラス
    CfdProcData procData_;

    // 通信バッファクラス
    CfdCommData commData_;

    // 通信処理クラスは無し

public:

    ~CfdDriver_sp();

    CfdDriver_sp();

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

    // リスタートファイルの読み込みまたは0で初期化
    void initVariables();

    // ループ不変量を計算する
    void calcInvariants();

    // 時間発展ループ
    void timeLoop();

    // 時間発展を1ステップ実行する
    void doStep();

    // リスタートファイルの書き出し
    void outputVariables();

    // シミュレーション終了時の後処理。
    // ログファイルのクローズなど。
    // MPIのfinalizeはこの後で呼ぶこと。
    void finalize();

private:

    // 速度の補正ループ
    void correctVelocity();

};

#endif /* CFDDRIVER_SP_H_ */
