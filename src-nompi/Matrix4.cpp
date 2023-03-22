/*
 * Matrix4.cpp
 */
#include <Matrix4.h>
#include <Vector4.h>
#include <vector>

Matrix4::~Matrix4(){}
Matrix4::Matrix4(){
    val_.resize(16, 0.0);
}
Matrix4::Matrix4(const Matrix4 &o){
    val_ = o.val_;
}
void Matrix4::unit(){
    for(size_t i = 0; i < 4; i++){
        for(size_t j = 0; j < 4; j++){
            if(i==j){val_[i*4+j] = 1.;}
            else    {val_[i*4+j] = 0.;}
        }
    }
}
void Matrix4::set(int row, int col, double val){
    val_[row*4 + col] = val;
}
void Matrix4::set(int rowcol, double val){
    val_[rowcol] = val;
}

double Matrix4::get(int row, int col) const{
    return val_[row*4 + col];
}
double Matrix4::get(int rowcol) const{
    return val_[rowcol];
}

Matrix4 &Matrix4::operator=(const Matrix4 &o){
    val_ = o.val_;
    return *this;
}

Matrix4 Matrix4::operator+(const Matrix4 &o) const{
    Matrix4 result;
    for(size_t i = 0; i < 16; i++){
        result.val_[i] = val_[i] + o.val_[i];
    }
    return result;
}

Matrix4 Matrix4::operator-(const Matrix4 &o) const{
    Matrix4 result;
    for(size_t i = 0; i < 16; i++){
        result.val_[i] = val_[i] - o.val_[i];
    }
    return result;
}

Matrix4 Matrix4::operator*(const Matrix4 &o) const{
    Matrix4 result;
    for(size_t i = 0; i < 4; i++){
        for(size_t j = 0; j < 4; j++){
            for(size_t k = 0; k < 4; k++){
                result.val_[i*4 + j] += val_[i*4 + k]*o.val_[k*4 + j];
            }
        }
    }
    return result;
}

Vector4 Matrix4::operator*(const Vector4 &o) const{
    Vector4 result;
    for(size_t i = 0; i < 4; i++){
        double val = 0;
        for(size_t j = 0; j < 4; j++){
            val += val_[i*4 + j] * o.get(j);
        }
        result.set(i,val);
    }
    return result;
}

Matrix4 Matrix4::operator*(double x) const{
    Matrix4 result;
    for(size_t i = 0; i < 16; i++){
        result.val_[i] = val_[i]*x;
    }
    return result;
}

Matrix4 Matrix4::T() const{
    Matrix4 transpose;
    for(size_t i = 0; i < 4; i++){
        for(size_t j = 0; j < 4; j++){
            transpose.set(i,j, val_[j*4 + i]);
        }
    }
    return transpose;
}

Vector4 operator*(const Vector4 &v, const Matrix4 &m){
    Vector4 result;
    double val;
    for(int i = 0; i < 4; i++){
        val = 0;
        for(int j = 0; j < 4; j++){
            val += v.get(j)*m.get(j,i);
        }
        result.set(i, val);
    }
    return result;
}

Matrix4 operator*(double x, const Matrix4 &m){
    return m * x;
}
