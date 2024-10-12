#ifndef COMPONENT_H
#define COMPONENT_H

#include "matrix.h"  // Assuming matrix.h contains your matrix and complex class
#include <string>
#include <vector>
#include <cmath>
#include <complex>

class component
{
protected:
    matrix m{2, 2};
    std::string symbol;

public:
    virtual ~component() {}
    component(matrix mat, std::string sym);
    virtual matrix get_matrix();
    virtual std::string get_symbol();
};

class single_component : public component
{
protected:
    int qubit;

public:
    virtual ~single_component() {}
    single_component(matrix mat, std::string sym, int q);
    virtual int get_qubit();
};

class pauli_x : public single_component
{
public:
    ~pauli_x() {}
    pauli_x(int q);
};

class pauli_y : public single_component
{
public:
    ~pauli_y() {}
    pauli_y(int q);
};

class pauli_z : public single_component
{
public:
    ~pauli_z() {}
    pauli_z(int q);
};

class hadamard : public single_component
{
public:
    ~hadamard() {}
    hadamard(int q);
};

class identity : public single_component
{
public:
    ~identity() {}
    identity(int size);
};

class projector : public single_component
{
public:
    ~projector() {}
    projector(bool is_p0);
};

class multi_component : public component
{
protected:
    int control;
    int target;
    int qubits;

public:
    virtual ~multi_component() {}
    multi_component(matrix mat, std::string sym, int c, int t, int qs);
    virtual int get_target();
    virtual int get_control();
    matrix construct_controlled_matrix(const matrix &gate_matrix);
    matrix compute_tensor_product(const std::vector<matrix> &product_vector);
};

// Controlled gate classes
class controlled_x : public multi_component
{
public:
    ~controlled_x() {}
    controlled_x(int c, int t, int qs);
};

class controlled_y : public multi_component
{
public:
    ~controlled_y() {}
    controlled_y(int c, int t, int qs);
};

class controlled_z : public multi_component
{
public:
    ~controlled_z() {}
    controlled_z(int c, int t, int qs);
};

class controlled_h : public multi_component
{
public:
    ~controlled_h() {}
    controlled_h(int c, int t, int qs);
};

#endif