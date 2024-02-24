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

// Function to print an error message based on an input string
void error_msg(std::string message)
{
    std::cout << message;
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << std::endl;
}

// Function to calculate input statevector from user input of initial states for each qubit
matrix get_input_vector(std::vector<char> initial_states)
{
    // Define matrices for |0> and |1> vector states
    matrix zero_ket{2,1};
    matrix one_ket{2,1};
    zero_ket.set_value(1,1,complex{1,0});
    zero_ket.set_value(2,1,complex{0,0});
    one_ket.set_value(1,1,complex{0,0});
    one_ket.set_value(2,1,complex{1,0});

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

// Function to print component library to console
void print_library(std::vector<std::string> comp_library)
{
    std::cout << "Component library: " << std::endl;
    std::cout << "[";
    // Iterate over the library and print each component followed by a comma
    for (std::vector<std::string>::iterator i = comp_library.begin(); i != comp_library.end();) {
        std::cout << *i;
        ++i;
        if (i != comp_library.end()) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl << std::endl;
}

// Function to create component library from validated user input
std::vector<std::string> library_from_input()
{
    std::cout << "Select components to add to the library (y/n)..." << std::endl;
    std::vector<std::string> comp_full_names; // Vector containing the full names of the components
    std::vector<std::string> comp_library; // Vector containing the (input) abbreviated component names
    comp_full_names.insert(comp_full_names.end(), {"Pauli-X", "Pauli-Y", "Pauli-Z", "Pauli-H", "Controlled-X", "Controlled-Y", "Controlled-Z", "Controlled-H"});
    comp_library.insert(comp_library.end(), {"x", "y", "z", "h", "cx", "cy", "cz", "ch"});


    // Iterate over components in library and erase them based on user input
    int library_counter{0};
    for (std::vector<std::string>::iterator i = comp_library.begin(); i != comp_library.end();) {
        char yn_input;
        while (std::cout << comp_full_names[library_counter] << ": " && !(std::cin >> yn_input) || (yn_input != 'y' && yn_input != 'n') ) {
            error_msg("Invalid input. Please enter 'y' or 'n': ");
        }
        if (yn_input == 'n') {
            comp_library.erase(i);
        } else {
            ++i;
        }
        library_counter++;
    } std::cout << std::endl;
    
    return comp_library;
}

// Main function
int main()
{
    // Get user input for number of qubits in circuit
    int qubits;
    while (std::cout << "Enter number of qubits in circuit: " && (!(std::cin >> qubits) || qubits < 1)) {
        error_msg("Error: Input must be a positive integer.");
    }
    // Output warning if number of qubits is higher than recommended
    if(qubits > 8) {
        char yn_input;
        while (std::cout << "Warning! Selected number of qubits is large (>8) and calculation may take a long time. Would you still like to continue? (y/n) " && !(std::cin >> yn_input) || (yn_input != 'y' && yn_input != 'n') || std::cin.peek() != '\n') {
            error_msg("Error: Invalid input. Please enter 'y' or 'n'.");
        }
        if (yn_input == 'n') {
            exit(1);
        }
    }

    // Get user input for initial state of each qubit
    std::vector<char> initial_states(qubits);
    for (int i = 0; i < qubits; i++) {
        while (std::cout << "Enter initial state of qubit " << i << " ('0' or '1'): " && (!(std::cin >> initial_states[i]) || (initial_states[i] != '0' && initial_states[i] != '1')  || std::cin.peek() != '\n')) {
            error_msg("Error: Input must be one of the following characters: 0, 1.");
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cout << std::endl;

    // Calculate input statevector based on initial states of the qubits
    matrix input_vector = get_input_vector(initial_states);

    // Create library of components from user input and print to console
    std::vector<std::string> comp_library = library_from_input();
    print_library(comp_library);

    // Create circuit
    circuit c{qubits, input_vector, initial_states};
    
    // Add components selected by user to circuit
    std::vector<component*> comp_added; // Vector containing all added components in order
    while (true) {
        std::string comp_name;
        // Get user input for component to add and ensure it is in library
        while (std::cout << "Enter name of component to add ('x', 'y', 'z', 'h', 'cx', 'cy', 'cz', 'ch' OR type '0' to finish and compute): " && (!(std::cin >> comp_name)
                    || ((std::find(comp_library.begin(), comp_library.end(), comp_name) == comp_library.end()) && !(comp_name == "0")))) {
            error_msg("Error: Component not in library.");
        }
        if (comp_name == "0") { // Exit the loop if the user inputs '0'
            if (comp_added.size() > 0) {
                break;
            } else {
                error_msg("Error: Circuit cannot be empty.");
                continue;
            }
        } else if (comp_name == "cx" || comp_name == "cy" || comp_name == "cz" || comp_name == "ch") { // Check if input is multi-qubit component
            // Get validated user input for control and target qubits
            int control_input;
            int target_input;
            while (std::cout << "Which qubit should be the control? " && (!(std::cin >> control_input) || control_input < 0 || control_input > (c.get_qubits()-1))) {
                error_msg("Error: Invalid qubit entered.");
                std::cout << std::endl;
            }
            while (std::cout << "Which qubit should be the target? " && (!(std::cin >> target_input) || target_input < 0 || target_input > (c.get_qubits()-1)) || target_input == control_input) {
                error_msg("Error: Invalid qubit entered.");
                std::cout << std::endl;
            }
            std::cout << std::endl;
            // Add user-specified component to the vector as a pointer 
            if (comp_name == "cx") {
                comp_added.push_back(new controlled_x(control_input, target_input, qubits));
            } else if (comp_name == "cy") {
                comp_added.push_back(new controlled_y(control_input, target_input, qubits));
            } else if (comp_name == "cz") {
                comp_added.push_back(new controlled_z(control_input, target_input, qubits));
            } else if (comp_name == "ch") {
                comp_added.push_back(new controlled_h(control_input, target_input, qubits));
            }
        } else { // If input is single-qubit component...
            int qubit_input;
            while (std::cout << "Which qubit should the component be applied to? (e.g '0' for qubit 0): " && (!(std::cin >> qubit_input) || qubit_input < 0 || qubit_input > (c.get_qubits()-1))) {
                error_msg("Error: Invalid qubit entered.");
            }
            std::cout << std::endl;
            // Add user-specified component to the vector as a pointer 
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
        c.add(comp_added.back()); // Add the most recently inputted component to the circuit
        c.order_reg(); // Reorder the circuit register

         // Print current circuit diagram
        std::cout << "Current circuit:" << std::endl;
        c.draw();
        print_library(comp_library);
    }
    std::cout << std::endl;
    
    // Calculate the output statevector of the circuit
    std::cout << "Performing calculation..." << std::endl << std::endl;
    matrix output_vector = c.get_resultant_matrix() * input_vector;

    // Print final circuit diagram
    std::cout << "---------- RESULTS ----------" << std::endl << std::endl;
    std::cout << "Final circuit:" << std::endl;
    c.draw();
    
    // Print input and output states in vector and bra-ket format
    std::cout << "INPUT: " << std::endl << "ψᵀ = " << input_vector.get_transpose();
    std::cout << "ψ = ";
    c.print_braket(input_vector);
    std::cout << std::endl;
    std::cout << "OUTPUT: " << std::endl << "ψᵀ = " << output_vector.get_transpose();
    std::cout << "ψ = ";
    c.print_braket(output_vector);

    // Clear dynamically allocated memory in component library
    for (std::vector<component*>::iterator i = comp_added.begin(); i != comp_added.end(); ++i) {
        delete *i;
    }
    comp_added.clear();

    return 0;
}