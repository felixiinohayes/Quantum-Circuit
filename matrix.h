#ifndef MATRIX_H
#define MATRIX_H

class matrix
{
friend std::ostream& operator<<(std::ostream &os, const matrix &m);
friend std::istream& operator>>(std::istream &is, matrix &m);
private:
    complex *matrix_data {nullptr};
    int rows;
    int columns;

public:
    matrix() = default;
    matrix(int r, int c) : rows{r}, columns{c}, matrix_data{new complex[r * c]} {
        for (int i = 1; i <= rows; i++) {
            for (int j = 1; j <= columns; j++) {
                if (i == j) {
                    matrix_data[index(i,j)] = complex{1,0};
                }
            }
        }
    }
    matrix(const matrix &m) : rows{m.rows}, columns{m.columns}, matrix_data{new complex[m.rows * m.columns]} {
        for (int i = 0; i < (rows * columns); i++) {
            matrix_data[i] = m.matrix_data[i];
        }
    }
    matrix(matrix&& m) noexcept : rows{m.rows}, columns{m.columns}, matrix_data{m.matrix_data} {
        m.rows = 0;
        m.columns = 0;
        m.matrix_data = nullptr;
    }
    ~matrix() {
        delete[] matrix_data;
    }

    // Access functions
    int get_rows() const
    {
        return rows;
    }
    int get_cols() const
    {
        return columns;
    }

    // Set dimensions of matrix
    void set_dimensions(const int &r, const int &c)
    {
        if (r < 0 || c < 0) {
            std::cout << "Error: dimensions not valid.";
        } else {
            rows = r;
            columns = c;
            matrix_data = new complex[r * c];
        }
    }
    // Return position in array of element (m,n)
    int index(const int &m, const int &n) const
    {
        if (m > 0 && m <= rows && n > 0 && n <= columns) {
            return (n - 1) + (m - 1) * columns;
        } else {
            std::cout << "Error: index out of range" << std::endl; exit(1);
        }
    }

    // Set value in matrix given row and column (m,n)
    void set_value(const int &m, const int &n, complex value)
    {
        int i = index(m,n);
        matrix_data[i] = value;
    }
    // Get value in matrix given row and column (m,n)
    complex get_value(const int &m, const int &n) const
    {
        int i = index(m,n);
        return matrix_data[i];
    }

    // Returns input matrix with ith row and jth column deleted
    matrix get_minor(const int &row, const int &column) const
    {
        matrix output(rows - 1, columns - 1);
        if (0 < row && row <= rows && 0 < column && column <= columns) {
            for(int i = 1; i <= rows; i++) {
                for(int j = 1; j <= columns; j++) {
                    int row_offset{0};
                    int column_offset{0};
                    if (i >= row) {
                        row_offset = 1;
                    }
                    if (j >= column) {
                        column_offset = 1;
                    }
                    if ((i != row) && (j != column)) {
                        output.matrix_data[output.index(i - row_offset, j - column_offset)] = matrix_data[index(i,j)];
                    }
                }
            }
        } else {
            std::cout << "Error: row or column invalid." << std::endl; exit(1);
        }
        return output;
    }

    // Returns the transpose of the matrix
    matrix get_transpose() const
    {
        matrix result(columns, rows);
        for (int i = 1; i <= rows; i++) {
            for (int j = 1; j <= columns; j++) {
                result.set_value(j, i, get_value(i, j));
            }
        }
        return result;
    }

    // Overload copy assignment operator
    matrix& operator=(const matrix &m)
    {
        if (this != &m) {
            delete[] matrix_data;
            rows = m.rows;
            columns = m.columns;
            matrix_data = new complex[rows * columns];
            for (int i = 0; i < rows * columns; i++) {
                matrix_data[i] = m.matrix_data[i];
            }
        } else {
            std::cout << "Error: cannot copy matrix to itself" << std::endl;
        }
        return *this;
    }

    // Overload move assignment operator
    matrix& operator=(matrix &&m) noexcept
    {
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

    // Overload matrix arithmetic operators (+,-,*)
    matrix operator+(const matrix &m) const
    {
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
    matrix operator-(const matrix &m) const
    {
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
    matrix operator*(const matrix &m) const
    {
        matrix output(rows, m.columns);
        if (columns == m.rows) {
            for (int i = 1; i <= rows; i++) {
                for (int j = 1; j <= m.columns; j++) {
                    complex sum{0,0};
                    for (int k = 1; k <= columns; k++) {
                        sum = sum + (get_value(i,k) * m.get_value(k,j));
                    }
                    output.set_value(i,j,sum);
                    sum = complex{0,0};
                }
            }
        } else {
            std::cout << "Error: cannot perform multiplication as dimensions are not compatible."; exit(1);
        }
        return output;
    }
    
    // Calculate tensor product of matrix with input
    matrix tensor_product(const matrix &m) const
    {
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
};

// Overload << and >> operators for matrix insertion/extraction from iostreams
std::ostream &operator<<(std::ostream &os, const matrix &m)
{
    int digits;
    if (m.rows == 0 && m.columns == 0) {
        std::cout << "Empty matrix." << std::endl;
    }

    for (int i = 1; i <= m.rows; i++) {
        os << "[";
        for (int j = 1; j <= m.columns; j++) {
            if (j == m.columns) {
                os << m.get_value(i,j);
            } else {
            os << m.get_value(i,j) << "  ";
            }
        }
        os << "]";
        std::cout << std::endl;
    }
    return os;
}
std::istream &operator>>(std::istream &is, matrix &m)
{
    // Validation for matrix dimension inputs
    int rows_input, cols_input;
    while (std::cout << "Enter the number of rows:" << std::endl && (!(is >> rows_input) || rows_input < 1)){
            std::cout << "Error: Input must be a positive integer." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    while (std::cout << "Enter the number of columns:" << std::endl && (!(is >> cols_input) || cols_input < 1)){
            std::cout << "Error: Input must be a positive integer." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    m.set_dimensions(rows_input, cols_input);

    std::cout << "Enter the values of the matrix, each separated by a space:";
    for(int i = 1; i <= m.rows; i++) {
            for(int j = 1; j <= m.columns; j++) {
                is >> m.matrix_data[m.index(i,j)];
            }
    }
    std::cout << std::endl;
    return is;
}
#endif