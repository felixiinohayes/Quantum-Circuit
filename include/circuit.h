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
    
    // Parameterized constructor
    circuit(int qubits, matrix input, std::vector<char> initial_states);

    // Get the number of qubits in the circuit
    int get_qubits() const;

    // Add component to circuit at specified qubit
    void add(component* comp);

    // Computes matrix product of the current circuit
    matrix get_resultant_matrix();

    // Reorder circuit register to minimize the number of columns
    void order_reg();

    // Print statevector in bra-ket notation
    void print_braket(matrix statevector);

    // Print ASCII representation of the circuit
    void draw();
};

#endif