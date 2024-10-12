#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "matrix.h"
#include "circuit.h"
#include <vector>
#include <string>
#include <iostream>
#include <complex>

void error_msg(std::string message);

matrix get_input_vector(std::vector<char> initial_states);
int get_qubits_from_user();
std::vector<char> get_initial_states(int qubits);
std::string get_component_from_user(const std::vector<std::string>& comp_library);

void print_library(const std::vector<std::string>& comp_library);
void add_components(circuit& c, std::vector<component*>& comp_added, const std::vector<std::string>& comp_library, int qubits);
void add_single_qubit_component(circuit& c, std::vector<component*>& comp_added, const std::string& comp_name, int qubits);
void add_multi_qubit_component(circuit& c, std::vector<component*>& comp_added, const std::string& comp_name, int qubits);
void calculate_and_display_results(circuit& c, const matrix& input_vector);

#endif