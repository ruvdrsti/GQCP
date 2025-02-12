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


#include "Mathematical/Functions/CartesianGTO.hpp"
#include "Mathematical/Functions/EvaluableLinearCombination.hpp"
#include "Molecule/Nucleus.hpp"


namespace GQCP {


/**
 *  A class that represents a shell of GTOs: it specifies in a condensed way which GTOs are on an nucleus.
 */
class GTOShell {
public:
    // The type of primitive that underlies this shell.
    using Primitive = CartesianGTO;

    // The type of basis function that this shell can produce.
    using BasisFunction = EvaluableLinearCombination<double, Primitive>;


private:
    // If this shell is considered to be 'pure', i.e. a spherical shell is pure, a Cartesian shell is not pure.
    bool pure;

    // If the normalization factors of the primitives are embedded in the contraction coefficients.
    bool are_embedded_normalization_factors_of_primitives;

    // If the total normalization factor of the contracted GTO is already embedded in the contraction coefficients.
    bool normalized;

    // The angular momentum of the shell, i.e. the sum of the Cartesian exponents of this shell.
    size_t l;

    // The nucleus on which the shell is centered.
    Nucleus m_nucleus;

    // The Gaussian exponents for this shell, i.e. the exponents for the exponential. These are shared for every contraction.
    std::vector<double> gaussian_exponents;

    // The contraction coefficients for this shell.
    std::vector<double> contraction_coefficients;


public:
    /*
     *  MARK: Constructors
     */

    /**
     *  Create a `GTOShell` from all its specifications.
     * 
     *  @param l                                                        The angular momentum of the shell, i.e. the sum of the Cartesian exponents of this shell.
     *  @param nucleus                                                  The nucleus on which the shell is centered.
     *  @param gaussian_exponents                                       The Gaussian exponents for this shell, i.e. the exponents for the exponential. These are shared for every contraction.
     *  @param contraction_coefficients                                 The contraction coefficients for this shell.
     *  @param pure                                                     If this shell is considered to be 'pure', i.e. a spherical shell is pure, a Cartesian shell is not pure.
     *  @param are_embedded_normalization_factors_of_primitives         If the normalization factors of the primitives are embedded in the contraction coefficients.
     *  @param is_normalized                                            If the total normalization factor of the contracted GTO is already embedded in the contraction coefficients.
     */
    GTOShell(const size_t l, const Nucleus& nucleus, const std::vector<double>& gaussian_exponents, const std::vector<double>& contraction_coefficients, const bool pure = true, const bool are_embedded_normalization_factors_of_primitives = false, const bool is_normalized = false);


    /*
     *  MARK: Shell characteristics
     */

    /**
     *  @return The angular momentum of this shell.
     */
    size_t angularMomentum() const { return this->l; }

    /**
     *  @return The Gaussian exponents for this shell, i.e. the exponents for the exponential. These are shared for every contraction.
     */
    const std::vector<double>& gaussianExponents() const { return this->gaussian_exponents; }

    /**
     *  @return If this shell is considered to be 'pure', i.e. a spherical shell is pure, a Cartesian shell is not pure.
     */
    bool isPure() const { return this->pure; }

    /**
     *  @return The nucleus on which this shell is centered.
     */
    const Nucleus& nucleus() const { return this->m_nucleus; }

    /**
     *  @param rhs      The right-hand side.
     *
     *  @return If this shell is considered equal to the other.
     */
    bool operator==(const GTOShell& rhs) const;


    /*
     *  MARK: Normalization
     */

    /**
     *  @return If the total normalization factor of the contracted GTO is already embedded in the contraction coefficients.
     */
    bool isNormalized() const { return this->normalized; }

    /**
     *  Embed the total normalization factor of the corresponding linear combination of spherical (or axis-aligned Cartesian) GTOs into the contraction coefficients.
     */
    void embedNormalizationFactor();

    /**
     *  @return If the normalization factors of the primitives are embedded in this shell's contraction coefficients.
     */
    bool areEmbeddedNormalizationFactorsOfPrimitives() const { return this->are_embedded_normalization_factors_of_primitives; }

    /**
     *  Embed the normalization factor of every Gaussian primitive into its corresponding contraction coefficient. If this has already been done, this function does nothing.
     *
     *  @note The normalization factor that is embedded, corresponds to the spherical (or axis-aligned Cartesian) GTO.
     */
    void embedNormalizationFactorsOfPrimitives();

    /**
     *  Un-embed the normalization factor of every Gaussian primitive into its corresponding contraction coefficient. If this has already been done, this function does nothing.
     *
     *  @note The the normalization factor that is un-embedded corresponds to the spherical (or axis-aligned Cartesian) GTO.
     */
    void unEmbedNormalizationFactorsOfPrimitives();


    /*
     *  MARK: Contractions
     */

    /**
     *  @return The contraction coefficients for this shell.
     */
    const std::vector<double>& contractionCoefficients() const { return this->contraction_coefficients; }

    /**
     *  @return The size of the contraction in the shell, i.e. the number of primitives contracted in this shell.
     */
    size_t contractionSize() const { return this->contraction_coefficients.size(); }


    /*
     *  MARK: Basis functions
     */

    /**
     *  @return The number of basis functions that this shell contains.
     */
    size_t numberOfBasisFunctions() const;

    /**
     *  @return A list of the Cartesian exponents that have this shell's angular momentum (in lexicographical ordering).
     */
    std::vector<CartesianExponents> generateCartesianExponents() const;

    /**
     *  Construct all basis functions contained in this shell.
     * 
     *  @return The basis functions that correspond to this shell.
     * 
     *  @note The basis functions are ordered lexicographically. This means x < y < z.
     */
    std::vector<BasisFunction> basisFunctions() const;
};


}  // namespace GQCP
