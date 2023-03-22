/*
 * Boundary.cpp
 */
#include <Boundary.h>
#include <Logger.h>
#include <cmath>

void Boundary::apply(double t, double t_ramp)
{
    // 境界上の速度値を多項式に基づいて設定する
    size_t i;
    double x, y;
    double weight;
    double pi = M_PI;
    for(i = 0; i < nodes_.size(); i++){
        x = nodes_[i]->pos_.x_;
        y = nodes_[i]->pos_.y_;
        if(t < t_ramp){
            weight = (1.0 - std::cos(pi*t / t_ramp))/2.0;
        }else{
            weight = 1.0;
        }
        nodes_[i]->vel_.x_ = (a0_ + a1_*x + a2_*y + a3_*x*x + a4_*x*y + a5_*y*y)*weight;
        nodes_[i]->vel_.y_ = (b0_ + b1_*x + b2_*y + b3_*x*x + b4_*x*y + b5_*y*y)*weight;
    }
}
