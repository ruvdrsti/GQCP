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


#include "Partition/DiscreteDomainPartition.hpp"


namespace GQCP {


/*
 *  MARK: Constructors
 */

/**
 *  Create a discrete domain partition from a vector of discrete domains.
 *
 *  @param domains          The vector of discrete domains.
 */
DiscreteDomainPartition::DiscreteDomainPartition(const std::vector<DiscreteDomain>& domains) :
    DomainPartition<DiscreteDomain>(domains) {
    size_t domain_occupations = 0;

    for (size_t i = 0; i < domains.size() - 1; i++) {
        for (size_t j = i + 1; j < domains.size(); j++) {
            if (domains[i].dimension() != domains[j].dimension()) {
                throw std::invalid_argument("DiscreteDomainPartition::DiscreteDomainPartition(const std::vector<DiscreteDomain> domains): Found domains where the domain dimensions are not of equal size.");
            }
            if (domains[i].overlapWith(domains[j])) {
                throw std::invalid_argument("DiscreteDomainPartition::DiscreteDomainPartition(const std::vector<DiscreteDomain> domains): Found fuzzy domains, please provide domains that do not overlap with eachother.");
            }
        }
        domain_occupations += domains[i].numberOfElements();
    }
    domain_occupations += domains.back().numberOfElements();

    if (domain_occupations != domains[0].dimension()) {
        throw std::invalid_argument("DiscreteDomainPartition::DiscreteDomainPartition(const std::vector<DiscreteDomain> domains): Found non-complete domains, please provide a collection of domains where each index belongs to a domain once.");
    }
}

/**
 *  Create a discrete domain partition from a vector representation of discrete domains.
 *
 *  @param domain_partition_vector          The vector representation of discrete domains.
 */
DiscreteDomainPartition::DiscreteDomainPartition(const std::vector<size_t>& domain_partition_vector) {
    const auto n_unique = std::set<size_t>(domain_partition_vector.begin(), domain_partition_vector.end()).size();

    std::vector<size_t> domains_unsigned_representations(n_unique, 0);
    for (size_t i = 0; i < domain_partition_vector.size(); i++) {
        auto d = domain_partition_vector[i];
        domains_unsigned_representations[d] |= (1 << i);
    }
    std::vector<DiscreteDomain> domains;
    for (size_t unsigned_representation : domains_unsigned_representations) {
        domains.push_back(DiscreteDomain(unsigned_representation, domain_partition_vector.size()));
    }

    this->partition = domains;
    DiscreteDomainPartition {this->partition};  // Check conditions for domains.
}

/**
 *  Create a discrete domain partition from a vector of unsigned representations.
 *
 *  @param domains          The vector of unsigned representations.
 *  @param M                The dimension of each domain.
 */
DiscreteDomainPartition::DiscreteDomainPartition(const std::vector<size_t>& unsigned_representations, size_t M) {
    std::vector<DiscreteDomain> domains;
    for (size_t u : unsigned_representations) {
        domains.push_back(DiscreteDomain(u, M));
    }

    this->partition = domains;
    DiscreteDomainPartition {this->partition};  // Check conditions for domains.
}

/*
 *  MARK: General info
 */

/**
 *  @return The discrete domain partition string representation.
 */
std::string DiscreteDomainPartition::asString() const {

    const auto& v = this->asVector();
    std::string s;

    for (size_t i = 0; i < v.size(); i++) {
        s += std::to_string(v[i]) + "-";
    }
    return s.substr(0, s.length() - 1);
}

/**
 *  @return The discrete domain partition vector representation.
 */
std::vector<size_t> DiscreteDomainPartition::asVector() const {
    std::vector<size_t> v;
    for (size_t i = 0; i < this->operator()(0).dimension(); i++) {
        int domain = -1;
        for (size_t d = 0; d < this->dimension(); d++) {
            if (this->operator()(d)(i)) {
                domain = d;
            }
        }

        if (domain == -1) {
            throw std::invalid_argument("DiscreteDomainPartition::asVector(): Found an index " + std::to_string(i) + "that does not belong to any domain.");
        } else {
            v.push_back(domain);
        }
    }

    return v;
}


/*
 * MARK: Overlap
 */

/**
 * Calculate the overlap between the discrete domains in this domain partition and a spin-unresolved ONV since both can be represented as a bitstring.
 *
 *  @param onv            The spin-unresolved ONV.
 *
 *  @return     The numbers of overlapping set bits after a bit-by-bit comparison between the discrete domains and the spin-unresolved ONV.
 */
SpinUnresolvedElectronPartition DiscreteDomainPartition::overlapWithONV(const SpinUnresolvedONV& onv) const {
    std::vector<size_t> overlaps;
    for (const auto& domain : this->partition) {
        overlaps.push_back(domain.overlapWithONV(onv));
    }
    return SpinUnresolvedElectronPartition(overlaps);
}


}  // namespace GQCP
