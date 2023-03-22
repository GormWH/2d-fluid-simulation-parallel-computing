/*
 * Boundary.h
 */

#ifndef _BOUNDARY_H
#define _BOUNDARY_H

#include <Node.h>
#include <vector>

/*
 * 境界条件の定義を保持するクラス
 *
 * 境界条件上の節点には速度を指定することができるが、節点の座標に応じて
 * 変化する速度を指定できるように、座標の2次多項式の係数として、速度を定義する。
 * 節点の座標を(x,y)とすると、そこでの速度(u,v)は、次式で定義する
 * u(x,y) = a0_ + a1_*x + a2_*y + a3_*x*x + a4_*x*y + a5_*y*y
 * v(x,y) = b0_ + b1_*x + b2_*y + b3_*x*x + b4_*x*y + b5_*y*y
 */
class Boundary {
public:
    // u(x,y)の係数
    double a0_, a1_, a2_, a3_, a4_, a5_;

    // v(x,y)の係数
    double b0_, b1_, b2_, b3_, b4_, b5_;

    // 境界上の節点の一覧
    std::vector<Node *> nodes_;

    // 境界に節点を追加する
    void addNode(Node *node) {
        nodes_.push_back(node);
    }

    // 境界上の節点の速度値を多項式により設定する。
    void apply(double t, double t_ramp);
};

#endif
