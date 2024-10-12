#include "circuit.h"

// Destructor
circuit::~circuit() {
    reg.clear();
}

// Constructor
circuit::circuit(int qubits, matrix input, std::vector<char> initial_states) 
    : input_vector{input}, initial_states{initial_states},qubits{qubits} {
    matrix_size = 1 << qubits;  // Set matrix size to (2^q) using bitshifting
}

// Return number of qubits in the circuit
int circuit::get_qubits() const {
    return qubits;
}

// Add component to the circuit
void circuit::add(component* comp) {
    if (dynamic_cast<single_component*>(comp)) {  // Checks if component is single-qubit
        identity* id = new identity(2);
        std::vector<component*> comp_column;
        for (int i = 0; i < qubits; i++) {
            if (i == dynamic_cast<single_component*>(comp)->get_qubit()) {
                comp_column.push_back(comp);
            } else {
                comp_column.push_back(id);
            }
        }
        reg.push_back(comp_column);  // Add vector to register
    } else {
        std::vector<component*> comp_column;
        comp_column.push_back(comp);
        reg.push_back(comp_column);
    }
}

// Computes matrix product of current circuit
matrix circuit::get_resultant_matrix() {
    if (reg.empty()) {
        throw std::logic_error("The circuit has no components.");
    }

    matrix total_product{matrix_size, matrix_size};
    for (const auto& comp_column : reg) {
        matrix column_product = (*comp_column.begin())->get_matrix();
        auto multiply_matrix = [&](component* comp) {
            column_product = comp->get_matrix().tensor_product(column_product);
        };

        std::for_each(comp_column.begin() + 1, comp_column.end(), multiply_matrix);
        total_product = column_product * total_product;
    }
    return total_product;
}

// Reorder circuit register to minimize number of columns
void circuit::order_reg() {
    if (reg.size() == 1) {
        return;
    }
    bool moved{false};
    int i = reg.size() - 1;
    for (int j = 0; j < reg[i].size(); j++) {
        if (dynamic_cast<single_component*>(reg[i][j])) {
            if (reg[i][j]->get_symbol() != "I") {
                bool leftmost{false};
                while (!leftmost && i > 0) {
                    if (reg[i - 1].size() > 1) {
                        if (reg[i - 1][j]->get_symbol() != "I") {
                            leftmost = true;
                        } else {
                            std::swap(reg[i][j], reg[i - 1][j]);
                            i--;
                            moved = true;
                        }
                    } else {
                        leftmost = true;
                    }
                }
            }
        }
    }
    if (moved) {
        reg.pop_back();
    }
}

// Print statevector in bra-ket notation
void circuit::print_braket(matrix statevector) {
    if (statevector.get_cols() != 1 || statevector.get_rows() != matrix_size) { // Checks if input statevector is valid
        throw std::invalid_argument("Invalid statevector size.");
    }
    bool first_term = true;
    for (int i = 1; i <= statevector.get_rows(); i++) {
        std::complex<double> value = statevector.get_value(i,1);
        if (std::abs(value) != 0) { // Check if vector element is 0
            if (!first_term) {
                if (value.real() > 0 || (value.real() == 0 && value.imag() > 0))
                std::cout << "+ ";
            } else {
                first_term = false;
            }
            if (std::abs(value) == 1 && value.imag() == 0) { // Clean up output when real part is +/- 1
                if (value.real() == 1) {
                    std::cout << "|";
                } else {
                    std::cout << "-|";
                }
            } else if (std::abs(value) == 1 && value.real() == 0) { // Clean up output when real part is +/- 1
                if (value.imag() == 1) {
                    std::cout << "i|";
                } else {
                    std::cout << "-i|";
                }
            } else {
                if(value.real() != 0 && value.imag() == 0) {
                    std::cout << std::setprecision(3) << value.real() << "|"; 
                } else if(value.real() == 0 && value.imag() != 0) {
                    std::cout << std::setprecision(3) << value.imag() << "i|"; 
                }
            }

            // Calculates string inside ket using bitset
            for (int j = qubits - 1; j >= 0; j--) {
                std::cout << (((i-1) >> j) & 1);
            }
        std::cout << "> ";
        }   
    }
}

// Print ASCII representation of the circuit
void circuit::draw() {
    // Determine which columns of the circuit register contain multi-qubit components
    std::vector<std::vector<int>> multi_gates; // Stores control and target locations of multi-qubit gates 
    std::vector<std::string> multi_gate_symbols;
    for (int j=0; j<reg.size(); j++) { 
        if (dynamic_cast<multi_component*>(reg[j][0])) {
                multi_component* gate = dynamic_cast<multi_component*>(reg[j][0]);
                int control = gate->get_control();
                int target = gate->get_target();

                std::vector<int> control_target_locations;
                control_target_locations.push_back(control);
                control_target_locations.push_back(target);
                multi_gates.push_back(control_target_locations);
                multi_gate_symbols.push_back(gate->get_symbol());
        } else {
            std::vector<int> empty_vector;
            multi_gates.push_back(empty_vector);
            multi_gate_symbols.push_back("0");
        }
    }

    // Loop over qubits in circuit, printing each qubit line in order
    for (int i=0; i<qubits; i++) {
        // Top third of line
        std::cout << "        ";
        for (int j=0; j<reg.size(); j++) { // Loop over columns in register
            if (!multi_gates[j].empty()) {   
                if (multi_gates[j][0] > multi_gates[j][1]) {
                    if (multi_gates[j][0] == i) {
                        std::cout << "    │    ";
                    } else if (multi_gates[j][1] == i) {
                        std::cout << "  ┌───┐  ";
                    } else if ((multi_gates[j][0] < i && i < multi_gates[j][1]) || (multi_gates[j][0] > i && i > multi_gates[j][1])) {
                        std::cout << "    │    ";
                    } else {
                        std::cout << "         ";
                    }
                } else {
                    if(multi_gates[j][0] == i) {
                        std::cout << "         ";
                    } else if (multi_gates[j][1] == i) {
                        std::cout << "  ┌─┴─┐  ";
                    } else if ((multi_gates[j][0] < i && i < multi_gates[j][1]) || (multi_gates[j][0] > i && i > multi_gates[j][1])) {
                        std::cout << "    │    ";
                    } else {
                        std::cout << "         ";
                    }
                }
            } else if (reg[j][i]->get_symbol() == "I") {
                std::cout << "         ";
            } else {
                std::cout << "  ┌───┐  ";
            }
        }
        std::cout << std::endl;

        // Middle third of line
        std::cout << "q" << i << ": " << "|" << initial_states[i] << "⟩ " ;
        for (int j=0; j<reg.size(); j++) { // Loop over columns in register
            if (!multi_gates[j].empty()) {
                if (multi_gates[j][0] == i) { // Target qubit
                    std::cout << "────■────";
                } else if (multi_gates[j][1] == i) { // Control qubit
                    std::cout << "──┤ " << multi_gate_symbols[j] << " ├──"; // Print component symbol in the square
                } else if ((multi_gates[j][0] < i && i < multi_gates[j][1]) || (multi_gates[j][0] > i && i > multi_gates[j][1])) {
                    std::cout << "────┼────";
                } else {
                    std::cout << "─────────";
                }
            } else if (reg[j][i]->get_symbol() == "I") {
                std::cout << "─────────";
            } else {
                std::cout << "──┤ " + reg[j][i]->get_symbol() + " ├──";
            }
        }
        std::cout << std::endl;

        // Bottom third of line
        std::cout << "        ";
        for (int j=0; j<reg.size(); j++) { // Loop over columns in register
            if (!multi_gates[j].empty()) {   
                if (multi_gates[j][0] > multi_gates[j][1]) {
                    if(multi_gates[j][0] == i) { // Target qubit
                        std::cout << "         ";
                    } else if (multi_gates[j][1] == i) { // Control qubit
                        std::cout << "  └─┬─┘  ";
                    } else if ((multi_gates[j][0] < i && i < multi_gates[j][1]) || (multi_gates[j][0] > i && i > multi_gates[j][1])) {
                        std::cout << "    │    ";
                    } else {
                        std::cout << "         ";
                    }
                } else {
                    if (multi_gates[j][0] == i) {
                        std::cout << "    │    ";
                    } else if (multi_gates[j][1] == i) {
                        std::cout << "  └───┘  ";
                    } else if ((multi_gates[j][0] < i && i < multi_gates[j][1]) || (multi_gates[j][0] > i && i > multi_gates[j][1])) {
                        std::cout << "    │    ";
                    } else {
                        std::cout << "         ";
                    }
                }
            } else if (reg[j][i]->get_symbol() == "I") {
                std::cout << "         ";  
            } else {
                std::cout << "  └───┘  ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
