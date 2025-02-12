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

#include "Basis/Integrals/IntegralEngine.hpp"


namespace GQCP {


/*
 *  LIBINT
 */

/**
 *  @param op               the Coulomb repulsion operator
 *  @param max_nprim        the maximum number of primitives per contracted Gaussian shell
 *  @param max_l            the maximum angular momentum of Gaussian shell
 * 
 *  @return a two-electron integral engine that can calculate integrals over the Coulomb repulsion operator using the Libint integral library backend
 */
auto IntegralEngine::Libint(const CoulombRepulsionOperator& op, const size_t max_nprim, const size_t max_l) -> LibintTwoElectronIntegralEngine<CoulombRepulsionOperator::NumberOfComponents> {

    return LibintTwoElectronIntegralEngine<CoulombRepulsionOperator::NumberOfComponents>(op, max_nprim, max_l);
}


/**
 *  @param op               the electronic electric dipole operator
 *  @param max_nprim        the maximum number of primitives per contracted Gaussian shell
 *  @param max_l            the maximum angular momentum of Gaussian shell
 * 
 *  @return a one-electron integral engine that can calculate integrals over the electronic electric dipole operator using the Libint integral library backend
 */
auto IntegralEngine::Libint(const ElectronicDipoleOperator& op, const size_t max_nprim, const size_t max_l) -> LibintOneElectronIntegralEngine<ElectronicDipoleOperator::NumberOfComponents> {

    return LibintOneElectronIntegralEngine<ElectronicDipoleOperator::NumberOfComponents>(op, max_nprim, max_l);
}


/**
 *  @param op               the kinetic operator
 *  @param max_nprim        the maximum number of primitives per contracted Gaussian shell
 *  @param max_l            the maximum angular momentum of Gaussian shell
 * 
 *  @return a one-electron integral engine that can calculate integrals over the kinetic operator using the Libint integral library backend
 */
auto IntegralEngine::Libint(const KineticOperator& op, const size_t max_nprim, const size_t max_l) -> LibintOneElectronIntegralEngine<KineticOperator::NumberOfComponents> {

    return LibintOneElectronIntegralEngine<KineticOperator::NumberOfComponents>(op, max_nprim, max_l);
}


/**
 *  @param op               the nuclear attraction operator
 *  @param max_nprim        the maximum number of primitives per contracted Gaussian shell
 *  @param max_l            the maximum angular momentum of Gaussian shell
 * 
 *  @return a one-electron integral engine that can calculate integrals over the nuclear attraction operator using the Libint integral library backend
 */
auto IntegralEngine::Libint(const NuclearAttractionOperator& op, const size_t max_nprim, const size_t max_l) -> LibintOneElectronIntegralEngine<NuclearAttractionOperator::NumberOfComponents> {

    return LibintOneElectronIntegralEngine<NuclearAttractionOperator::NumberOfComponents>(op, max_nprim, max_l);
}


/**
 *  @param op               the overlap operator
 *  @param max_nprim        the maximum number of primitives per contracted Gaussian shell
 *  @param max_l            the maximum angular momentum of Gaussian shell
 * 
 *  @return a one-electron integral engine that can calculate integrals over the overlap operator using the Libint integral library backend
 */
auto IntegralEngine::Libint(const OverlapOperator& op, const size_t max_nprim, const size_t max_l) -> LibintOneElectronIntegralEngine<OverlapOperator::NumberOfComponents> {

    return LibintOneElectronIntegralEngine<OverlapOperator::NumberOfComponents>(op, max_nprim, max_l);
}


/*
 *  LIBCINT
 */

/**
 *  @param op               the Coulomb repulsion operator
 *  @param shell_set        the ShellSet whose information should be converted to a RawContainer, which will serve as some kind of 'global' data for the libcint engine to use in all its calculate() calls
 * 
 *  @return a two-electron integral engine that can calculate integrals over the Coulomb repulsion operator using the Libcint integral library backend
 */
auto IntegralEngine::Libcint(const CoulombRepulsionOperator& op, const ShellSet<GTOShell>& shell_set) -> LibcintTwoElectronIntegralEngine<GTOShell, CoulombRepulsionOperator::NumberOfComponents, double> {

    return LibcintTwoElectronIntegralEngine<GTOShell, CoulombRepulsionOperator::NumberOfComponents, double>(op, shell_set);
}


/**
 *  @param op               the electron electronic dipole operator
 * 
 *  @return a one-electron integral engine that can calculate integrals over the electronic dipole operator using the Libcint integral library backend
 */
auto IntegralEngine::Libcint(const ElectronicDipoleOperator& op, const ShellSet<GTOShell>& shell_set) -> LibcintOneElectronIntegralEngine<GTOShell, ElectronicDipoleOperator::NumberOfComponents, double> {

    return LibcintOneElectronIntegralEngine<GTOShell, ElectronicDipoleOperator::NumberOfComponents, double>(op, shell_set);
}


/**
 *  @param op               the kinetic operator
 *  @param shell_set        the ShellSet whose information should be converted to a RawContainer, which will serve as some kind of 'global' data for the libcint engine to use in all its calculate() calls
 * 
 *  @return a one-electron integral engine that can calculate integrals over the kinetic operator using the Libcint integral library backend
 */
auto IntegralEngine::Libcint(const KineticOperator& op, const ShellSet<GTOShell>& shell_set) -> LibcintOneElectronIntegralEngine<GTOShell, KineticOperator::NumberOfComponents, double> {

    return LibcintOneElectronIntegralEngine<GTOShell, KineticOperator::NumberOfComponents, double>(op, shell_set);
}


/**
 *  @param op               the nuclear attraction operator
 *  @param shell_set        the ShellSet whose information should be converted to a RawContainer, which will serve as some kind of 'global' data for the libcint engine to use in all its calculate() calls
 * 
 *  @return a one-electron integral engine that can calculate integrals over the nuclear attraction operator using the Libcint integral library backend
 */
auto IntegralEngine::Libcint(const NuclearAttractionOperator& op, const ShellSet<GTOShell>& shell_set) -> LibcintOneElectronIntegralEngine<GTOShell, NuclearAttractionOperator::NumberOfComponents, double> {

    return LibcintOneElectronIntegralEngine<GTOShell, NuclearAttractionOperator::NumberOfComponents, double>(op, shell_set);
}


/**
 *  @param op               the overlap operator
 *  @param shell_set        the ShellSet whose information should be converted to a RawContainer, which will serve as some kind of 'global' data for the libcint engine to use in all its calculate() calls
 * 
 *  @return a one-electron integral engine that can calculate integrals over the overlap operator using the Libcint integral library backend
 */
auto IntegralEngine::Libcint(const OverlapOperator& op, const ShellSet<GTOShell>& shell_set) -> LibcintOneElectronIntegralEngine<GTOShell, OverlapOperator::NumberOfComponents, double> {

    return LibcintOneElectronIntegralEngine<GTOShell, OverlapOperator::NumberOfComponents, double>(op, shell_set);
}


}  // namespace GQCP
