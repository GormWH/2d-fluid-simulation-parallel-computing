/*
 * State.cpp
 */
#include <State.h>

void State::reset() {
    // 時間発展の回数とシミュレーション上の経過時間の変数を初期化する。
    t_ = 0;
    round_ = 0;
}

double State::getT() {
    return t_;
}

int State::getRound() {
    return round_;
}

void State::nextRound(double delta_t) {
    // 時間発展の回数と時刻を1ステップ分進める。
    t_ += delta_t;
    round_++;
}

void State::initT(double t, double delta_t) {
    // リスタートの時にシミュレーション上の経過時間の変数を設定する
    t_ = t;
    // 時間発展の回数を設定する．誤差で多少ずれるかもしれないが気にしない．誤差なので．
    round_ = static_cast<int>(t_/delta_t);
}
