/*
 * Node.h
 */

#ifndef NODE_H_
#define NODE_H_

#include <cstdlib>
#include <cassert>
#include <vector>
#include <algorithm>
#include <VectorXY.h>

/*
 * 節点の情報を保持するクラス
 */
class Node {
public:

    /*
     * [part 1] 節点の入力ファイルから直接的に定まる情報
     */

    // 入力ファイルに指定されている、全領域通しての節点番号(0はじまり)
    int global_index_;

    // 自身のrankの中でのローカルな節点番号(0はじまり)
    int local_index_;

    // 座標
    VectorXY pos_;

    // この節点が属しているrank（プロセスのrank == 領域分割番号)
    // 節点がどのrankに属しているのかは、入力ファイルの中で四角形領域単位で
    // 指定されている領域番号に基づいて調べる必要がある。全ての四角形領域
    // について、４隅の節点に領域番号を記録していく。節点の側では渡された
    // 領域番号を以下の変数に記録する。一つの節点がいくつの領域に属するのかは
    // 可変であり、典型的には一つの節点の周囲に４つの四角形要素が存在し、
    // それらの領域番号は同一である。その場合には first_rank_ だけが領域番号を
    // 保持する。二つ以上の領域にまたがる節点の場合には、全ての領域番号を
    // ranks_ に保持する。典型ケースにおいてはranks_を空に保つことによって
    // std::vectorが動的メモリ割り当てに及ばずに済むように配慮している。
    int first_rank_; // まだ一つも領域番号が指定されていない時の値は -1
    std::vector<int> ranks_;

    /*
     * [part 2] ループ不変量
     */
    // 集中化近似された質量
    double m_;
    // 質量の逆数
    double inv_m_;
    double delta_t_by_m_;

    /*
     * [part 3] 状態変数(時間と共に変化する値や、計算のための中間的な値）
     */
    // 節点での速度, 速度補正
    VectorXY vel_;
    VectorXY d_vel_;

    // コンストラクタ
    Node() {
        // 質量への代入は += で周囲から加算されていくので、0に初期化する必要がある。
        m_ = 0;
        // 速度の補正, m_ と同様に += で加算される．
        d_vel_.x_ = 0;
        d_vel_.y_ = 0;
        // この節点には領域番号が一つも設定されていないことを示す。
        first_rank_ = -1;
    }

    // コピーコンストラクタ
    // Nodeクラスは CfdProcDataの中でstd::vectorに格納されるので、コピーコンストラクタの定義が必須。
    Node(const Node &o) : ranks_(o.ranks_) {
        // 一般のコピーコンストラクタでは、全メンバ変数をコピーするが、
        // ここでは、処理手順を考慮して、コピーが必須のメンバ変数だけ、コピーする。
        m_ = o.m_;
        pos_ = o.pos_;
        vel_ = o.vel_;
        first_rank_ = o.first_rank_;
    }

    // 節点が属する領域番号を記録する。
    // 引数は領域番号(0はじまり)
    // 重複を排除して記録する。すなわち、初めて渡される領域番号を記録していき、
    // すでに登録済みの領域番号は追記しない。
    void addRank(int rank) {
        if (first_rank_ == rank) {
            /* 登録済みである */
            return;
        } else if (std::find(ranks_.begin(), ranks_.end(), rank) < ranks_.end()) {
            /* ranks_に存在するので登録済みである */
            return;
        } else if (first_rank_ == -1) {
            /* 今回のaddRankが初めてのaddRankであるので、何が来ても記録する */
            first_rank_ = rank;
        } else if (ranks_.size() > 0) {
            /* 二回め以降のaddRankで新しい値である。すでにranks_を使用中である。*/
            /* この場合は、ranks_に追加する。 */
            ranks_.push_back(rank);
        } else {
            /* 二回め以降のaddRankで新しい値である。ranks_は空である。*/
            /* この場合は、ranks_にfirst_rank_を追加した上で、今回の値も追加する */
            ranks_.push_back(first_rank_);
            ranks_.push_back(rank);
        }
    }

    // この節点が特定の領域に属すればtrueを返す。
    bool isOnRank(int rank) const {
        if (first_rank_ == rank) {
            return true;
        } else if (std::find(ranks_.begin(), ranks_.end(), rank) < ranks_.end()) {
            return true;
        }
        return false;
    }

    // この節点が複数の領域に属すればtrueを返す。
    bool isOnBoundary() const {
        return ranks_.size() > 1;
    }

    // 質量をゼロでクリアする。
    // ※コンストラクタでもやっているので、冗長な処理。
    void clearMass() {
        m_ = 0;
    }

    // 集中化質量が計算された後で、その逆数を求めて記録する。
    void calcInvMass() {
        assert(m_ > 0.0);
        inv_m_ = 1.0 / m_;
    }

    void calcDtByM(double delta_t){
        delta_t_by_m_ = delta_t * inv_m_;
    }

    void clearVelocityDelta(){
        d_vel_.x_ = 0;
        d_vel_.y_ = 0;
    }

    void applyVelocityDelta(){
        // 変化量の適用
        vel_.x_ += d_vel_.x_;
        vel_.y_ += d_vel_.y_;
    }

};

#endif
