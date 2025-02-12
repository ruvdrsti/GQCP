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

#pragma once


#include "Basis/SpinorBasis/OccupationType.hpp"
#include "Basis/SpinorBasis/OrbitalSpace.hpp"
#include "Mathematical/Representation/SquareMatrix.hpp"


namespace GQCP {


/**
 *  MARK: SimpleOrbitalRotationGenerator implementation
 */

/**
 *  A set of orbital rotation generators that can be represented by a single vector.
 *   
 *  This class is used as a base class for `ROrbitalRotationGenerator` and `GOrbitalRotationGenerator`, since they are both expressed using a single vector of kappa_PQ values, as opposed to `UOrbitalRotationGenerator`, which uses separate vectors for alpha- and beta- generators. The word 'simple' is used here as an antonym for 'compound'.
 * 
 *  @tparam _Scalar                                           The scalar type used for an orbital rotation generator: real or complex.
 *  @tparam _DerivedOrbitalRotationGenerator                  The type of the orbital rotation generator that derives from this class, enabling CRTP and compile-time polymorphism.
 */
template <typename _Scalar, typename _DerivedOrbitalRotationGenerators>
class SimpleOrbitalRotationGenerators {

public:
    // The scalar type used for an orbital rotation generator: real or complex.
    using Scalar = _Scalar;

    // The type of the orbital rotation generator that derives from this class, enabling CRTP and compile-time polymorphism.
    using DerivedOrbitalRotationGenerators = _DerivedOrbitalRotationGenerators;

private:
    // The number of orbitals (spinors for the general(ized) case, spin-orbitals for the restricted and unrestricted case) that can be rotated using these orbital rotation generators.
    size_t number_of_orbitals;

    // The strict lower triangle of the kappa matrix. The entries of the matrix are stored in a `column major`format in this vector.
    VectorX<Scalar> v;


public:
    /**
     *  MARK: Constructors
     */

    /**
     *  Create a `SimpleOrbitalRotationGenerator' from a given vector containing orbital rotation generators kappa_PQ with P>Q.
     * 
     *  @param  v        The orbital rotation generators represented as a vector that corresponds to the strict lower triangle of the kappa matrix (kappa_PQ with P>Q). The entries of the matrix are stored in a `column major`format in this vector.
     */
    SimpleOrbitalRotationGenerators(const VectorX<Scalar>& v) :
        v {v},
        number_of_orbitals {strictTriangularRootOf(v.size())} {}


    /**
     *  Create a `SimpleOrbitalRotationGenerator' from a given kappa matrix.
     * 
     *  @param  kappa        The orbital rotation generators represented as a square matrix that corresponds to the full anti-Hermitian kappa matrix.
     */
    SimpleOrbitalRotationGenerators(const SquareMatrix<Scalar>& kappa) :

        SimpleOrbitalRotationGenerators(kappa.pairWiseStrictReduced()) {
        if (!kappa.isAntiHermitian()) {
            throw std::invalid_argument("SimpleOrbitalRotationGenerators(const SquareMatrix<Scalar>& kappa): The given kappa matrix is not anti-Hermitian.");
        }
    }


    /**
     *  MARK: Named constructors
     */

    /**
     *  Construct orbital rotation generators by adding redundant (i.e. 0) generators to the given occupation_type - occupation_type generators.
     * 
     *  @param generators                           The orbital rotation generators of the specified occupation types.
     *  @param row_occupation_type                  The occupation type of the rows of the orbital rotation generator kappa matrix.
     *  @param column_occupation_type               The occupation type of the column of the orbital rotation generator kappa matrix.
     *  @param K                                    The total number of orbitals. In the general(ized) case these are spinors, for restricted/unrestricted these will be spin-orbitals.
     * 
     *  @return The 'full' orbital rotation generators from the given row_occupation_type - column_occupation_type generators.
     */
    static DerivedOrbitalRotationGenerators FromOccupationTypes(const DerivedOrbitalRotationGenerators& generators, const OccupationType row_occupation_type, const OccupationType column_occupation_type, const size_t K) {
        if (row_occupation_type != column_occupation_type) {
            throw std::invalid_argument("SimpleOrbitalRotationGenerators::FromOccupationTypes(const DerivedOrbitalRotationGenerators& generators, const OccupationType row_occupation_type, const OccupationType column_occupation_type, const size_t K): Occupied/virtual and virtual/occupied rotations are currently disabled. The row and column occupation types should be the same.");
        }

        // The total number of orbitals determines the size of the total kappa matrix.
        SquareMatrix<Scalar> kappa = SquareMatrix<Scalar>::Zero(K);

        // Depending on the row and column occupation types, we fill in the correct block of the total kappa matrix and leave the rest to be zero.
        if (row_occupation_type == OccupationType::k_occupied && column_occupation_type == OccupationType::k_occupied) {
            kappa.topLeftCorner(generators.numberOfOrbitals(), generators.numberOfOrbitals()) = generators.asMatrix();
        } else {
            kappa.bottomRightCorner(generators.numberOfOrbitals(), generators.numberOfOrbitals()) = generators.asMatrix();
        }

        return DerivedOrbitalRotationGenerators(kappa);
    }


    /**
     *  MARK: Access
     */

    /**
     *  @return The anti-Hermitian orbital rotation generator matrix kappa.
     */
    const SquareMatrix<Scalar> asMatrix() const {

        const auto kappa = SquareMatrix<Scalar>::FromStrictTriangle(this->v);  // Lower triangle only.

        return kappa - kappa.adjoint();
    }

    /**
     *  @return The orbital rotation generators as the strict lower triangle of the kappa matrix.
     */
    const VectorX<Scalar>& asVector() const { return this->v; }


    /**
     *  @return The number of spin-orbitals that can be rotated using these orbital rotation generators.
     */
    size_t numberOfOrbitals() const { return this->number_of_orbitals; }
};


}  // namespace GQCP
