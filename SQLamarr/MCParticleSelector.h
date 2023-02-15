// (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration.
//
// This software is distributed under the terms of the GNU General Public
// Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
//
// In applying this licence, CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


#pragma once
#include <vector>

#include "SQLamarr/db_functions.h"
#include "SQLamarr/BaseSqlInterface.h"
#include "SQLamarr/preprocessor_symbols.h"
#include "SQLamarr/Transformer.h"

namespace SQLamarr
{
  /** Converts `GenParticle`s to `MCParticle`s

    Most `GenParticle`s defined in HepMC graphs by most generators
    are completely irrelevant to LHCb as they describe the interactions
    at parton-level, which is impossible to access experimentally.

    Before describing the reconstruction procedure, it is worth cleaning 
    the graph retaining those particles and relations describing 
    experimentally-accessible features of the interaction.

    The resulting graph is actually a tree, as experimentally we can 
    only describe decays: each vertex has a unique *input* particle and 
    at least two *output* particles.

    The MCParticleSelector algorithm describe the conversion from the HepMC
    graph to the Monte Carlo decay tree.

    The condition defining whether to retain a particle or not is defined 
    by white lists of status and PDG ID values identifying `GenParticle`s to be 
    promoted to `MCParticle`s.

    Promoted particles are stored in the `MCParticles` table.

    More advanced or branched selection criteria can be defined by 
    inheriting from this class and overriding the `keep` method.
  */
  class MCParticleSelector: public BaseSqlInterface, Transformer
  {
    public:
      /// Initializes and configures the algorithm
      MCParticleSelector (
          /// Reference to the database (not owned)
          SQLite3DB& db, 

          /// List of the status values retained as part of the decay tree
          const std::vector<uint64_t> retained_status_values = {
            LAMARR_LHCB_STABLE_IN_PRODGEN,
            LAMARR_LHCB_DECAYED_BY_DECAYGEN,
            LAMARR_LHCB_DECAYED_BY_DECAYGEN_AND_PRODUCED_BY_PRODGEN,
            LAMARR_LHCB_SIGNAL_IN_LAB_FRAME,
            LAMARR_LHCB_STABLE_IN_DECAYGEN
          }, 

          /// List of the pid values retained as abspid values
          const std::vector<uint64_t> retained_abspid_values = {
            /* Standard model particles */
            6, 22, 23, 24, 25, 32, 33, 34, 35, 36, 37, 102,
            /* Strange Mesons: */
            130, 310, 311, 321, 
            /* Charm Mesons: */
            411, 421, 413, 423, 415, 425, 431, 435, 
            /* Beauty Mesons: */
            511, 521, 513, 523, 515, 525, 531, 535, 541, 545, 
            /* Charmonium states: */
            441, 10441, 100441, 443, 10443, 20443, 100443, 30443, 9000443, 9010443,
            9020443, 445, 10445, 
            /* Bottomonium states: */
            551, 10551, 100551, 110551, 200551, 210551, 553, 10553, 20553, 30553,
            100553, 110553, 120553, 130553, 200553, 210553, 220553, 300553, 
            9000553, 9010553, 555, 10555, 20555, 100555, 110555, 120555, 200555,
            557, 100557, 
            /* Light baryons: */
            2212, 2212, 
            /* Strange baryons: */
            3122, 3222, 3212, 3224, 3214, 3114, 3322, 3312, 3324, 3314, 3334, 
            /* Charm baryons: */
            4122, 4222, 4212, 4112, 4224, 4214, 4114, 4232, 4132, 4322, 4312, 4324,
            4314, 4332, 4334, 4412, 4422, 4414, 4424, 4432, 4434, 4444, 
            /* Beauty baryons: */
            5122, 5112, 5212, 5222, 5114, 5214, 5224, 5132, 5232, 5312, 5322,
            5314, 5324, 5332, 5334, 5142, 5242, 5412, 5422, 5414, 5424, 5342, 5432,
            5442, 5444, 5512, 5522, 5514, 5524, 5532, 5534, 5542, 5544, 5554
          }
      );

      /// Execute the algorithm on the database (a batch of data)
      void execute () override;

    protected:
      /// Recursive function processing a particles and its daughters (if any).
      /// If the particle is discarded, but any of its dauthers are kept, their 
      /// production vertex is set to their parent's production vertex to
      /// preserve a tree structure.
      bool process_particle (
          int genparticle_id,   ///< UID of the input particle
          int prod_vtx          ///< UID of the production vertex of the input
                                ///  particle. 
          );

      /// Definition of the criterion to promote a GenParticle to MCParticle
      bool keep (
          int status,   ///< HepMC status of the GenParticle 
          int abspid    ///< Absolute PID of the GenParticle
          ) const;

      /// Returns the UID of the end vertex of a particle, creating it if
      /// missing.
      uint64_t get_or_create_end_vertex (
          int genparticle_id  ///< UID of the input GenParticle
          );


    private:
      const std::vector<uint64_t> m_retained_status_values;
      const std::vector<uint64_t> m_retained_abspid_values;
  };
}
