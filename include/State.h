/*
 * State.h
 */
#ifndef _STATE_H
#define _STATE_H

/*
 * 計算の経過を保持するクラス
 */
class State {
    friend class TestState;

private:

    // 無次元時間での現在時刻
    double t_;

    // 時間発展の回次（何ラウンド目か）
    int round_;

public:

    // 時刻と回次を0に設定する
    void reset();

    // 現在の時刻を取得する
    double getT();

    // 現在の回次を取得する
    int getRound();

    // 次の回次に進める
    void nextRound(double delta_t);

    void initT(double t, double delta_t);

};

#endif
