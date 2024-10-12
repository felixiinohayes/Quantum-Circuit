#include "matrix.h"
#include <iostream>
#include <limits>

// Constructor
matrix::matrix(int r, int c) : rows{r}, columns{c}, matrix_data{new std::complex<double>[r * c]} {
    for (int i = 1; i <= rows; i++) {
        for (int j = 1; j <= columns; j++) {
            if (i == j) {
                matrix_data[index(i, j)] = std::complex<double>(1, 0);
            }
        }
    }
}

// Copy Constructor
matrix::matrix(const matrix &m) : rows{m.rows}, columns{m.columns}, matrix_data{new std::complex<double>[m.rows * m.columns]} {
    for (int i = 0; i < (rows * columns); i++) {
        matrix_data[i] = m.matrix_data[i];
    }
}

// Move Constructor
matrix::matrix(matrix&& m) noexcept : rows{m.rows}, columns{m.columns}, matrix_data{m.matrix_data} {
    m.rows = 0;
    m.columns = 0;
    m.matrix_data = nullptr;
}

// Destructor
matrix::~matrix() {
    delete[] matrix_data;
}

// Accessors
int matrix::get_rows() const {
    return rows;
}

int matrix::get_cols() const {
    return columns;
}

// Set dimensions
void matrix::set_dimensions(const int &r, const int &c) {
    if (r < 0 || c < 0) {
        std::cout << "Error: dimensions not valid.";
    } else {
        rows = r;
        columns = c;
        matrix_data = new std::complex<double>[r * c];
    }
}

// Calculate index
int matrix::index(const int &m, const int &n) const {
    if (m > 0 && m <= rows && n > 0 && n <= columns) {
        return (n - 1) + (m - 1) * columns;
    } else {
        std::cout << "Error: index out of range" << std::endl; exit(1);
    }
}

// Set value
void matrix::set_value(const int &m, const int &n, std::complex<double> value) {
    int i = index(m, n);
    matrix_data[i] = value;
}

// Get value
std::complex<double> matrix::get_value(const int &m, const int &n) const {
    int i = index(m, n);
    return matrix_data[i];
}

// Returns the transpose of the matrix
matrix matrix::get_transpose() const {
    matrix result(columns, rows);
    for (int i = 1; i <= rows; i++) {
        for (int j = 1; j <= columns; j++) {
            result.set_value(j, i, get_value(i, j));
        }
    }
    return result;
}

// Arithmetic Operators
matrix matrix::operator+(const matrix &m) const {
    matrix output(rows, columns);
    if (rows == m.rows && columns == m.columns) {
        for (int i = 0; i < rows * columns; i++) {
            output.matrix_data[i] = matrix_data[i] + m.matrix_data[i];
        }
    } else {
        std::cout << "Error: cannot perform addition as dimensions do not match."; exit(1);
    }
    return output;
}
matrix matrix::operator-(const matrix &m) const {
    matrix output(rows, columns);
    if (rows == m.rows && columns == m.columns) {
        for (int i = 0; i < rows * columns; i++) {
            output.matrix_data[i] = matrix_data[i] - m.matrix_data[i];
        }
    } else {
        std::cout << "Error: cannot perform subtraction as dimensions do not match."; exit(1);
    }
    return output;
}
matrix matrix::operator*(const matrix &m) const {
    matrix output(rows, m.columns);
    if (columns == m.rows) {
        for (int i = 1; i <= rows; i++) {
            for (int j = 1; j <= m.columns; j++) {
                std::complex<double> sum{0,0};
                for (int k = 1; k <= columns; k++) {
                    sum = sum + (get_value(i,k) * m.get_value(k,j));
                }
                output.set_value(i,j,sum);
                sum = std::complex<double>{0,0};
            }
        }
    } else {
        std::cout << "Error: cannot perform multiplication as dimensions are not compatible."; exit(1);
    }
    return output;
}

// Overloaded << operator
std::ostream& operator<<(std::ostream &os, const matrix &m) {
    for (int i = 1; i <= m.rows; i++) {
        os << "[";
        for (int j = 1; j <= m.columns; j++) {
            os << m.get_value(i, j);
            if (j != m.columns) os << " ";
        }
        os << "]" << std::endl;
    }
    return os;
}

// Overloaded >> operator
std::istream& operator>>(std::istream &is, matrix &m) {
    int rows_input, cols_input;
    while (std::cout << "Enter the number of rows: " && (!(is >> rows_input) || rows_input < 1)) {
        std::cout << "Error: Input must be a positive integer." << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    while (std::cout << "Enter the number of columns: " && (!(is >> cols_input) || cols_input < 1)) {
        std::cout << "Error: Input must be a positive integer." << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    m.set_dimensions(rows_input, cols_input);
    std::cout << "Enter the values of the matrix, each separated by a space:";
    for (int i = 1; i <= m.rows; i++) {
        for (int j = 1; j <= m.columns; j++) {
            is >> m.matrix_data[m.index(i, j)];
        }
    }
    return is;
}

 // Overload copy assignment operator
matrix& matrix::operator=(const matrix &m) {
    if (this != &m) {
        delete[] matrix_data;
        rows = m.rows;
        columns = m.columns;
        matrix_data = new std::complex<double>[rows * columns];
        for (int i = 0; i < rows * columns; i++) {
            matrix_data[i] = m.matrix_data[i];
        }
    } else {
        std::cout << "Error: cannot copy matrix to itself" << std::endl;
    }
    return *this;
}

// Overload move assignment operator
matrix& matrix::operator=(matrix &&m) noexcept {
    if (this != &m) {
        rows = m.rows;
        columns = m.columns;
        delete[] matrix_data;
        matrix_data = m.matrix_data;
        m.rows = 0;
        m.columns = 0;
        m.matrix_data = nullptr;
    } else {
        std::cout << "Error: cannot move matrix to itself" << std::endl;
    }

    return *this;
}

// Calculate tensor product of matrix with input
matrix matrix::tensor_product(const matrix &m) const {
    matrix output{rows * m.rows, columns * m.columns};
    for (int i = 1; i <= rows; i++) {
        for (int j = 1; j <= columns; j++) {
            for (int k = 1; k <= m.rows; k++) {
                for (int l = 1; l <= m.columns; l++) {
                    int row = (i - 1) * m.rows + k;
                    int col = (j - 1) * m.columns + l;
                    output.set_value(row, col, get_value(i, j) * m.get_value(k, l));
                }
            }
        }
    }
    return output;
}