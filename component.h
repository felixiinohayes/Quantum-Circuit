#ifndef COMPONENT_H
#define COMPONENT_H

class component
{
protected:
    matrix m{2,2};
    std::string symbol;
public:
    virtual ~component() {}
    component(matrix mat, std::string sym) : m{mat}, symbol{sym} {}
    virtual matrix get_matrix()
    {
        return m;
    }
    virtual std::string get_symbol()
    {
        return symbol;
    }
};

class single_component : public component
{
protected:
    int qubit;
public:
    virtual ~single_component() {}
    single_component(matrix mat, std::string sym, int q) : component{mat, sym}, qubit{q} {}
    virtual int get_qubit()
    {
        return qubit;
    }
};

class multi_component : public component
{
protected:
    int control;
    int target;
public:
    virtual ~multi_component() {}
    multi_component(matrix mat, std::string sym, int c, int t) : component{mat, sym}, control{c}, target{t} {}
    virtual int get_target()
    {
        return target;
    }
    virtual int get_control()
    {
        return control;
    }
};

class pauli_x : public single_component
{
public:
    ~pauli_x() {}
    pauli_x(int q) : single_component{matrix{2, 2}, std::string("X"), q}
    {
        m.set_value(1,1,complex{0,0});
        m.set_value(1,2,complex{1,0});
        m.set_value(2,1,complex{1,0});
        m.set_value(2,2,complex{0,0});
    }
};

class pauli_y : public single_component
{
public:
    ~pauli_y() {}
    pauli_y(int q) : single_component{matrix{2, 2}, std::string("Y"), q}
    {
        m.set_value(1,1,complex{0,0});
        m.set_value(1,2,complex{0,-1});
        m.set_value(2,1,complex{0,1});
        m.set_value(2,2,complex{0,0});
    }
};

class pauli_z : public single_component
{
public:
    ~pauli_z() {}
    pauli_z(int q) : single_component{matrix{2, 2}, std::string("Z"), q}
    {
        m.set_value(1,1,complex{1,0});
        m.set_value(1,2,complex{0,0});
        m.set_value(2,1,complex{0,0});
        m.set_value(2,2,complex{-1,0});
    }
};

class hadamard : public single_component
{
public:
    ~hadamard() {}
    hadamard(int q) : single_component{matrix{2, 2}, std::string("H"), q}
    {
        m.set_value(1,1,complex{1/sqrt(2),0});
        m.set_value(1,2,complex{1/sqrt(2),0});
        m.set_value(2,1,complex{1/sqrt(2),0});
        m.set_value(2,2,complex{-1/sqrt(2),0});
    }
};

class identity : public single_component
{
public:
    ~identity() {}
    identity(int size) : single_component{matrix{size, size}, std::string("I"), 0}
    {
        // Sets matrix with 1s in diagonal (Identity matrix)
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (i==j) {
                    m.set_value(i+1, j+1, complex{1,0});
                } else {
                    m.set_value(i+1, j+1, complex{0,0});
                }
            }
        }
    }
};

class projector : public single_component
{
public:
    ~projector() {}
    // Checks what type of projector it is and sets matrix accordingly
    projector(bool is_p0) : single_component{matrix{2, 2}, std::string("P"), 0}{
        if (!is_p0) {
            m.set_value(1,1,complex{1,0});
            m.set_value(1,2,complex{0,0});
            m.set_value(2,1,complex{0,0});
            m.set_value(2,2,complex{0,0});
        } else {
            m.set_value(1,1,complex{0,0});
            m.set_value(1,2,complex{0,0});
            m.set_value(2,1,complex{0,0});
            m.set_value(2,2,complex{1,0});
        }
    }
};

class controlled_x : public multi_component
{
private:
    int qubits;
    int matrix_size;
public:
    ~controlled_x() {}
    controlled_x(int c, int t, int qs) : multi_component{matrix{2,2}, "X", c, t}, qubits{qs} {
        // Constructor to set the values of the matrix given the number of qubits
        // and the control and target qubits.
        
        matrix_size = 1 << qubits; // // Use bitset to calculate size of matrix
        m.set_dimensions(matrix_size,matrix_size);
        identity id(2);
        projector p0(0);
        projector p1(1);
        pauli_x x(1);

        // Create vectors of constituent matrices before computing tensor product
        std::vector<matrix> product_vector1;
        std::vector<matrix> product_vector2;
        for (int i = 0; i < qubits; i++) {
            if (i == control) {
                product_vector1.push_back((&p0)->get_matrix());
                product_vector2.push_back((&p1)->get_matrix());
            } else if (i == target) {
                product_vector1.push_back((&id)->get_matrix());
                product_vector2.push_back((&x)->get_matrix());
            } else {
                product_vector1.push_back((&id)->get_matrix());
                product_vector2.push_back((&id)->get_matrix());
            }
        }

        // Calculate the running tensor product of matrices in vector
        matrix product1;
        matrix product2;
        product1 = product_vector1.back();
        product2 = product_vector2.back();
        for (int i = product_vector1.size() - 2; i >= 0; i--) {
            product1 = product1.tensor_product(product_vector1[i]);
        }
        for (int i = product_vector2.size() - 2; i >= 0; i--) {
            product2 = product2.tensor_product(product_vector2[i]);
        }
        m = product1 + product2;
    }
};

class controlled_y  : public multi_component
{
private:
    int qubits;
    int matrix_size;
public:
    ~controlled_y() {}
    controlled_y(int c, int t, int qs) : multi_component{matrix{2,2}, "Y", c, t}, qubits{qs} {
        // Constructor to set the values of the matrix given the number of qubits
        // and the control and target qubits.
        
        matrix_size = 1 << qubits;
        m.set_dimensions(matrix_size,matrix_size);
        identity id(2);
        projector p0(0);
        projector p1(1);
        pauli_x y(1);

        // Create vectors of constituent matrices before computing tensor product
        std::vector<matrix> product_vector1;
        std::vector<matrix> product_vector2;
        for (int i = 0; i < qubits; i++) {
            if (i == control) {
                product_vector1.push_back((&p0)->get_matrix());
                product_vector2.push_back((&p1)->get_matrix());
            } else if (i == target) {
                product_vector1.push_back((&id)->get_matrix());
                product_vector2.push_back((&y)->get_matrix());
            } else {
                product_vector1.push_back((&id)->get_matrix());
                product_vector2.push_back((&id)->get_matrix());
            }
        }

        // Calculate the running tensor product of matrices in vector
        matrix product1;
        matrix product2;
        product1 = product_vector1.back();
        product2 = product_vector2.back();
        for (int i = product_vector1.size() - 2; i >= 0; i--) {
            product1 = product1.tensor_product(product_vector1[i]);
        }
        for (int i = product_vector2.size() - 2; i >= 0; i--) {
            product2 = product2.tensor_product(product_vector2[i]);
        }
        m = product1 + product2;
    }
};

class controlled_z  : public multi_component
{
private:
    int qubits;
    int matrix_size;
public:
    ~controlled_z() {}
    controlled_z(int c, int t, int qs) : multi_component{matrix{2,2}, "Z", c, t}, qubits{qs} {
        // Constructor to set the values of the matrix given the number of qubits
        // and the control and target qubits.

        matrix_size = 1 << qubits; // Use bitset to calculate size of matrix
        m.set_dimensions(matrix_size,matrix_size);
        identity id(2);
        projector p0(0);
        projector p1(1);
        pauli_x z(1);

        // Create vectors of constituent matrices before computing tensor product
        std::vector<matrix> product_vector1;
        std::vector<matrix> product_vector2;
        for (int i = 0; i < qubits; i++) {
            if (i == control) {
                product_vector1.push_back((&p0)->get_matrix());
                product_vector2.push_back((&p1)->get_matrix());
            } else if (i == target) {
                product_vector1.push_back((&id)->get_matrix());
                product_vector2.push_back((&z)->get_matrix());
            } else {
                product_vector1.push_back((&id)->get_matrix());
                product_vector2.push_back((&id)->get_matrix());
            }
        }

        // Calculate the running tensor product of matrices in vector
        matrix product1;
        matrix product2;
        product1 = product_vector1.back();
        product2 = product_vector2.back();
        for (int i = product_vector1.size() - 2; i >= 0; i--) {
            product1 = product1.tensor_product(product_vector1[i]);
        }
        for (int i = product_vector2.size() - 2; i >= 0; i--) {
            product2 = product2.tensor_product(product_vector2[i]);
        }
        m = product1 + product2;
    }
};

class controlled_h  : public multi_component
{
private:
    int qubits;
    int matrix_size;
public:
    ~controlled_h() {}
    controlled_h(int c, int t, int qs) : multi_component{matrix{2,2}, "H", c, t}, qubits{qs} {
        // Constructor to set the values of the matrix given the number of qubits
        // and the control and target qubits.    
        matrix_size = 1 << qubits;
        m.set_dimensions(matrix_size,matrix_size);
        identity id(2);
        projector p0(0);
        projector p1(1);
        pauli_x h(1);

        // Create vectors of constituent matrices before computing tensor product
        std::vector<matrix> product_vec1;
        std::vector<matrix> product_vec2;
        for (int i = 0; i < qubits; i++) {
            if (i == control) {
                product_vec1.push_back((&p0)->get_matrix());
                product_vec2.push_back((&p1)->get_matrix());
            } else if (i == target) {
                product_vec1.push_back((&id)->get_matrix());
                product_vec2.push_back((&h)->get_matrix());
            } else {
                product_vec1.push_back((&id)->get_matrix());
                product_vec2.push_back((&id)->get_matrix());
            }
        }

        // Calculate the running tensor product of matrices in vector
        matrix product1;
        matrix product2;
        product1 = product_vec1.back();
        product2 = product_vec2.back();
        for (int i = product_vec1.size() - 2; i >= 0; i--) {
            product1 = product1.tensor_product(product_vec1[i]);
        }
        for (int i = product_vec2.size() - 2; i >= 0; i--) {
            product2 = product2.tensor_product(product_vec2[i]);
        }
        m = product1 + product2;
    }
};
#endif