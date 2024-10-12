#include "input_handler.h"
#include <iostream>
#include <limits>

// Function to print an error message based on an input string
void error_msg(std::string message)
{
    std::cout << message;
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << std::endl;
}

matrix get_input_vector(std::vector<char> initial_states)
{
    // Define matrices for |0> and |1> vector states
    matrix zero_ket{2,1};
    matrix one_ket{2,1};
    zero_ket.set_value(1,1,std::complex<double>{1,0});
    zero_ket.set_value(2,1,std::complex<double>{0,0});
    one_ket.set_value(1,1,std::complex<double>{0,0});
    one_ket.set_value(2,1,std::complex<double>{1,0});

    // Construct a vector of initial states
    std::vector<matrix> product_vector;
    for (std::vector<char>::iterator i = initial_states.begin(); i != initial_states.end(); ++i) {
        if (*i == '0') {
            product_vector.push_back(zero_ket);
        } else if (*i == '1') {
            product_vector.push_back(one_ket);
        }
    }
    // Iterate over the vector and calculate the running tensor product
    matrix product = product_vector.back();
    for (int i = initial_states.size() - 2; i >= 0; i--) {
            product = product.tensor_product(product_vector[i]);
    }
    
    return product;
}

int get_qubits_from_user() {
    int qubits;
    while (std::cout << "Enter number of qubits in circuit: " && (!(std::cin >> qubits) || qubits < 1)) {
        error_msg("Error: Input must be a positive integer.");
    }
    return qubits;
}

std::vector<char> get_initial_states_from_user(int qubits) {
    std::vector<char> initial_states(qubits);
    for (int i = 0; i < qubits; i++) {
        while (std::cout << "Enter initial state of qubit " << i << " ('0' or '1'): " 
               && (!(std::cin >> initial_states[i]) || (initial_states[i] != '0' && initial_states[i] != '1'))) {
            error_msg("Error: Input must be one of the following characters: 0, 1.");
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return initial_states;
}

// Function to print component library to console
void print_library(const std::vector<std::string>& comp_library) {
    std::cout << "Component library: " << std::endl;
    std::cout << "[";
    for (auto it = comp_library.begin(); it != comp_library.end(); ++it) {
        std::cout << *it;
        if (it + 1 != comp_library.end()) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl << std::endl;
}

std::string get_component_from_user(const std::vector<std::string>& comp_library) {
    std::string comp_name;
    while (std::cout << "Enter name of component to add ('x', 'y', 'z', 'h', 'cx', 'cy', 'cz', 'ch' OR type '0' to finish and compute): " 
           && (!(std::cin >> comp_name) || (std::find(comp_library.begin(), comp_library.end(), comp_name) == comp_library.end() && comp_name != "0"))) {
        error_msg("Error: Component not in library.");
    }
    return comp_name;
}

void add_components(circuit& c, std::vector<component*>& comp_added, const std::vector<std::string>& comp_library, int qubits) {
    while (true) {
        std::string comp_name;

        // Get user input for the component to add, ensuring it's in the library
        while (std::cout << "Enter name of component to add ('x', 'y', 'z', 'h', 'cx', 'cy', 'cz', 'ch' OR type '0' to finish and compute): "
               && (!(std::cin >> comp_name)
               || (std::find(comp_library.begin(), comp_library.end(), comp_name) == comp_library.end() && comp_name != "0"))) {
            error_msg("Error: Component not in library.");
        }

        // Exit if the user inputs '0'
        if (comp_name == "0") {
            if (!comp_added.empty()) {
                break;
            } else {
                error_msg("Error: Circuit cannot be empty.");
                continue;
            }
        }

        if (comp_name == "cx" || comp_name == "cy" || comp_name == "cz" || comp_name == "ch") {
            // Multi-qubit component
            add_multi_qubit_component(c, comp_added, comp_name, qubits);
        } else {
            // Single-qubit component
            add_single_qubit_component(c, comp_added, comp_name, qubits);
        }

        // Add the most recently inputted component to the circuit
        c.add(comp_added.back());
        c.order_reg();

        // Print the current circuit diagram
        std::cout << "Current circuit:" << std::endl;
        c.draw();
        print_library(comp_library);
    }
}

// Helper function to add single-qubit components
void add_single_qubit_component(circuit& c, std::vector<component*>& comp_added, const std::string& comp_name, int qubits) {
    int qubit_input;

    // Get user input for qubit selection
    while (std::cout << "Which qubit should the component be applied to? (e.g. '0' for qubit 0): " 
           && (!(std::cin >> qubit_input) || qubit_input < 0 || qubit_input >= qubits)) {
        error_msg("Error: Invalid qubit entered.");
    }

    if (comp_name == "x") {
        comp_added.push_back(new pauli_x(qubit_input));
    } else if (comp_name == "y") {
        comp_added.push_back(new pauli_y(qubit_input));
    } else if (comp_name == "z") {
        comp_added.push_back(new pauli_z(qubit_input));
    } else if (comp_name == "h") {
        comp_added.push_back(new hadamard(qubit_input));
    }
}

// Helper function to add multi-qubit components
void add_multi_qubit_component(circuit& c, std::vector<component*>& comp_added, const std::string& comp_name, int qubits) {
    int control_input, target_input;

    // Get validated user input for control qubit
    while (std::cout << "Which qubit should be the control? " 
           && (!(std::cin >> control_input) || control_input < 0 || control_input >= qubits)) {
        error_msg("Error: Invalid control qubit entered.");
    }

    // Get validated user input for target qubit
    while (std::cout << "Which qubit should be the target? " 
           && (!(std::cin >> target_input) || target_input < 0 || target_input >= qubits || target_input == control_input)) {
        error_msg("Error: Invalid target qubit entered.");
    }

    if (comp_name == "cx") {
        comp_added.push_back(new controlled_x(control_input, target_input, qubits));
    } else if (comp_name == "cy") {
        comp_added.push_back(new controlled_y(control_input, target_input, qubits));
    } else if (comp_name == "cz") {
        comp_added.push_back(new controlled_z(control_input, target_input, qubits));
    } else if (comp_name == "ch") {
        comp_added.push_back(new controlled_h(control_input, target_input, qubits));
    }
}

void calculate_and_display_results(circuit& c, const matrix& input_vector) {
    // Calculate the output state vector of the circuit
    std::cout << "Performing calculation..." << std::endl << std::endl;
    matrix output_vector = c.get_resultant_matrix() * input_vector;

    // Print the final circuit diagram
    std::cout << "---------- RESULTS ----------" << std::endl << std::endl;
    std::cout << "Final circuit:" << std::endl;
    c.draw();
    
    // Print input state in vector and bra-ket format
    std::cout << "INPUT: " << std::endl;
    std::cout << "ψᵀ = " << input_vector.get_transpose();  // Transpose of input vector
    std::cout << "ψ = ";
    c.print_braket(input_vector);  // Bra-ket notation for input vector
    std::cout << std::endl;

    // Print output state in vector and bra-ket format
    std::cout << "OUTPUT: " << std::endl;
    std::cout << "ψᵀ = " << output_vector.get_transpose();  // Transpose of output vector
    std::cout << "ψ = ";
    c.print_braket(output_vector);  // Bra-ket notation for output vector
    std::cout << std::endl;
}