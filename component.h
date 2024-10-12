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

class pauli_x : public single_component
{
public:
    ~pauli_x() {}
    pauli_x(int q) : single_component{matrix{2, 2}, "X", q}
    {
        m.set_value(1, 1, complex{0, 0});
        m.set_value(1, 2, complex{1, 0});
        m.set_value(2, 1, complex{1, 0});
        m.set_value(2, 2, complex{0, 0});
    }
};

class pauli_y : public single_component
{
public:
    ~pauli_y() {}
    pauli_y(int q) : single_component{matrix{2, 2}, "Y", q}
    {
        m.set_value(1, 1, complex{0, 0});
        m.set_value(1, 2, complex{0, -1});
        m.set_value(2, 1, complex{0, 1});
        m.set_value(2, 2, complex{0, 0});
    }
};

class pauli_z : public single_component
{
public:
    ~pauli_z() {}
    pauli_z(int q) : single_component{matrix{2, 2}, "Z", q}
    {
        m.set_value(1, 1, complex{1, 0});
        m.set_value(1, 2, complex{0, 0});
        m.set_value(2, 1, complex{0, 0});
        m.set_value(2, 2, complex{-1, 0});
    }
};

class hadamard : public single_component
{
public:
    ~hadamard() {}
    hadamard(int q) : single_component{matrix{2, 2}, "H", q}
    {
        m.set_value(1, 1, complex{1 / sqrt(2), 0});
        m.set_value(1, 2, complex{1 / sqrt(2), 0});
        m.set_value(2, 1, complex{1 / sqrt(2), 0});
        m.set_value(2, 2, complex{-1 / sqrt(2), 0});
    }
};

class identity : public single_component
{
public:
    ~identity() {}
    identity(int size) : single_component{matrix{size, size}, "I", 0}
    {
        for (int i = 0; i < size; ++i) {
            m.set_value(i + 1, i + 1, complex{1, 0});
        }
    }
};

class projector : public single_component
{
public:
    ~projector() {}
    projector(bool is_p0) : single_component{matrix{2, 2}, "P", 0}
    {
        if (!is_p0) {
            m.set_value(1, 1, complex{1, 0});
        } else {
            m.set_value(2, 2, complex{1, 0});
        }
    }
};

class multi_component : public component
{
protected:
    int control;
    int target;
    int qubits;
public:
    virtual ~multi_component() {}
    multi_component(matrix mat, std::string sym, int c, int t, int qs) 
        : component{mat, sym}, control{c}, target{t}, qubits{qs} {}
    virtual int get_target()
    {
        return target;
    }
    virtual int get_control()
    {
        return control;
    }

    // Helper function to construct matrix based on control and target qubits
    matrix construct_controlled_matrix(const matrix& gate_matrix)
    {
        int matrix_size = 1 << qubits;
        m.set_dimensions(matrix_size, matrix_size);

        identity id(2);
        projector p0(0);
        projector p1(1);

        std::vector<matrix> product_vector1;
        std::vector<matrix> product_vector2;

        for (int i = 0; i < qubits; ++i) {
            if (i == control) {
                product_vector1.push_back(p0.get_matrix());
                product_vector2.push_back(p1.get_matrix());
            } else if (i == target) {
                product_vector1.push_back(id.get_matrix());
                product_vector2.push_back(gate_matrix);
            } else {
                product_vector1.push_back(id.get_matrix());
                product_vector2.push_back(id.get_matrix());
            }
        }

        matrix product1 = compute_tensor_product(product_vector1);
        matrix product2 = compute_tensor_product(product_vector2);

        return product1 + product2;
    }

    // Helper function to compute the tensor product
    matrix compute_tensor_product(const std::vector<matrix>& product_vector)
    {
        matrix product = product_vector.back();
        for (int i = product_vector.size() - 2; i >= 0; --i) {
            product = product.tensor_product(product_vector[i]);
        }
        return product;
    }
};


// Controlled gate classes
class controlled_x : public multi_component
{
public:
    ~controlled_x() {}
    controlled_x(int c, int t, int qs) : multi_component{matrix{2, 2}, "CX", c, t, qs}
    {
        pauli_x x_gate(1);
        m = construct_controlled_matrix(x_gate.get_matrix());
    }
};

class controlled_y : public multi_component
{
public:
    ~controlled_y() {}
    controlled_y(int c, int t, int qs) : multi_component{matrix{2, 2}, "CY", c, t, qs}
    {
        pauli_y y_gate(1);
        m = construct_controlled_matrix(y_gate.get_matrix());
    }
};

class controlled_z : public multi_component
{
public:
    ~controlled_z() {}
    controlled_z(int c, int t, int qs) : multi_component{matrix{2, 2}, "CZ", c, t, qs}
    {
        pauli_z z_gate(1);
        m = construct_controlled_matrix(z_gate.get_matrix());
    }
};

class controlled_h : public multi_component
{
public:
    ~controlled_h() {}
    controlled_h(int c, int t, int qs) : multi_component{matrix{2, 2}, "CH", c, t, qs}
    {
        hadamard h_gate(1);
        m = construct_controlled_matrix(h_gate.get_matrix());
    }
};

#endif