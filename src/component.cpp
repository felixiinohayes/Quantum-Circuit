#include "component.h"

component::component(matrix mat, std::string sym) : m{mat}, symbol{sym} {}

matrix component::get_matrix() {
    return m;
}

std::string component::get_symbol() {
    return symbol;
}

// Single-qubit component constructor
single_component::single_component(matrix mat, std::string sym, int q) : component{mat, sym}, qubit{q} {}

int single_component::get_qubit() {
    return qubit;
}

pauli_x::pauli_x(int q) : single_component{matrix{2, 2}, "X", q} {
    m.set_value(1, 1, std::complex<double>{0, 0});
    m.set_value(1, 2, std::complex<double>{1, 0});
    m.set_value(2, 1, std::complex<double>{1, 0});
    m.set_value(2, 2, std::complex<double>{0, 0});
}

pauli_y::pauli_y(int q) : single_component{matrix{2, 2}, "Y", q} {
    m.set_value(1, 1, std::complex<double>{0, 0});
    m.set_value(1, 2, std::complex<double>{0, -1});
    m.set_value(2, 1, std::complex<double>{0, 1});
    m.set_value(2, 2, std::complex<double>{0, 0});
}

pauli_z::pauli_z(int q) : single_component{matrix{2, 2}, "Z", q} {
    m.set_value(1, 1, std::complex<double>{1, 0});
    m.set_value(1, 2, std::complex<double>{0, 0});
    m.set_value(2, 1, std::complex<double>{0, 0});
    m.set_value(2, 2, std::complex<double>{-1, 0});
}

hadamard::hadamard(int q) : single_component{matrix{2, 2}, "H", q} {
    m.set_value(1, 1, std::complex<double>{1 / sqrt(2), 0});
    m.set_value(1, 2, std::complex<double>{1 / sqrt(2), 0});
    m.set_value(2, 1, std::complex<double>{1 / sqrt(2), 0});
    m.set_value(2, 2, std::complex<double>{-1 / sqrt(2), 0});
}

identity::identity(int size) : single_component{matrix{size, size}, "I", 0} {
    for (int i = 0; i < size; ++i) {
        m.set_value(i + 1, i + 1, std::complex<double>{1, 0});
    }
}

projector::projector(bool is_p0) : single_component{matrix{2, 2}, "P", 0} {
    if (!is_p0) {
        m.set_value(1, 1, std::complex<double>{1, 0});
    } else {
        m.set_value(2, 2, std::complex<double>{1, 0});
    }
}

// Multi-qubit component contructor
multi_component::multi_component(matrix mat, std::string sym, int c, int t, int qs) : component{mat, sym}, control{c}, target{t}, qubits{qs} {}

int multi_component::get_target() {
    return target;
}

int multi_component::get_control() {
    return control;
}

matrix multi_component::compute_tensor_product(const std::vector<matrix> &product_vector) {
    matrix product = product_vector.back();
    for (int i = product_vector.size() - 2; i >= 0; --i) {
        product = product.tensor_product(product_vector[i]);
    }
    return product;
}

matrix multi_component::construct_controlled_matrix(const matrix& gate_matrix) {
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

controlled_x::controlled_x(int c, int t, int qs) : multi_component{matrix{2, 2}, "X", c, t, qs} {
    pauli_x x_gate(1);
    m = construct_controlled_matrix(x_gate.get_matrix());
}

controlled_y::controlled_y(int c, int t, int qs) : multi_component{matrix{2, 2}, "Y", c, t, qs} {
    pauli_y y_gate(1);
    m = construct_controlled_matrix(y_gate.get_matrix());
}

controlled_z::controlled_z(int c, int t, int qs) : multi_component{matrix{2, 2}, "Z", c, t, qs} {
    pauli_z z_gate(1);
    m = construct_controlled_matrix(z_gate.get_matrix());
}

controlled_h::controlled_h(int c, int t, int qs) : multi_component{matrix{2, 2}, "H", c, t, qs} {
    hadamard h_gate(1);
    m = construct_controlled_matrix(h_gate.get_matrix());
}