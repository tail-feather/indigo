#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

extern void init_indigo_bus(pybind11::module &module);
extern void init_indigo_client(pybind11::module &module);


PYBIND11_MODULE(__indigo, module) {
    init_indigo_bus(module);
    init_indigo_client(module);
}
