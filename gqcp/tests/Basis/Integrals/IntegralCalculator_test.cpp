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

#define BOOST_TEST_MODULE "IntegralCalculator"

#include <boost/test/unit_test.hpp>

#include "Basis/Integrals/IntegralCalculator.hpp"
#include "Basis/ScalarBasis/ScalarBasis.hpp"
#include "Molecule/Molecule.hpp"


/**
 *  Check integrals calculated by Libint with reference values in Szabo.
 */
BOOST_AUTO_TEST_CASE(Szabo_integrals_h2_sto3g) {

    // In Szabo, section 3.5.2, we read that the internuclear distance R = 1.4 a.u. = 0.740848 Angstrom
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2_szabo.xyz");
    const GQCP::ScalarBasis<GQCP::GTOShell> scalar_basis {molecule, "STO-3G"};
    BOOST_CHECK_EQUAL(scalar_basis.numberOfBasisFunctions(), 2);


    // Let Libint2 calculate some integrals.
    const auto S = GQCP::IntegralCalculator::calculateLibintIntegrals(GQCP::OverlapOperator(), scalar_basis);
    const auto T = GQCP::IntegralCalculator::calculateLibintIntegrals(GQCP::KineticOperator(), scalar_basis);
    const auto V = GQCP::IntegralCalculator::calculateLibintIntegrals(GQCP::NuclearAttractionOperator(molecule.nuclearFramework()), scalar_basis);
    const GQCP::SquareMatrix<double> H_core = T + V;

    const auto g = GQCP::IntegralCalculator::calculateLibintIntegrals(GQCP::CoulombRepulsionOperator(), scalar_basis);


    // Check the one-electron integrals with the reference.
    GQCP::SquareMatrix<double> ref_S {2};
    // clang-format off
    ref_S << 1.0,    0.6593,
             0.6593, 1.0;
    // clang-format on

    GQCP::SquareMatrix<double> ref_T {2};
    // clang-format off
    ref_T << 0.7600, 0.2365,
             0.2365, 0.7600;
    // clang-format on

    GQCP::SquareMatrix<double> ref_H_core {2};
    // clang-format off
    ref_H_core << -1.1204, -0.9584,
                  -0.9584, -1.1204;
    // clang-format on

    BOOST_CHECK(S.isApprox(ref_S, 1.0e-04));
    BOOST_CHECK(T.isApprox(ref_T, 1.0e-04));
    BOOST_CHECK(H_core.isApprox(ref_H_core, 1.0e-04));


    // Check the two-electron integrals with the reference
    // The two-electron integrals in Szabo are given in chemist's notation, so this confirms that AO basis gives them in chemist's notation as well
    BOOST_CHECK(std::abs(g(0, 0, 0, 0) - 0.7746) < 1.0e-04);
    BOOST_CHECK(std::abs(g(0, 0, 0, 0) - g(1, 1, 1, 1)) < 1.0e-12);

    BOOST_CHECK(std::abs(g(0, 0, 1, 1) - 0.5697) < 1.0e-04);

    BOOST_CHECK(std::abs(g(1, 0, 0, 0) - 0.4441) < 1.0e-04);
    BOOST_CHECK(std::abs(g(1, 0, 0, 0) - g(1, 1, 1, 0)) < 1.0e-12);

    BOOST_CHECK(std::abs(g(1, 0, 1, 0) - 0.2970) < 1.0e-04);
}


/**
 *  Check integrals calculated by Libint with reference values from HORTON.
 */
BOOST_AUTO_TEST_CASE(HORTON_integrals_h2o_sto3g) {

    // Set up an AO basis
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");
    const GQCP::ScalarBasis<GQCP::GTOShell> scalar_basis {molecule, "STO-3G"};
    const auto nbf = scalar_basis.numberOfBasisFunctions();


    // Calculate some integrals.
    const auto S = GQCP::IntegralCalculator::calculateLibintIntegrals(GQCP::OverlapOperator(), scalar_basis);
    const auto T = GQCP::IntegralCalculator::calculateLibintIntegrals(GQCP::KineticOperator(), scalar_basis);
    const auto V = GQCP::IntegralCalculator::calculateLibintIntegrals(GQCP::NuclearAttractionOperator(molecule.nuclearFramework()), scalar_basis);
    const auto g = GQCP::IntegralCalculator::calculateLibintIntegrals(GQCP::CoulombRepulsionOperator(), scalar_basis);


    // Read in reference data from HORTON.
    const auto ref_S = GQCP::SquareMatrix<double>::FromFile("data/h2o_sto-3g_overlap_horton.data", nbf, nbf);
    const auto ref_T = GQCP::SquareMatrix<double>::FromFile("data/h2o_sto-3g_kinetic_horton.data", nbf, nbf);
    const auto ref_V = GQCP::SquareMatrix<double>::FromFile("data/h2o_sto-3g_nuclear_horton.data", nbf, nbf);
    const auto ref_g = GQCP::SquareRankFourTensor<double>::FromFile("data/h2o_sto-3g_coulomb_horton.data", nbf);


    // Check if the calculated integrals are close to those of HORTON
    BOOST_CHECK(S.isApprox(ref_S, 1.0e-07));
    BOOST_CHECK(T.isApprox(ref_T, 1.0e-07));
    BOOST_CHECK(V.isApprox(ref_V, 1.0e-07));
    BOOST_CHECK(g.isApprox(ref_g, 1.0e-06));
}


// The following test has been commented out as this test has been shown to fail on the current Docker infrastructure.
/**
 *  Check the calculation of some integrals between Libint2 and libcint.
 */
/* 
BOOST_AUTO_TEST_CASE(libcint_vs_libint2_H2O_STO_3G) {

    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");
    const GQCP::ScalarBasis<GQCP::GTOShell> scalar_basis {molecule, "STO-3G"};


    const auto S_libint2 = GQCP::IntegralCalculator::calculateLibintIntegrals(GQCP::OverlapOperator(), scalar_basis);
    const auto T_libint2 = GQCP::IntegralCalculator::calculateLibintIntegrals(GQCP::KineticOperator(), scalar_basis);
    const auto V_libint2 = GQCP::IntegralCalculator::calculateLibintIntegrals(GQCP::NuclearAttractionOperator(molecule.nuclearFramework()), scalar_basis);
    const auto dipole_libint2 = GQCP::IntegralCalculator::calculateLibintIntegrals(GQCP::ElectronicDipoleOperator(), scalar_basis);
    const auto g_libint2 = GQCP::IntegralCalculator::calculateLibintIntegrals(GQCP::CoulombRepulsionOperator(), scalar_basis);

    const auto S_libcint = GQCP::IntegralCalculator::calculateLibcintIntegrals(GQCP::OverlapOperator(), scalar_basis);
    const auto T_libcint = GQCP::IntegralCalculator::calculateLibcintIntegrals(GQCP::KineticOperator(), scalar_basis);
    const auto V_libcint = GQCP::IntegralCalculator::calculateLibcintIntegrals(GQCP::NuclearAttractionOperator(molecule.nuclearFramework()), scalar_basis);
    const auto dipole_libcint = GQCP::IntegralCalculator::calculateLibcintIntegrals(GQCP::ElectronicDipoleOperator(), scalar_basis);
    const auto g_libcint = GQCP::IntegralCalculator::calculateLibcintIntegrals(GQCP::CoulombRepulsionOperator(), scalar_basis);


    BOOST_CHECK(S_libcint.isApprox(S_libint2, 1.0e-08));
    BOOST_CHECK(T_libcint.isApprox(T_libint2, 1.0e-08));
    BOOST_CHECK(V_libcint.isApprox(V_libint2, 1.0e-08));
    for (size_t i = 0; i < 3; i++) {
        BOOST_CHECK(dipole_libcint[i].isApprox(dipole_libint2[i], 1.0e-08));
    }
    BOOST_CHECK(g_libcint.isApprox(g_libint2, 1.0e-08));
}
*/

// The following test has been commented out as this test has been shown to fail on the current Docker infrastructure.
/**
 *  Check the dipole integrals between libcint and libint2 for an origin different from zero.
 */
/*
BOOST_AUTO_TEST_CASE(libcint_vs_libint2_dipole_origin) {

    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");
    const GQCP::ScalarBasis<GQCP::GTOShell> scalar_basis {molecule, "STO-3G"};

    const GQCP::Vector<double, 3> origin {0.0, 1.0, -0.5};

    const auto dipole_libint2 = GQCP::IntegralCalculator::calculateLibintIntegrals(GQCP::ElectronicDipoleOperator(origin), scalar_basis);
    const auto dipole_libcint = GQCP::IntegralCalculator::calculateLibcintIntegrals(GQCP::ElectronicDipoleOperator(origin), scalar_basis);

    for (size_t i = 0; i < 3; i++) {
        BOOST_CHECK(dipole_libcint[i].isApprox(dipole_libint2[i], 1.0e-08));
    }
}
*/


/*
 *  MARK: In-house Cartesian GTO integrals
 */

/**
 *  Check if our implementation of the overlap integrals yields the same result as Libint.
 */
BOOST_AUTO_TEST_CASE(overlap_integrals) {

    // Set up an AO basis.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");
    const GQCP::ScalarBasis<GQCP::GTOShell> scalar_basis {molecule, "STO-3G"};


    // Calculate the overlap integrals and check if they are equal.
    const auto ref_S = GQCP::IntegralCalculator::calculateLibintIntegrals(GQCP::OverlapOperator(), scalar_basis);

    auto engine = GQCP::IntegralEngine::InHouse<GQCP::GTOShell>(GQCP::OverlapOperator());
    const auto S = GQCP::IntegralCalculator::calculate(engine, scalar_basis.shellSet(), scalar_basis.shellSet())[0];

    BOOST_CHECK(S.isApprox(ref_S, 1.0e-12));
}


/**
 *  Check if our implementation of the kinetic energy integrals yields the same result as Libint.
 */
BOOST_AUTO_TEST_CASE(kinetic_energy_integrals) {

    // Set up an AO basis.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");
    const GQCP::ScalarBasis<GQCP::GTOShell> scalar_basis {molecule, "STO-3G"};


    // Calculate the kinetic energy integrals and check if they are equal.
    const auto ref_T = GQCP::IntegralCalculator::calculateLibintIntegrals(GQCP::KineticOperator(), scalar_basis);

    auto engine = GQCP::IntegralEngine::InHouse<GQCP::GTOShell>(GQCP::KineticOperator());
    const auto T = GQCP::IntegralCalculator::calculate(engine, scalar_basis.shellSet(), scalar_basis.shellSet())[0];

    BOOST_CHECK(T.isApprox(ref_T, 1.0e-12));
}


/**
 *  Check if our implementation of the nuclear attraction integrals yields the same result as Libint.
 */
BOOST_AUTO_TEST_CASE(nuclear_attraction_integrals) {

    // Set up an AO basis.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");
    const GQCP::ScalarBasis<GQCP::GTOShell> scalar_basis {molecule, "STO-3G"};
    const auto op = GQCP::NuclearAttractionOperator(molecule.nuclearFramework());

    // Calculate the nuclear attraction integrals and check if they are equal.
    const auto ref_V = GQCP::IntegralCalculator::calculateLibintIntegrals(op, scalar_basis);

    auto engine = GQCP::IntegralEngine::InHouse<GQCP::GTOShell>(op);
    const auto V = GQCP::IntegralCalculator::calculate(engine, scalar_basis.shellSet(), scalar_basis.shellSet())[0];

    BOOST_CHECK(V.isApprox(ref_V, 1.0e-12));
}


/**
 *  Check if our implementation of the electronic dipole integrals yields the same result as Libint.
 */
BOOST_AUTO_TEST_CASE(electronic_dipole_integrals) {

    // Set up an AO basis.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");
    const GQCP::ScalarBasis<GQCP::GTOShell> scalar_basis {molecule, "STO-3G"};


    // Calculate the electronic dipole integrals (with a non-zero origin) and check if they are equal.
    const GQCP::Vector<double, 3> origin {0.0, 1.0, -0.5};
    const auto ref_dipole_integrals = GQCP::IntegralCalculator::calculateLibintIntegrals(GQCP::ElectronicDipoleOperator(origin), scalar_basis);

    auto engine = GQCP::IntegralEngine::InHouse<GQCP::GTOShell>(GQCP::ElectronicDipoleOperator(origin));
    const auto dipole_integrals = GQCP::IntegralCalculator::calculate(engine, scalar_basis.shellSet(), scalar_basis.shellSet());

    for (size_t i = 0; i < 3; i++) {
        BOOST_CHECK(dipole_integrals[i].isApprox(ref_dipole_integrals[i], 1.0e-12));
    }
}


/**
 *  Check if our implementation of the linear momentum integrals are correct.
 * 
 *  The reference integrals have been calculated by transposing and multiplying by (-i) the intor='int1e_ipovlp' libcint/PySCF integrals.
 */
BOOST_AUTO_TEST_CASE(linear_momentum_integrals) {

    // Set up an AO basis.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");
    const GQCP::ScalarBasis<GQCP::GTOShell> scalar_basis {molecule, "STO-3G"};


    // Provide the reference linear momentum integrals.
    GQCP::MatrixX<GQCP::complex> ref_px {7, 7};
    // clang-format off
    ref_px << GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -1.00208382876844e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -3.81198221642141e-02), GQCP::complex(0, +3.81198221642141e-02),
              GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -6.52384267435559e-01), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -2.43165315952177e-01), GQCP::complex(0, +2.43165315952177e-01),
              GQCP::complex(0, +1.00208382876844e+00), GQCP::complex(0, +6.52384267435559e-01), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -1.40834875742559e-17), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -6.37270069213999e-02), GQCP::complex(0, -6.37270069213999e-02),
              GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, +1.40834875742559e-17), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -1.77825513632359e-01), GQCP::complex(0, +1.77825513632359e-01),
              GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00),
              GQCP::complex(0, +3.81198221642141e-02), GQCP::complex(0, +2.43165315952177e-01), GQCP::complex(0, +6.37270069213999e-02), GQCP::complex(0, +1.77825513632359e-01), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, +1.46856864757787e-01),
              GQCP::complex(0, -3.81198221642141e-02), GQCP::complex(0, -2.43165315952177e-01), GQCP::complex(0, +6.37270069213999e-02), GQCP::complex(0, -1.77825513632359e-01), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -1.46856864757787e-01), GQCP::complex(0, -0.00000000000000e+00);
    // clang-format on

    GQCP::MatrixX<GQCP::complex> ref_py {7, 7};
    // clang-format off
    ref_py << GQCP::complex(0, +4.34797300312354e-16), GQCP::complex(0, -1.21973924110322e-17), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -1.00208382876844e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -2.97826365227336e-02), GQCP::complex(0, -2.97826365227336e-02),
              GQCP::complex(0, +1.19442746331320e-17), GQCP::complex(0, -1.92158568634595e-17), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -6.52384267435559e-01), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -1.89982633936264e-01), GQCP::complex(0, -1.89982633936264e-01),
              GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -1.40834875742559e-17), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -1.77825513632359e-01), GQCP::complex(0, +1.77825513632359e-01),
              GQCP::complex(0, +1.00208382876844e+00), GQCP::complex(0, +6.52384267435559e-01), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -1.40834875742559e-17), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, +2.49446934333875e-02), GQCP::complex(0, +2.49446934333875e-02),
              GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -1.40834875742559e-17), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00),
              GQCP::complex(0, +2.97826365227336e-02), GQCP::complex(0, +1.89982633936264e-01), GQCP::complex(0, +1.77825513632359e-01), GQCP::complex(0, -2.49446934333875e-02), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, +9.11512715720799e-17), GQCP::complex(0, +3.16029272460888e-19),
              GQCP::complex(0, +2.97826365227336e-02), GQCP::complex(0, +1.89982633936264e-01), GQCP::complex(0, -1.77825513632359e-01), GQCP::complex(0, -2.49446934333875e-02), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, +3.16029272460888e-19), GQCP::complex(0, +9.11512715720799e-17);
    // clang-format on

    GQCP::MatrixX<GQCP::complex> ref_pz {7, 7};
    // clang-format off
    ref_pz << GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -1.00208382876844e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00),
              GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -6.52384267435559e-01), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00),
              GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00),
              GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, +1.40834875742559e-17), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00),
              GQCP::complex(0, +1.00208382876844e+00), GQCP::complex(0, +6.52384267435559e-01), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -1.40834875742559e-17), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, +1.63877992076968e-01), GQCP::complex(0, +1.63877992076968e-01),
              GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -1.63877992076968e-01), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00),
              GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -1.63877992076968e-01), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00);
    // clang-format on

    const std::array<GQCP::MatrixX<GQCP::complex>, 3> ref_linear_momentum_integrals {ref_px, ref_py, ref_pz};


    // Calculate our own linear momentum integrals and check if they are correct.
    auto engine = GQCP::IntegralEngine::InHouse<GQCP::GTOShell>(GQCP::LinearMomentumOperator());
    const auto linear_momentum_integrals = GQCP::IntegralCalculator::calculate(engine, scalar_basis.shellSet(), scalar_basis.shellSet());


    for (size_t i = 0; i < 3; i++) {
        BOOST_CHECK(linear_momentum_integrals[i].isApprox(ref_linear_momentum_integrals[i], 1.0e-07));
    }
}


/**
 *  Check if our implementation of the angular momentum integrals are correct.
 * 
 *  The reference integrals have been calculated by multiplying by (-i) the intor='int1e_cg_irxp' libcint/PySCF integrals.
 */
BOOST_AUTO_TEST_CASE(angular_momentum_integrals) {

    // Set up an AO basis.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");
    const GQCP::ScalarBasis<GQCP::GTOShell> scalar_basis {molecule, "STO-3G"};


    // Provide the reference angular momentum integrals.
    GQCP::MatrixX<GQCP::complex> ref_lx {7, 7};
    // clang-format off
    ref_lx << GQCP::complex(0, -2.17398650156177e-16), GQCP::complex(0, +6.09869620551612e-18), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, +5.01041914384220e-01), GQCP::complex(0, +1.14560462258781e+00), GQCP::complex(0, +1.48913182613668e-02), GQCP::complex(0, +1.48913182613668e-02),
              GQCP::complex(0, -5.97213731656600e-18), GQCP::complex(0, +9.60792843172973e-18), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, +3.26192133717779e-01), GQCP::complex(0, +7.45820270741483e-01), GQCP::complex(0, +9.49913169681320e-02), GQCP::complex(0, +9.49913169681320e-02),
              GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, +7.04174378712795e-18), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, +8.89127568161794e-02), GQCP::complex(0, -8.89127568161794e-02),
              GQCP::complex(0, -5.01041914384219e-01), GQCP::complex(0, -3.26192133717779e-01), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, +7.04174378712795e-18), GQCP::complex(0, -1.00000000000000e+00), GQCP::complex(0, -1.24723467166937e-02), GQCP::complex(0, -1.24723467166937e-02),
              GQCP::complex(0, -1.14560462258781e+00), GQCP::complex(0, -7.45820270741483e-01), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, +1.00000000000000e+00), GQCP::complex(0, +7.04174378712795e-18), GQCP::complex(0, +2.23786673574312e-02), GQCP::complex(0, +2.23786673574312e-02),
              GQCP::complex(0, -1.48913182613667e-02), GQCP::complex(0, -9.49913169681320e-02), GQCP::complex(0, -8.89127568161794e-02), GQCP::complex(0, +1.24723467166938e-02), GQCP::complex(0, -2.23786673574311e-02), GQCP::complex(0, -4.55756357860399e-17), GQCP::complex(0, -1.58014636230444e-19),
              GQCP::complex(0, -1.48913182613667e-02), GQCP::complex(0, -9.49913169681320e-02), GQCP::complex(0, +8.89127568161794e-02), GQCP::complex(0, +1.24723467166938e-02), GQCP::complex(0, -2.23786673574311e-02), GQCP::complex(0, -1.58014636230444e-19), GQCP::complex(0, -4.55756357860399e-17);
    // clang-format on

    GQCP::MatrixX<GQCP::complex> ref_ly {7, 7};
    // clang-format off
    ref_ly << GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -5.01041914384220e-01), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -1.90599110821070e-02), GQCP::complex(0, +1.90599110821070e-02),
              GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -3.26192133717779e-01), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -1.21582657976089e-01), GQCP::complex(0, +1.21582657976089e-01),
              GQCP::complex(0, +5.01041914384219e-01), GQCP::complex(0, +3.26192133717779e-01), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -7.04174378712795e-18), GQCP::complex(0, +1.00000000000000e+00), GQCP::complex(0, -3.18635034606999e-02), GQCP::complex(0, -3.18635034606999e-02),
              GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, +7.04174378712795e-18), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -8.89127568161794e-02), GQCP::complex(0, +8.89127568161794e-02),
              GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -1.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -2.68437641268176e-01), GQCP::complex(0, +2.68437641268176e-01),
              GQCP::complex(0, +1.90599110821070e-02), GQCP::complex(0, +1.21582657976089e-01), GQCP::complex(0, +3.18635034607000e-02), GQCP::complex(0, +8.89127568161794e-02), GQCP::complex(0, +2.68437641268176e-01), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, +7.34284323788936e-02),
              GQCP::complex(0, -1.90599110821070e-02), GQCP::complex(0, -1.21582657976089e-01), GQCP::complex(0, +3.18635034607000e-02), GQCP::complex(0, -8.89127568161794e-02), GQCP::complex(0, -2.68437641268176e-01), GQCP::complex(0, -7.34284323788936e-02), GQCP::complex(0, -0.00000000000000e+00);
    // clang-format on

    GQCP::MatrixX<GQCP::complex> ref_lz {7, 7};
    // clang-format off
    ref_lz << GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -1.14560462258781e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -4.35794324085837e-02), GQCP::complex(0, +4.35794324085837e-02),
              GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -7.45820270741483e-01), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -2.77992022234511e-01), GQCP::complex(0, +2.77992022234511e-01),
              GQCP::complex(0, +1.14560462258781e+00), GQCP::complex(0, +7.45820270741483e-01), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -1.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -2.82581787586485e-01), GQCP::complex(0, -2.82581787586485e-01),
              GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, +1.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, +6.51435409316292e-02), GQCP::complex(0, -6.51435409316292e-02),
              GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, -0.00000000000000e+00),
              GQCP::complex(0, +4.35794324085837e-02), GQCP::complex(0, +2.77992022234511e-01), GQCP::complex(0, +2.82581787586485e-01), GQCP::complex(0, -6.51435409316292e-02), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, +1.49308836588088e-16), GQCP::complex(0, -2.00543153105404e-02),
              GQCP::complex(0, -4.35794324085837e-02), GQCP::complex(0, -2.77992022234511e-01), GQCP::complex(0, +2.82581787586485e-01), GQCP::complex(0, +6.51435409316292e-02), GQCP::complex(0, -0.00000000000000e+00), GQCP::complex(0, +2.00543153105404e-02), GQCP::complex(0, -1.49308836588088e-16);
    // clang-format on

    const std::array<GQCP::MatrixX<GQCP::complex>, 3> ref_angular_momentum_integrals {ref_lx, ref_ly, ref_lz};


    // Calculate our own angular momentum integrals (with respect to a reference different from the origin) and check if they are correct.
    const GQCP::Vector<double, 3> origin {0.0, 1.0, -0.5};
    auto engine = GQCP::IntegralEngine::InHouse<GQCP::GTOShell>(GQCP::AngularMomentumOperator(origin));
    const auto angular_momentum_integrals = GQCP::IntegralCalculator::calculate(engine, scalar_basis.shellSet(), scalar_basis.shellSet());


    for (size_t i = 0; i < 3; i++) {
        BOOST_CHECK(angular_momentum_integrals[i].isApprox(ref_angular_momentum_integrals[i], 1.0e-07));
    }
}


/*
 *  Check if our implementation of the Coulomb repulsion integrals yields the same result as Libint.
 */
BOOST_AUTO_TEST_CASE(Coulomb_repulsion_integrals) {

    // Set up an AO basis.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");
    const GQCP::ScalarBasis<GQCP::GTOShell> scalar_basis {molecule, "STO-3G"};


    // Calculate the Coulomb repulsion integrals and check if they are equal.
    const auto op = GQCP::CoulombRepulsionOperator();
    const auto ref_g = GQCP::IntegralCalculator::calculateLibintIntegrals(op, scalar_basis);

    auto engine = GQCP::IntegralEngine::InHouse<GQCP::GTOShell>(op);
    const auto g = GQCP::IntegralCalculator::calculate(engine, scalar_basis.shellSet(), scalar_basis.shellSet())[0];

    BOOST_CHECK(g.isApprox(ref_g, 1.0e-12));
}


/*
 *  MARK: In-house London Cartesian GTO integrals
 */

/**
 *  Check if the London overlap integrals are gauge invariant.
 */
BOOST_AUTO_TEST_CASE(London_S_gauge_invariant) {

    // Set up a scalar basis with London GTO shells.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2_szabo.xyz");

    const GQCP::HomogeneousMagneticField B1 {{0.0, 0.0, 1.0}};  // Gauge origin at the origin.
    const GQCP::ScalarBasis<GQCP::LondonGTOShell> scalar_basis1 {molecule, "STO-3G", B1};

    auto engine = GQCP::IntegralEngine::InHouse<GQCP::LondonGTOShell>(GQCP::OverlapOperator());
    const auto S1 = GQCP::IntegralCalculator::calculate(engine, scalar_basis1.shellSet(), scalar_basis1.shellSet())[0];


    const GQCP::HomogeneousMagneticField B2 {{0.0, 0.0, 1.0}, {5.0, 5.0, 5.0}};  // Gauge origin at (5.0, 5.0, 5.0).
    const GQCP::ScalarBasis<GQCP::LondonGTOShell> scalar_basis2 {molecule, "STO-3G", B1};

    const auto S2 = GQCP::IntegralCalculator::calculate(engine, scalar_basis2.shellSet(), scalar_basis2.shellSet())[0];

    BOOST_CHECK(S1.isApprox(S2, 1.0e-12));
}


/**
 *  Check if the London overlap integrals are implemented correctly, for a magnetic field of B=(0,0,1).
 * 
 *  The references values are by generated through ChronusQ.
 */
BOOST_AUTO_TEST_CASE(London_overlap_001) {

    // Set up a scalar basis with London GTO shells.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");

    const GQCP::HomogeneousMagneticField B {{0.0, 0.0, 1.0}};  // Gauge origin at the origin.
    const GQCP::ScalarBasis<GQCP::LondonGTOShell> scalar_basis {molecule, "6-31G", B};
    const auto nbf = scalar_basis.numberOfBasisFunctions();

    // Calculate the overlap integrals through our own engines.
    auto engine = GQCP::IntegralEngine::InHouse<GQCP::LondonGTOShell>(GQCP::OverlapOperator());
    const auto S = GQCP::IntegralCalculator::calculate(engine, scalar_basis.shellSet(), scalar_basis.shellSet())[0];


    // Read in the reference values.
    const auto S_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_overlap_chronusq_real.data", nbf, nbf);
    const auto S_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_overlap_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> S_ref = S_ref_real + std::complex<double>(0, 1) * S_ref_complex;

    BOOST_CHECK(S.isApprox(S_ref, 1.0e-12));
}


/**
 *  Check if the London overlap integrals are implemented correctly, for a magnetic field of B=(1,1,1).
 * 
 *  The references values are by generated through ChronusQ.
 */
BOOST_AUTO_TEST_CASE(London_overlap_111) {

    // Set up a scalar basis with London GTO shells.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");

    const GQCP::HomogeneousMagneticField B {{1.0, 1.0, 1.0}};  // Gauge origin at the origin.
    const GQCP::ScalarBasis<GQCP::LondonGTOShell> scalar_basis {molecule, "6-31G", B};
    const auto nbf = scalar_basis.numberOfBasisFunctions();

    // Calculate the overlap integrals through our own engines.
    auto engine = GQCP::IntegralEngine::InHouse<GQCP::LondonGTOShell>(GQCP::OverlapOperator());
    const auto S = GQCP::IntegralCalculator::calculate(engine, scalar_basis.shellSet(), scalar_basis.shellSet())[0];


    // Read in the reference values.
    const auto S_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_overlap_chronusq_real.data", nbf, nbf);
    const auto S_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_overlap_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> S_ref = S_ref_real + std::complex<double>(0, 1) * S_ref_complex;

    BOOST_CHECK(S.isApprox(S_ref, 1.0e-12));
}


/**
 *  Check if the London canonical kinetic integrals are implemented correctly, for a magnetic field of B=(0,0,1).
 * 
 *  The references values are by generated through ChronusQ.
 */
BOOST_AUTO_TEST_CASE(London_canonical_kinetic_001) {

    // Set up a scalar basis with London GTO shells.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");

    const GQCP::HomogeneousMagneticField B {{0.0, 0.0, 1.0}};  // Gauge origin at the origin.
    const GQCP::ScalarBasis<GQCP::LondonGTOShell> scalar_basis {molecule, "6-31G", B};
    const auto nbf = scalar_basis.numberOfBasisFunctions();

    // Calculate the overlap integrals through our own engines.
    auto engine = GQCP::IntegralEngine::InHouse<GQCP::LondonGTOShell>(GQCP::KineticOperator());
    const auto T = GQCP::IntegralCalculator::calculate(engine, scalar_basis.shellSet(), scalar_basis.shellSet())[0];


    // Read in the reference values.
    const auto T_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_kinetic_chronusq_real.data", nbf, nbf);
    const auto T_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_kinetic_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> T_ref = T_ref_real + std::complex<double>(0, 1) * T_ref_complex;

    BOOST_CHECK(T.isApprox(T_ref, 1.0e-12));
}


/**
 *  Check if the London canonical kinetic integrals are implemented correctly, for a magnetic field of B=(1,1,1).
 * 
 *  The references values are by generated through ChronusQ.
 */
BOOST_AUTO_TEST_CASE(London_canonical_kinetic_111) {

    // Set up a scalar basis with London GTO shells.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");

    const GQCP::HomogeneousMagneticField B {{1.0, 1.0, 1.0}};  // Gauge origin at the origin.
    const GQCP::ScalarBasis<GQCP::LondonGTOShell> scalar_basis {molecule, "6-31G", B};
    const auto nbf = scalar_basis.numberOfBasisFunctions();

    // Calculate the overlap integrals through our own engines.
    auto engine = GQCP::IntegralEngine::InHouse<GQCP::LondonGTOShell>(GQCP::KineticOperator());
    const auto T = GQCP::IntegralCalculator::calculate(engine, scalar_basis.shellSet(), scalar_basis.shellSet())[0];


    // Read in the reference values.
    const auto T_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_kinetic_chronusq_real.data", nbf, nbf);
    const auto T_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_kinetic_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> T_ref = T_ref_real + std::complex<double>(0, 1) * T_ref_complex;

    BOOST_CHECK(T.isApprox(T_ref, 1.0e-12));
}


/**
 *  Check if the London electronic dipole integrals are implemented correctly, for a magnetic field of B=(0,0,1).
 * 
 *  The references values are by generated through ChronusQ, which has implemented position integrals, i.e. we expect our results to differ with a factor (-1).
 */
BOOST_AUTO_TEST_CASE(London_electronic_dipole_001) {

    // Set up a scalar basis with London GTO shells.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");

    const GQCP::HomogeneousMagneticField B {{0.0, 0.0, 1.0}};  // Gauge origin at the origin.
    const GQCP::ScalarBasis<GQCP::LondonGTOShell> scalar_basis {molecule, "6-31G", B};
    const auto nbf = scalar_basis.numberOfBasisFunctions();

    // Calculate the overlap integrals through our own engines.
    auto engine = GQCP::IntegralEngine::InHouse<GQCP::LondonGTOShell>(GQCP::ElectronicDipoleOperator());  // Reference point at the origin.
    const auto integrals = GQCP::IntegralCalculator::calculate(engine, scalar_basis.shellSet(), scalar_basis.shellSet());
    const auto D_x = integrals[0];
    const auto D_y = integrals[1];
    const auto D_z = integrals[2];


    // Read in the reference values. ChronusQ has implemented position integrals, so we apply the required phase factor manually.
    const GQCP::MatrixX<double> D_x_ref_real = -GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_position_x_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> D_x_ref_complex = -GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_position_x_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> D_x_ref = D_x_ref_real + std::complex<double>(0, 1) * D_x_ref_complex;

    const GQCP::MatrixX<double> D_y_ref_real = -GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_position_y_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> D_y_ref_complex = -GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_position_y_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> D_y_ref = D_y_ref_real + std::complex<double>(0, 1) * D_y_ref_complex;

    const GQCP::MatrixX<double> D_z_ref_real = -GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_position_z_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> D_z_ref_complex = -GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_position_z_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> D_z_ref = D_z_ref_real + std::complex<double>(0, 1) * D_z_ref_complex;

    BOOST_CHECK(D_x.isApprox(D_x_ref, 1.0e-12));
    BOOST_CHECK(D_y.isApprox(D_y_ref, 1.0e-12));
    BOOST_CHECK(D_z.isApprox(D_z_ref, 1.0e-12));
}


/**
 *  Check if the London electronic dipole integrals are implemented correctly, for a magnetic field of B=(1,1,1).
 * 
 *  The references values are by generated through ChronusQ, which has implemented position integrals, i.e. we expect our results to differ with a factor (-1).
 */
BOOST_AUTO_TEST_CASE(London_electronic_dipole_111) {

    // Set up a scalar basis with London GTO shells.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");

    const GQCP::HomogeneousMagneticField B {{1.0, 1.0, 1.0}};  // Gauge origin at the origin.
    const GQCP::ScalarBasis<GQCP::LondonGTOShell> scalar_basis {molecule, "6-31G", B};
    const auto nbf = scalar_basis.numberOfBasisFunctions();

    // Calculate the overlap integrals through our own engines.
    auto engine = GQCP::IntegralEngine::InHouse<GQCP::LondonGTOShell>(GQCP::ElectronicDipoleOperator());  // Reference point at the origin.
    const auto integrals = GQCP::IntegralCalculator::calculate(engine, scalar_basis.shellSet(), scalar_basis.shellSet());
    const auto D_x = integrals[0];
    const auto D_y = integrals[1];
    const auto D_z = integrals[2];


    // Read in the reference values. ChronusQ has implemented position integrals, so we apply the required phase factor manually.
    const GQCP::MatrixX<double> D_x_ref_real = -GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_position_x_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> D_x_ref_complex = -GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_position_x_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> D_x_ref = D_x_ref_real + std::complex<double>(0, 1) * D_x_ref_complex;

    const GQCP::MatrixX<double> D_y_ref_real = -GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_position_y_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> D_y_ref_complex = -GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_position_y_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> D_y_ref = D_y_ref_real + std::complex<double>(0, 1) * D_y_ref_complex;

    const GQCP::MatrixX<double> D_z_ref_real = -GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_position_z_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> D_z_ref_complex = -GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_position_z_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> D_z_ref = D_z_ref_real + std::complex<double>(0, 1) * D_z_ref_complex;

    BOOST_CHECK(D_x.isApprox(D_x_ref, 1.0e-12));
    BOOST_CHECK(D_y.isApprox(D_y_ref, 1.0e-12));
    BOOST_CHECK(D_z.isApprox(D_z_ref, 1.0e-12));
}


/**
 *  Check if the London angular momentum integrals are implemented correctly, for a magnetic field of B=(0,0,1).
 * 
 *  The references values are by generated through ChronusQ, which has implemented angular momentum integrals without the prefactor (-i).
 */
BOOST_AUTO_TEST_CASE(London_angular_momentum_001) {

    using namespace GQCP::literals;

    // Set up a scalar basis with London GTO shells.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");

    const GQCP::HomogeneousMagneticField B {{0.0, 0.0, 1.0}};  // Gauge origin at the origin.
    const GQCP::ScalarBasis<GQCP::LondonGTOShell> scalar_basis {molecule, "6-31G", B};
    const auto nbf = scalar_basis.numberOfBasisFunctions();

    // Calculate the overlap integrals through our own engines.
    auto engine = GQCP::IntegralEngine::InHouse<GQCP::LondonGTOShell>(GQCP::AngularMomentumOperator());  // Reference point at the origin.
    const auto integrals = GQCP::IntegralCalculator::calculate(engine, scalar_basis.shellSet(), scalar_basis.shellSet());
    const auto L_x = integrals[0];
    const auto L_y = integrals[1];
    const auto L_z = integrals[2];


    // Read in the reference values.
    const GQCP::MatrixX<double> L_x_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_L_x_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> L_x_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_L_x_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> L_x_ref = -1.0_ii * (L_x_ref_real + std::complex<double>(0, 1) * L_x_ref_complex);

    const GQCP::MatrixX<double> L_y_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_L_y_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> L_y_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_L_y_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> L_y_ref = -1.0_ii * (L_y_ref_real + std::complex<double>(0, 1) * L_y_ref_complex);

    const GQCP::MatrixX<double> L_z_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_L_z_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> L_z_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_L_z_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> L_z_ref = -1.0_ii * (L_z_ref_real + std::complex<double>(0, 1) * L_z_ref_complex);

    BOOST_CHECK(L_x.isApprox(L_x_ref, 1.0e-12));
    BOOST_CHECK(L_y.isApprox(L_y_ref, 1.0e-12));
    BOOST_CHECK(L_z.isApprox(L_z_ref, 1.0e-12));
}


/**
 *  Check if the London angular momentum integrals are implemented correctly, for a magnetic field of B=(1,1,1).
 * 
 *  The references values are by generated through ChronusQ, which has implemented angular momentum integrals without the prefactor (-i).
 */
BOOST_AUTO_TEST_CASE(London_angular_momentum_111) {

    using namespace GQCP::literals;


    // Set up a scalar basis with London GTO shells.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");

    const GQCP::HomogeneousMagneticField B {{1.0, 1.0, 1.0}};  // Gauge origin at the origin.
    const GQCP::ScalarBasis<GQCP::LondonGTOShell> scalar_basis {molecule, "6-31G", B};
    const auto nbf = scalar_basis.numberOfBasisFunctions();

    // Calculate the overlap integrals through our own engines.
    auto engine = GQCP::IntegralEngine::InHouse<GQCP::LondonGTOShell>(GQCP::AngularMomentumOperator());  // Reference point at the origin.
    const auto integrals = GQCP::IntegralCalculator::calculate(engine, scalar_basis.shellSet(), scalar_basis.shellSet());
    const auto L_x = integrals[0];
    const auto L_y = integrals[1];
    const auto L_z = integrals[2];


    // Read in the reference values.
    const GQCP::MatrixX<double> L_x_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_L_x_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> L_x_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_L_x_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> L_x_ref = -1.0_ii * (L_x_ref_real + std::complex<double>(0, 1) * L_x_ref_complex);

    const GQCP::MatrixX<double> L_y_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_L_y_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> L_y_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_L_y_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> L_y_ref = -1.0_ii * (L_y_ref_real + std::complex<double>(0, 1) * L_y_ref_complex);

    const GQCP::MatrixX<double> L_z_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_L_z_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> L_z_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_L_z_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> L_z_ref = -1.0_ii * (L_z_ref_real + std::complex<double>(0, 1) * L_z_ref_complex);

    BOOST_CHECK(L_x.isApprox(L_x_ref, 1.0e-12));
    BOOST_CHECK(L_y.isApprox(L_y_ref, 1.0e-12));
    BOOST_CHECK(L_z.isApprox(L_z_ref, 1.0e-12));
}


/**
 *  Check if the London electronic quadrupole integrals are implemented correctly, for a magnetic field of B=(0,0,1).
 * 
 *  The references values are by generated through ChronusQ.
 */
BOOST_AUTO_TEST_CASE(London_quadrupole_momentum_001) {

    // Set up a scalar basis with London GTO shells.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");

    const GQCP::HomogeneousMagneticField B {{0.0, 0.0, 1.0}};  // Gauge origin at the origin.
    const GQCP::ScalarBasis<GQCP::LondonGTOShell> scalar_basis {molecule, "6-31G", B};
    const auto nbf = scalar_basis.numberOfBasisFunctions();

    // Calculate the overlap integrals through our own engines.
    auto engine = GQCP::IntegralEngine::InHouse<GQCP::LondonGTOShell>(GQCP::ElectronicQuadrupoleOperator());  // Reference point at the origin.
    const auto integrals = GQCP::IntegralCalculator::calculate(engine, scalar_basis.shellSet(), scalar_basis.shellSet());
    const auto Q_xx = integrals[0];
    const auto Q_xy = integrals[1];
    const auto Q_xz = integrals[2];
    const auto Q_yy = integrals[4];
    const auto Q_yz = integrals[5];
    const auto Q_zz = integrals[8];


    // Read in the reference values.
    const GQCP::MatrixX<double> Q_xx_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_quadrupole_xx_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> Q_xx_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_quadrupole_xx_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> Q_xx_ref = Q_xx_ref_real + std::complex<double>(0, 1) * Q_xx_ref_complex;

    const GQCP::MatrixX<double> Q_xy_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_quadrupole_xy_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> Q_xy_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_quadrupole_xy_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> Q_xy_ref = Q_xy_ref_real + std::complex<double>(0, 1) * Q_xy_ref_complex;

    const GQCP::MatrixX<double> Q_xz_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_quadrupole_xz_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> Q_xz_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_quadrupole_xz_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> Q_xz_ref = Q_xz_ref_real + std::complex<double>(0, 1) * Q_xz_ref_complex;

    const GQCP::MatrixX<double> Q_yy_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_quadrupole_yy_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> Q_yy_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_quadrupole_yy_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> Q_yy_ref = Q_yy_ref_real + std::complex<double>(0, 1) * Q_yy_ref_complex;

    const GQCP::MatrixX<double> Q_yz_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_quadrupole_yz_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> Q_yz_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_quadrupole_yz_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> Q_yz_ref = Q_yz_ref_real + std::complex<double>(0, 1) * Q_yz_ref_complex;

    const GQCP::MatrixX<double> Q_zz_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_quadrupole_zz_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> Q_zz_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_quadrupole_zz_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> Q_zz_ref = Q_zz_ref_real + std::complex<double>(0, 1) * Q_zz_ref_complex;

    BOOST_CHECK(Q_xx.isApprox(Q_xx_ref, 1.0e-12));
    BOOST_CHECK(Q_xy.isApprox(Q_xy_ref, 1.0e-12));
    BOOST_CHECK(Q_xz.isApprox(Q_xz_ref, 1.0e-12));
    BOOST_CHECK(Q_yy.isApprox(Q_yy_ref, 1.0e-12));
    BOOST_CHECK(Q_yz.isApprox(Q_yz_ref, 1.0e-12));
    BOOST_CHECK(Q_zz.isApprox(Q_zz_ref, 1.0e-12));
}


/**
 *  Check if the London electronic quadrupole integrals are implemented correctly, for a magnetic field of B=(1,1,1).
 * 
 *  The references values are by generated through ChronusQ.
 */
BOOST_AUTO_TEST_CASE(London_quadrupole_momentum_111) {

    // Set up a scalar basis with London GTO shells.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");

    const GQCP::HomogeneousMagneticField B {{1.0, 1.0, 1.0}};  // Gauge origin at the origin.
    const GQCP::ScalarBasis<GQCP::LondonGTOShell> scalar_basis {molecule, "6-31G", B};
    const auto nbf = scalar_basis.numberOfBasisFunctions();

    // Calculate the overlap integrals through our own engines.
    auto engine = GQCP::IntegralEngine::InHouse<GQCP::LondonGTOShell>(GQCP::ElectronicQuadrupoleOperator());  // Reference point at the origin.
    const auto integrals = GQCP::IntegralCalculator::calculate(engine, scalar_basis.shellSet(), scalar_basis.shellSet());
    const auto Q_xx = integrals[0];
    const auto Q_xy = integrals[1];
    const auto Q_xz = integrals[2];
    const auto Q_yy = integrals[4];
    const auto Q_yz = integrals[5];
    const auto Q_zz = integrals[8];


    // Read in the reference values.
    const GQCP::MatrixX<double> Q_xx_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_quadrupole_xx_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> Q_xx_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_quadrupole_xx_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> Q_xx_ref = Q_xx_ref_real + std::complex<double>(0, 1) * Q_xx_ref_complex;

    const GQCP::MatrixX<double> Q_xy_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_quadrupole_xy_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> Q_xy_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_quadrupole_xy_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> Q_xy_ref = Q_xy_ref_real + std::complex<double>(0, 1) * Q_xy_ref_complex;

    const GQCP::MatrixX<double> Q_xz_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_quadrupole_xz_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> Q_xz_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_quadrupole_xz_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> Q_xz_ref = Q_xz_ref_real + std::complex<double>(0, 1) * Q_xz_ref_complex;

    const GQCP::MatrixX<double> Q_yy_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_quadrupole_yy_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> Q_yy_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_quadrupole_yy_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> Q_yy_ref = Q_yy_ref_real + std::complex<double>(0, 1) * Q_yy_ref_complex;

    const GQCP::MatrixX<double> Q_yz_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_quadrupole_yz_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> Q_yz_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_quadrupole_yz_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> Q_yz_ref = Q_yz_ref_real + std::complex<double>(0, 1) * Q_yz_ref_complex;

    const GQCP::MatrixX<double> Q_zz_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_quadrupole_zz_chronusq_real.data", nbf, nbf);
    const GQCP::MatrixX<double> Q_zz_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_quadrupole_zz_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> Q_zz_ref = Q_zz_ref_real + std::complex<double>(0, 1) * Q_zz_ref_complex;

    BOOST_CHECK(Q_xx.isApprox(Q_xx_ref, 1.0e-12));
    BOOST_CHECK(Q_xy.isApprox(Q_xy_ref, 1.0e-12));
    BOOST_CHECK(Q_xz.isApprox(Q_xz_ref, 1.0e-12));
    BOOST_CHECK(Q_yy.isApprox(Q_yy_ref, 1.0e-12));
    BOOST_CHECK(Q_yz.isApprox(Q_yz_ref, 1.0e-12));
    BOOST_CHECK(Q_zz.isApprox(Q_zz_ref, 1.0e-12));
}


/**
 *  Check if the London nuclear attraction integrals are implemented correctly, for a magnetic field of B=(0,0,1).
 * 
 *  The references values are by generated through ChronusQ.
 */
BOOST_AUTO_TEST_CASE(London_nuclear_001) {

    // Set up a scalar basis with London GTO shells.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");

    const GQCP::HomogeneousMagneticField B {{0.0, 0.0, 1.0}};  // Gauge origin at the origin.
    const GQCP::ScalarBasis<GQCP::LondonGTOShell> scalar_basis {molecule, "6-31G", B};
    const auto nbf = scalar_basis.numberOfBasisFunctions();

    // Calculate the nuclear attraction integrals through our own engines.
    const auto op = GQCP::NuclearAttractionOperator(molecule.nuclearFramework());
    auto engine = GQCP::IntegralEngine::InHouse<GQCP::LondonGTOShell>(op);
    const auto V = GQCP::IntegralCalculator::calculate(engine, scalar_basis.shellSet(), scalar_basis.shellSet())[0];


    // Read in the reference values.
    const auto V_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_nuclear_chronusq_real.data", nbf, nbf);
    const auto V_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_001_nuclear_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> V_ref = V_ref_real + std::complex<double>(0, 1) * V_ref_complex;

    BOOST_CHECK(V.isApprox(V_ref, 1.0e-12));
}


/**
 *  Check if the London nuclear attraction integrals are implemented correctly, for a magnetic field of B=(1,1,1).
 * 
 *  The references values are by generated through ChronusQ.
 */
BOOST_AUTO_TEST_CASE(London_nuclear_111) {

    // Set up a scalar basis with London GTO shells.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");

    const GQCP::HomogeneousMagneticField B {{1.0, 1.0, 1.0}};  // Gauge origin at the origin.
    const GQCP::ScalarBasis<GQCP::LondonGTOShell> scalar_basis {molecule, "6-31G", B};
    const auto nbf = scalar_basis.numberOfBasisFunctions();

    // Calculate the nuclear attraction integrals through our own engines.
    const auto op = GQCP::NuclearAttractionOperator(molecule.nuclearFramework());
    auto engine = GQCP::IntegralEngine::InHouse<GQCP::LondonGTOShell>(op);
    const auto V = GQCP::IntegralCalculator::calculate(engine, scalar_basis.shellSet(), scalar_basis.shellSet())[0];


    // Read in the reference values.
    const auto V_ref_real = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_nuclear_chronusq_real.data", nbf, nbf);
    const auto V_ref_complex = GQCP::MatrixX<double>::FromFile("data/h2o_6-31g_111_nuclear_chronusq_complex.data", nbf, nbf);
    GQCP::MatrixX<std::complex<double>> V_ref = V_ref_real + std::complex<double>(0, 1) * V_ref_complex;

    BOOST_CHECK(V.isApprox(V_ref, 1.0e-12));
}


/*
 *  Check if the London Coulomb repulsion integrals are implemented correctly, for a magnetic field of B=(0,0,1).
 * 
 *  The references values are by generated through ChronusQ.
 */
BOOST_AUTO_TEST_CASE(London_coulomb_001) {

    // Set up a scalar basis with London GTO shells.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");

    const GQCP::HomogeneousMagneticField B {{0.0, 0.0, 1.0}};  // Gauge origin at the origin.
    const GQCP::ScalarBasis<GQCP::LondonGTOShell> scalar_basis {molecule, "6-31G", B};
    const auto nbf = scalar_basis.numberOfBasisFunctions();


    // Calculate the Coulomb repulsion integrals through our own engines.
    const auto op = GQCP::CoulombRepulsionOperator();
    auto engine = GQCP::IntegralEngine::InHouse<GQCP::LondonGTOShell>(op);
    const auto g = GQCP::IntegralCalculator::calculate(engine, scalar_basis.shellSet(), scalar_basis.shellSet())[0];

    // Read in the reference values.
    const auto g_ref_real = GQCP::SquareRankFourTensor<double>::FromFile("data/h2o_6-31g_001_eri_chronusq_real.data", nbf);
    const auto g_ref_complex = GQCP::SquareRankFourTensor<double>::FromFile("data/h2o_6-31g_001_eri_chronusq_complex.data", nbf);

    BOOST_CHECK(GQCP::SquareRankFourTensor<double>(g.real()).isApprox(g_ref_real, 1.0e-06));
    BOOST_CHECK(GQCP::SquareRankFourTensor<double>(g.imag()).isApprox(g_ref_complex, 1.0e-06));
}


/*
 *  Check if the London Coulomb repulsion integrals are implemented correctly, for a magnetic field of B=(0,0,1).
 * 
 *  The references values are by generated through ChronusQ.
 */
BOOST_AUTO_TEST_CASE(London_coulomb_111) {

    // Set up a scalar basis with London GTO shells.
    const auto molecule = GQCP::Molecule::ReadXYZ("data/h2o.xyz");

    const GQCP::HomogeneousMagneticField B {{1.0, 1.0, 1.0}};  // Gauge origin at the origin.
    const GQCP::ScalarBasis<GQCP::LondonGTOShell> scalar_basis {molecule, "6-31G", B};
    const auto nbf = scalar_basis.numberOfBasisFunctions();


    // Calculate the Coulomb repulsion integrals through our own engines.
    const auto op = GQCP::CoulombRepulsionOperator();
    auto engine = GQCP::IntegralEngine::InHouse<GQCP::LondonGTOShell>(op);
    const auto g = GQCP::IntegralCalculator::calculate(engine, scalar_basis.shellSet(), scalar_basis.shellSet())[0];

    // Read in the reference values.
    const auto g_ref_real = GQCP::SquareRankFourTensor<double>::FromFile("data/h2o_6-31g_111_eri_chronusq_real.data", nbf);
    const auto g_ref_complex = GQCP::SquareRankFourTensor<double>::FromFile("data/h2o_6-31g_111_eri_chronusq_complex.data", nbf);

    BOOST_CHECK(GQCP::SquareRankFourTensor<double>(g.real()).isApprox(g_ref_real, 1.0e-06));
    BOOST_CHECK(GQCP::SquareRankFourTensor<double>(g.imag()).isApprox(g_ref_complex, 1.0e-06));
}
