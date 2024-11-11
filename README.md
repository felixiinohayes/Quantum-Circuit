# Quantum Circuit Simulator
Simulates a quantum circuit with a CLI interface, allowing users to build quantum circuits using predefined quantum gates, specify an input and compute the resultant quantum state.
## Features
- User-defined number of qubits
- Supported gates: Pauli (X,Y,Z), Hadamard and their controlled counterparts (CX, CY, CZ, CH)
- Displays circuit diagram in ASCII format
- Displays quantum statevectors in Dirac bra-ket notation
## Installation
To build this project all you need is a C++ compiler that supports C++11 or higher (e.g. g++, clang++).

```bash
git clone https://github.com/felixiinohayes/Quantum-Circuit
cd Quantum-Circuit
make
```

## Example
A simple example of a 3-qubit circuit with a variety of both single and controlled quantum gates applied.

<img width="753" alt="image" src="https://github.com/user-attachments/assets/7c2ab01b-946f-4503-ad9c-30cd179e333d">
