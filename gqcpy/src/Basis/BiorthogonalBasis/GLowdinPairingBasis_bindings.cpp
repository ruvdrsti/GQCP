// This file is part of GQCG-GQCP.
//
// Copyright (C) 2017-2020  the GQCG developers
//
// GQCG-GQCP is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// GQCG-GQCP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with GQCG-GQCP.  If not, see <http://www.gnu.org/licenses/>.

#include "Basis/BiorthogonalBasis/GLowdinPairingBasis.hpp"
#include "gqcpy/include/interfaces.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>


namespace py = pybind11;


namespace gqcpy {


// Provide some shortcuts for frequent namespaces.
namespace py = pybind11;
using namespace GQCP;


/**
 *  Add Python bindings for some APIs related to `GLowdinPairingBasis`.
 *
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *
 *  @param py_class             The Pybind11 `class_` that should obtain APIs related to `GLowdinPairingBasis`.
 */
template <typename Class>
void bindGLowdinPairingBasisInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;
    using Scalar = typename Type::Scalar;


    /**
     *  MARK: Constructors
     */

    py_class
        .def(py::init<const GTransformation<Scalar>&, const GTransformation<Scalar>&, const ScalarGSQOneElectronOperator<Scalar>&, const size_t, const double>(),
             py::arg("C_bra"),
             py::arg("C_ket"),
             py::arg("overlap_operator"),
             py::arg("number_of_occupied_orbitals"),
             py::arg("zero_threshold") = 1e-8);


    // Expose some Mulliken API to the Python class;
    bindLowdinPairingBasisInterface(py_class);
}


/**
 *  Register `GLowdinPairingBasis_d` and `GLowdinPairingBasis_cd` to the gqcpy module and expose parts of their C++ interfaces to Python.
 *
 *  @param module           The Pybind11 module in which the classes should be registered.
 */
void bindGLowdinPairingBases(py::module& module) {

    // Define the Python class for `GLowdinPairingBasis_d`.
    py::class_<GLowdinPairingBasis<double>> py_GLowdinPairingBasis_d {module, "GLowdinPairingBasis_d", "A class that represents a real, biorthogonalized Lowdin pairing basis, created from `generalized` states."};

    bindGLowdinPairingBasisInterface(py_GLowdinPairingBasis_d);


    // Define the Python class for `GLowdinPairingBasis_cd`.
    py::class_<GLowdinPairingBasis<complex>> py_GLowdinPairingBasis_cd {module, "GLowdinPairingBasis_cd", "A class that represents a complex, biorthogonalized Lowdin pairing basis, created from `generalized` states."};

    bindGLowdinPairingBasisInterface(py_GLowdinPairingBasis_cd);
}


}  // namespace gqcpy
