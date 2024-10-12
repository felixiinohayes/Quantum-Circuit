#include<iostream>
#include<vector>
#include<cmath>
#include<algorithm>
#include<sstream>
#include<iomanip>
#include<bitset>
#include<stdexcept>
#include<limits>
#include"complex.h"
#include"matrix.h"
#include"component.h"
#include"circuit.h"
#include"input_handler.h"

// Main function
int main() {
    // Get the number of qubits from the user
    int qubits = get_qubits_from_user();
    
    // Output warning if number of qubits is high
    if (qubits > 8) {
        char yn_input;
        while (std::cout << "Warning! Selected number of qubits is large (>8). Continue? (y/n): " 
               && (!(std::cin >> yn_input) || (yn_input != 'y' && yn_input != 'n'))) {
            error_msg("Error: Invalid input.");
        }
        if (yn_input == 'n') {
            return 1;
        }
    }

    // Get initial states of each qubit from the user
    std::vector<char> initial_states = get_initial_states(qubits);
    matrix input_vector = get_input_vector(initial_states);  // Assuming you have this function somewhere

    // Predefined component library
    std::vector<std::string> comp_library = {"x", "y", "z", "h", "cx", "cy", "cz", "ch"};
    print_library(comp_library);

    // Create circuit
    circuit c{qubits, input_vector, initial_states};  // Assuming circuit class is defined

    // Add components to the circuit
    std::vector<component*> comp_added;
    add_components(c, comp_added, comp_library, qubits);

    calculate_and_display_results(c, input_vector);

    // Clean up memory (delete components)
    for (component* comp : comp_added) {
        delete comp;
    }
    
    return 0;
}