#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include <bitset>
#include <iomanip>
#include "matrix.h"
#include "component.h"

class circuit
{
private:
    std::vector<std::vector<component*>> reg;  // Circuit register (where components are stored)
    matrix input_vector;
    std::vector<char> initial_states;  // Stores initial individual qubit states as char (0, 1, +, -)
    int matrix_size;
    int qubits;

public:
    ~circuit();
    
    circuit(int qubits, matrix input, std::vector<char> initial_states);

    int get_qubits() const;
    void add(component* comp);
    matrix get_resultant_matrix();
    void order_reg();
    void print_braket(matrix statevector);
    void draw();
};

#endif