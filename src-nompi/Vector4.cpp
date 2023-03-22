/*
 * Vector4.cpp
 */
#include <Vector4.h>
#include <vector>

Vector4::~Vector4(){}
Vector4::Vector4(){
    val_.resize(4,0.0);
}
Vector4::Vector4(const Vector4 &o){
    val_ = o.val_;
}

Vector4::Vector4(const double x0, const double x1, const double x2, const double x3){
    Vector4 x;
    x.set(0,x0);
    x.set(1,x0);
    x.set(2,x0);
    x.set(3,x0);
    *this = x;
}

void Vector4::set(int dim, double val){
    val_[dim] = val;
}
double Vector4::get(int dim) const{
    return val_[dim];
}

double &Vector4::operator[] (const int dim) {
    return val_[dim];
}

Vector4 &Vector4::operator=(const Vector4 &o){
    val_ = o.val_;
    return *this;
}
Vector4 Vector4::operator+(const Vector4 &o) const{
    Vector4 result;
    for(size_t i = 0; i < 4; i++){
        result.val_[i] = val_[i] + o.val_[i];
    }
    return result;
}
Vector4 Vector4::operator-(const Vector4 &o) const{
    Vector4 result;
    for(size_t i = 0; i < 4; i++){
        result.val_[i] = val_[i] - o.val_[i];
    }
    return result;
}
Vector4 Vector4::operator*(double r) const{
    Vector4 result;
    for(size_t i = 0; i < 4; i++){
        result.val_[i] = val_[i]*r;
    }
    return result;
}
double Vector4::dot(const Vector4 &o) const{
    double result=0;
    for(size_t i = 0; i < 4; i++){
        result += val_[i]*o.val_[i];
    }
    return result;
}

Vector4 operator*(double r, const Vector4 &o) {
    return o*r;
}
double dot(const Vector4 &v1, const Vector4 &v2){
    return v1.dot(v2);
}
