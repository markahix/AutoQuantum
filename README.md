# AutoQuantum

A wide variety of chemical questions involve the formation or breaking of bonds.
This necessitates the use of quantum chemistry software which can calculate the electronic structure of atoms and molecules.
AutoQuantum performs a sequence of calculations to obtain, for example, ground- and excited state optimized geometries, ground- and excited state QM/MM molecular dynamics simulations, and others. 
Because these simulations can be tedious to set up, AutoQuantum handles all inputs and outputs internally rather than requiring the end user to prepare input files beforehand. 
It also implements the [Adaptive Restraints](https://doi.org/10.1002/jcc.70127) algorithm to accelerate geometry optimizations of larger systems.

### AGIMUS Integration

When installed as part of the AGIMUS suite, AutoQuantum can receive results from the AutoAnalytics module and additional information from other modules in use. 


