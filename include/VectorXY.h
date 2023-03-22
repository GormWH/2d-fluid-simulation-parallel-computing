/*
 * VectorXY.h
 */
#ifndef _VECTORXY_H
#define _VECTORXY_H

#include <iostream>
#include <cmath>

/*
 * 二次元ベクトルクラス
 */
class VectorXY {
public:

    // 保持するx,y成分。
    // publicなので直接アクセス可能。
    double x_, y_;

    VectorXY() { }

    // コピーコンストラクタ
    VectorXY(const VectorXY &o) {
        x_ = o.x_;
        y_ = o.y_;
    }

    // 成分を指定するコンストラクタ
    VectorXY(double x, double y) {
        x_ = x;
        y_ = y;
    }

    // 代入
    VectorXY &operator=(const VectorXY &o) {
        x_ = o.x_;
        y_ = o.y_;
        return *this;
    }

    // 引き算
    VectorXY operator-(const VectorXY &o) const {
        return VectorXY(x_ - o.x_, y_ - o.y_);
    }

    // 成分を指定した代入
    void set(double x, double y) {
        x_ = x;
        y_ = y;
    }

    // ノルム
    double norm() const {
        return sqrt(x_*x_ + y_*y_);
    }
};

// ストリームにベクトルの中身を出力するためのオペレータ
inline static std::ostream & operator<<(std::ostream &os, const VectorXY &vec) {
    os << "(" << vec.x_ << "," << vec.y_ << ")";
    return os;
}

#endif
