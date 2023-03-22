#pragma once

#include "SQLamarr/db_functions.h"
#include "SQLamarr/GenerativePlugin.h"

#include <memory>


namespace SQLamarr 
{ 
  namespace BlockLib
  {
    /** Specialize GenerativePlugin to the LHCb Particle ID Pipeline.
     * 
     * The model of the LHCb Particle Identification reconstructed quantities
     * is defined, for example in https://github.com/mbarbetti/lb-pidsim-train.
     *
     * They take as in input kinematic features of each particle, 
     * event occupancy encoded by the number of tracks in the event,
     * the charge of the particle and the isMuon flag.
     *
     * A different model is defined for each particle (pi, K, p, and mu).
    */
    namespace LbParticleId
    {
      /// Initialize a `GenerativePlugin` specialized for PID pipelines
      std::unique_ptr<GenerativePlugin> make (
          SQLite3DB& db,
            ///< Reference to the database 
          const std::string& library,
            ///< Path to the library
          const std::string& function_name,
            ///< Name of the linking symbol
          const std::string& output_table,
            ///< Name of the output table (overwrite on existing)
          const std::string& particle_table,
            /**< Name of the MCParticle table.
             * Table must include columns `mcparticle_id`,`px`, `py`, `pz`.
             */
          const std::string& track_table,
            /**< Name of table with track type information.
             * Table must include column `track_type`.
             */
          const int abspid
            ///< Absolute particle ID to process with this pipeline
          );

      /// Provide a vector of the names of the columns of the output table.
      std::vector<std::string> get_column_names (
            bool include_indices = true,
              ///< If false, reference indices for future joins are not listed
            bool include_outputs = true
              ///< If false, output features from plugin are not listed
          );
    }
  }
}
