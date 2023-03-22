/*
 * Vector4.h
 */
#include <iostream>
#include <vector>

#ifndef VECTOR4_H_
#define VECTOR4_H_

/*
 * ４次元ベクトルクラス
 */
class Vector4
{
private:
    std::vector<double> val_;
public:
    ~Vector4();
    Vector4();
    Vector4(const Vector4 &o);
    Vector4(const double x0, const double x1, const double x2, const double x3);

    void set(int dim, double val);

    double get(int dim) const;

    double &operator[] (const int dim);

    Vector4 &operator=(const Vector4 &o);

    Vector4 operator+(const Vector4 &o) const;

    Vector4 operator-(const Vector4 &o) const;

    Vector4 operator*(double r) const;

    double dot(const Vector4 &o) const;
};

Vector4 operator*(double r, const Vector4 &o);

double dot(const Vector4 &v1, const Vector4 &v2);

#endif // VECTOR4_H_
