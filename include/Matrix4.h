/*
 * Matrix4.h
 */

#include <iostream>
#include <vector>
#include "Vector4.h"

#ifndef MATRIX4_H_
#define MATRIX4_H_

/*
 * 4x4行列のクラス。
 *
 * 行列間の四則演算，ベクトルと行列の掛け算，行列の転置
 */
class Matrix4
{
private:
    std::vector<double> val_;
public:
    ~Matrix4();

    Matrix4();
    Matrix4(const Matrix4 &o);

    void unit();

    void set(int row, int col, double val);
    void set(int rowcol, double val);
    double get(int row, int col) const;
    double get(int rowcol) const;

    Matrix4 &operator=(const Matrix4 &o);

    Matrix4 operator+(const Matrix4 &o) const;

    Matrix4 operator-(const Matrix4 &o) const;

    Matrix4 operator*(const Matrix4 &o) const;

    Vector4 operator*(const Vector4 &o) const;

    Matrix4 operator*(double x) const;

    Matrix4 T() const;
    // Matrix4 inv(){
    //     Matrix4 inv;
    //     inv.val_[0] = val_[5]  * val_[10] * val_[15] - 
    //         val_[5]  * val_[11] * val_[14] - 
    //         val_[9]  * val_[6]  * val_[15] + 
    //         val_[9]  * val_[7]  * val_[14] +
    //         val_[13] * val_[6]  * val_[11] - 
    //         val_[13] * val_[7]  * val_[10];

    //     inv.val_[4] = -val_[4]  * val_[10] * val_[15] + 
    //         val_[4]  * val_[11] * val_[14] + 
    //         val_[8]  * val_[6]  * val_[15] - 
    //         val_[8]  * val_[7]  * val_[14] - 
    //         val_[12] * val_[6]  * val_[11] + 
    //         val_[12] * val_[7]  * val_[10];

    //     inv.val_[8] = val_[4]  * val_[9] * val_[15] - 
    //         val_[4]  * val_[11] * val_[13] - 
    //         val_[8]  * val_[5] * val_[15] + 
    //         val_[8]  * val_[7] * val_[13] + 
    //         val_[12] * val_[5] * val_[11] - 
    //         val_[12] * val_[7] * val_[9];

    //     inv.val_[12] = -val_[4]  * val_[9] * val_[14] + 
    //         val_[4]  * val_[10] * val_[13] +
    //         val_[8]  * val_[5] * val_[14] - 
    //         val_[8]  * val_[6] * val_[13] - 
    //         val_[12] * val_[5] * val_[10] + 
    //         val_[12] * val_[6] * val_[9];

    //     inv.val_[1] = -val_[1]  * val_[10] * val_[15] + 
    //         val_[1]  * val_[11] * val_[14] + 
    //         val_[9]  * val_[2] * val_[15] - 
    //         val_[9]  * val_[3] * val_[14] - 
    //         val_[13] * val_[2] * val_[11] + 
    //         val_[13] * val_[3] * val_[10];

    //     inv.val_[5] = val_[0]  * val_[10] * val_[15] - 
    //         val_[0]  * val_[11] * val_[14] - 
    //         val_[8]  * val_[2] * val_[15] + 
    //         val_[8]  * val_[3] * val_[14] + 
    //         val_[12] * val_[2] * val_[11] - 
    //         val_[12] * val_[3] * val_[10];

    //     inv.val_[9] = -val_[0]  * val_[9] * val_[15] + 
    //         val_[0]  * val_[11] * val_[13] + 
    //         val_[8]  * val_[1] * val_[15] - 
    //         val_[8]  * val_[3] * val_[13] - 
    //         val_[12] * val_[1] * val_[11] + 
    //         val_[12] * val_[3] * val_[9];

    //     inv.val_[13] = val_[0]  * val_[9] * val_[14] - 
    //         val_[0]  * val_[10] * val_[13] - 
    //         val_[8]  * val_[1] * val_[14] + 
    //         val_[8]  * val_[2] * val_[13] + 
    //         val_[12] * val_[1] * val_[10] - 
    //         val_[12] * val_[2] * val_[9];

    //     inv.val_[2] = val_[1]  * val_[6] * val_[15] - 
    //         val_[1]  * val_[7] * val_[14] - 
    //         val_[5]  * val_[2] * val_[15] + 
    //         val_[5]  * val_[3] * val_[14] + 
    //         val_[13] * val_[2] * val_[7] - 
    //         val_[13] * val_[3] * val_[6];

    //     inv.val_[6] = -val_[0]  * val_[6] * val_[15] + 
    //         val_[0]  * val_[7] * val_[14] + 
    //         val_[4]  * val_[2] * val_[15] - 
    //         val_[4]  * val_[3] * val_[14] - 
    //         val_[12] * val_[2] * val_[7] + 
    //         val_[12] * val_[3] * val_[6];

    //     inv.val_[10] = val_[0]  * val_[5] * val_[15] - 
    //         val_[0]  * val_[7] * val_[13] - 
    //         val_[4]  * val_[1] * val_[15] + 
    //         val_[4]  * val_[3] * val_[13] + 
    //         val_[12] * val_[1] * val_[7] - 
    //         val_[12] * val_[3] * val_[5];

    //     inv.val_[14] = -val_[0]  * val_[5] * val_[14] + 
    //         val_[0]  * val_[6] * val_[13] + 
    //         val_[4]  * val_[1] * val_[14] - 
    //         val_[4]  * val_[2] * val_[13] - 
    //         val_[12] * val_[1] * val_[6] + 
    //         val_[12] * val_[2] * val_[5];

    //     inv.val_[3] = -val_[1] * val_[6] * val_[11] + 
    //         val_[1] * val_[7] * val_[10] + 
    //         val_[5] * val_[2] * val_[11] - 
    //         val_[5] * val_[3] * val_[10] - 
    //         val_[9] * val_[2] * val_[7] + 
    //         val_[9] * val_[3] * val_[6];

    //     inv.val_[7] = val_[0] * val_[6] * val_[11] - 
    //         val_[0] * val_[7] * val_[10] - 
    //         val_[4] * val_[2] * val_[11] + 
    //         val_[4] * val_[3] * val_[10] + 
    //         val_[8] * val_[2] * val_[7] - 
    //         val_[8] * val_[3] * val_[6];

    //     inv.val_[11] = -val_[0] * val_[5] * val_[11] + 
    //         val_[0] * val_[7] * val_[9] + 
    //         val_[4] * val_[1] * val_[11] - 
    //         val_[4] * val_[3] * val_[9] - 
    //         val_[8] * val_[1] * val_[7] + 
    //         val_[8] * val_[3] * val_[5];

    //     inv.val_[15] = val_[0] * val_[5] * val_[10] - 
    //         val_[0] * val_[6] * val_[9] - 
    //         val_[4] * val_[1] * val_[10] + 
    //         val_[4] * val_[2] * val_[9] + 
    //         val_[8] * val_[1] * val_[6] - 
    //         val_[8] * val_[2] * val_[5];

    //     double det;
    //     det  = val_[0] * inv.val_[0] + val_[1] * inv.val_[4] + val_[2] * inv.val_[8] + val_[3] * inv.val_[12];
    //     if(det == 0.0){
    //         std::cout << "det = 0, cannot get inverse matrix" << std::endl;
    //     }
    //     for (size_t i = 0; i < 16; i++){
    //         inv.val_[i] = inv.val_[i] / det;
    //     }
    //     return inv;                  
    // }

};

Vector4 operator*(const Vector4 &v, const Matrix4 &m);

Matrix4 operator*(double x, const Matrix4 &m);

#endif // MATRIX4_H_
