#ifndef CIRCUIT_H
#define CIRCUIT_H
// Class for a multi-qubit quantum circuit
class circuit
{
private:
    std::vector<std::vector<component*>> reg; // Circuit register (where components are stored)
    matrix input_vector;
    std::vector<char> initial_states; // Stores initial individual qubit states as char (0, 1, +, -).
    int matrix_size;
    int qubits;
public:
    ~circuit() {
        // Clear dynamically allocated memory
        reg.clear();
    }
    // Parameterised constructor
    circuit(int qubits, matrix input, std::vector<char> initial_states) : qubits{qubits}, input_vector{input} , initial_states{initial_states} {
        matrix_size = 1 << qubits; // Set matrix size to (2^q) using bitshifting
    }

    // Return number of qubits in circuit
    int get_qubits() const
    {
        return qubits;
    }

    // Add component to circuit at specified qubit
    void add(component* comp)
    {
        if (dynamic_cast<single_component*>(comp)) { // Checks if component is single-qubit
            identity* id = new identity(2);
            // Create vector containing identity and input components in correct order
            std::vector<component*> comp_column;
            for (int i = 0; i < qubits; i++) {
                if (i == dynamic_cast<single_component*>(comp)->get_qubit()) {
                    comp_column.push_back(comp);
                } else {
                    comp_column.push_back(id);
                }
            }
            reg.push_back(comp_column); // Add vector to register
        } else {
            // If input component is multi-qubit, vector should contain only that component
            std::vector<component*> comp_column;
            comp_column.push_back(comp);
            reg.push_back(comp_column);
        }
    }

    // Computes matrix product of current circuit
    matrix get_resultant_matrix()
    {
        // Checks if circuit is valid i.e. contains components
        if (reg.empty()) {
            throw std::logic_error("The circuit has no components.");
        }
        // Iterate over columns in register and calculate running matrix product
        matrix total_product{matrix_size,matrix_size};
        for (const auto& comp_column : reg) {
            matrix column_product = (*comp_column.begin())->get_matrix();
            auto multiply_matrix = [&](component* comp) {
                column_product = comp->get_matrix().tensor_product(column_product);
            };

            std::for_each(comp_column.begin()+1, comp_column.end(), multiply_matrix);
            total_product = column_product * total_product;
        }
        return total_product;
    }

    // Function to reorder circuit register to minimise number of columns
    void order_reg() {
        if (reg.size() == 1) {
            return;
        }
        bool moved{false};
        // Iterate over columns in register and move components to leftmost possible position
        int i = reg.size() - 1;
        for (int j = 0; j < reg[i].size(); j++) {
            if (dynamic_cast<single_component*>(reg[i][j])) { // Only move single-qubit components
                if (reg[i][j]->get_symbol() != "I") {
                    bool leftmost{false};
                    while (!leftmost && i > 0) {
                        if (reg[i-1].size() > 1) { // Checks if left neighbour column contains multi-qubit component
                            if (reg[i-1][j]->get_symbol() != "I") {
                                leftmost = true;
                            } else {
                                std::swap(reg[i][j], reg[i-1][j]);
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
            reg.pop_back(); // Delete last column in register if it's empty
        }
    }

    // Print statevector in bra-ket notation
    void print_braket(matrix statevector)
    {
        if (statevector.get_cols() != 1 || statevector.get_rows() != matrix_size) { // Checks if input statevector is valid
            throw std::invalid_argument("Invalid statevector size.");
        }
        bool first_term = true;
        for (int i = 1; i <= statevector.get_rows(); i++) {
            complex value = statevector.get_value(i,1);
            if (value.get_modulus() != 0) { // Check if vector element is 0
                if (!first_term) {
                    if (value.get_real() > 0 || (value.get_real() == 0 && value.get_imaginary() > 0))
                    std::cout << "+ ";
                } else {
                    first_term = false;
                }
                if (value.get_modulus() == 1 && value.get_imaginary() == 0) { // Clean up output when real part is +/- 1
                    if (value.get_real() == 1) {
                        std::cout << "|";
                    } else {
                        std::cout << "-|";
                    }
                } else {
                    std::cout << std::setprecision(3) << value << "|"; 
                }

                // Calculates string inside ket using bitset
                for (int j = qubits - 1; j >= 0; j--) {
                    std::cout << (((i-1) >> j) & 1);
                }
            std::cout << "> ";
            }   
        }
        std::cout << std::endl;
    }

    // Print ASCII representation of circuit
    void draw()
    {
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

        // loop over qubits in circuit, printing each qubit line in order
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
};
#endif