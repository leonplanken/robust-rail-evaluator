# Treinonderhoud- en -rangeersimulator (TORS)
This implementation of TORS consists of a backend written in C++ (cTORS), and a front-end written in python (TORS).

## Installation

### Compile cTORS from C++ source
To compile cTORS, cmake 3.11 (or higher) is required.

Clone the repository and in the cloned folder execute the following commands:
```sh
mkdir build
cd build
cmake ..
cmake --build .
```

### Build with setuptools
Alternatively you can build cTORS and the python library with the following command
```sh
python setup.py build
python setup.py install
```
