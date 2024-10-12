#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <limits>
#include <complex>

class matrix
{
    friend std::ostream& operator<<(std::ostream &os, const matrix &m);
    friend std::istream& operator>>(std::istream &is, matrix &m);

private:
    std::complex<double>* matrix_data {nullptr};
    int rows;
    int columns;

public:
    // Constructors, Destructor, and Operators
    matrix() = default;
    matrix(int r, int c);
    matrix(const matrix &m);
    matrix(matrix &&m) noexcept;
    ~matrix();

    matrix& operator=(const matrix &m);
    matrix& operator=(matrix &&m) noexcept;

    // Accessors
    int get_rows() const;
    int get_cols() const;

    
    void set_dimensions(const int &r, const int &c);

    // Matrix Manipulation Methods
    int index(const int &m, const int &n) const;
    void set_value(const int &m, const int &n, std::complex<double> value);
    std::complex<double> get_value(const int &m, const int &n) const;
    matrix get_minor(const int &row, const int &column) const;
    matrix get_transpose() const;

    matrix operator+(const matrix &m) const;
    matrix operator-(const matrix &m) const;
    matrix operator*(const matrix &m) const;
    matrix tensor_product(const matrix &m) const;
};

#endif