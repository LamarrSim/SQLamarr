#include "SQLamarr/BlockLib/LbParticleId.h"

namespace SQLamarr 
{ 
  namespace BlockLib
  {
    namespace LbParticleId
    {
      //========================================================================
      // make
      //========================================================================
      GenerativePlugin make(
          SQLite3DB& db,
          const std::string& library,
          const std::string& function_name,
          const std::string& output_table,
          const std::string& particle_table,
          const std::string& track_table,
          const int abspid
          )
      {
        const char q[] = R"(
              SELECT
                p.mcparticle_id AS mcparticle_id,
                norm2(p.px, p.py, p.pz) AS p,
                pseudorapidity(p.px, p.py, p.pz) AS eta,
                random_normal() * 10 + 100 as nTracks,
                propagation_charge(p.pid) AS track_charge,
                0 AS isMuon
              FROM %s AS p
              INNER JOIN %s AS recguess 
                ON p.mcparticle_id = recguess.mcparticle_id
              WHERE 
                  recguess.track_type == 3
                AND 
                  abs(p.pid) == %d;
              )";

        char buffer[1024];
        sprintf(buffer, q, particle_table.c_str(),  track_table.c_str(), abspid);

        return SQLamarr::GenerativePlugin (db,
              library,
              function_name,
              buffer,
              output_table,
              get_column_names(false, true), 
              64, 
              get_column_names(true, false)
            );
      }


      //========================================================================
      // get_column_names
      //========================================================================
      std::vector<std::string> get_column_names (
          bool include_indices, 
          bool include_outputs
          )
      {
        std::vector<std::string> ret;

        std::vector<std::string> indices { "mc_particleid" };
        std::vector<std::string> outputs {
          // Rich
          "RichDLLe", "RichDLLmu", "RichDLLK", "RichDLLp",
          // Muon
          "MuonMuLL", "MuonBkgLL", 
          // Global PID
          "PIDe", "PIDK", "PIDp", "ProbNNe", "ProbNNpi", "ProbNNk", "ProbNNp",
          // Global PID
          "PIDmu", "ProbNNmu"
        };

        if (include_indices)
          ret.insert(ret.end(), indices.begin(), indices.end());

        if (include_outputs)
          ret.insert(ret.end(), outputs.begin(), outputs.end());

        return ret;
      }
    }
  }
}
