#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include "SQLamarr/HepMC2DataLoader.h"
#include "SQLamarr/PVFinder.h"
#include "SQLamarr/db_functions.h"
#include "SQLamarr/MCParticleSelector.h"
#include "SQLamarr/PVReconstruction.h"
#include "SQLamarr/Plugin.h"
#include "SQLamarr/GenerativePlugin.h"
#include "SQLamarr/GlobalPRNG.h"
#include "SQLamarr/TemporaryTable.h"
#include <memory>
#include <glob.h>

#include "sqlite3.h"
#include <stdio.h>

#include <filesystem>


// Helper function to list files in a dir
std::vector<std::string> globVector(const std::string& pattern);


int main(int argc, char* argv[])
{
  std::string path{"../temporary_data/HepMC2-ascii/DSt_Pi.hepmc2/evt*.mc2"};

  std::vector<std::string> file_paths = globVector(path); 
//  {
//    "/home/lucio/test-data/DSt_Pi.hepmc2/evt0.mc2", 
//    "/home/lucio/test-data/DSt_Pi.hepmc2/evt1.mc2", 
//    "/home/lucio/test-data/DSt_Pi.hepmc2/evt2.mc2", 
//    "/home/lucio/test-data/DSt_Pi.hepmc2/evt3.mc2", 
//    "/home/lucio/test-data/DSt_Pi.hepmc2/evt4.mc2", 
//    "/home/lucio/test-data/DSt_Pi.hepmc2/evt5.mc2" 
//  };
//
  // Create the in-memory database
  SQLamarr::SQLite3DB db = SQLamarr::make_database(argc > 1 ? argv[1] : ":memory:" );
  SQLamarr::GlobalPRNG::get_or_create(db.get(), 123);


  // Define a data loader to take data from HepMC2-Ascii format
  SQLamarr::HepMC2DataLoader loader (db);

  // Loads the data to the database
  size_t evtNumber = 0;
  size_t runNumber = 456;

  for (std::string& file_path: file_paths)
    if (evtNumber < 10) 
      loader.load(file_path, runNumber, evtNumber++);

  // Runs the PVFinder algorithm
  SQLamarr::PVFinder pvfinder(db);
  pvfinder.execute();

  SQLamarr::MCParticleSelector mcps(db);
  mcps.execute();

  SQLamarr::PVReconstruction pv_reco(db,
      SQLamarr::PVReconstruction::load_parametrization(
        "../temporary_data/PrimaryVertex/PrimaryVertexSmearing.db",
        "PVSmearing", "2016_pp_MagUp")
      );
  pv_reco.execute();

  SQLamarr::Plugin acceptance_model (db,
        "../temporary_data/models/lhcb.trk.2016MU.so",
        "acceptance", 
        R"(
        SELECT 
          mcparticle_id,
          ov.x AS mc_x, 
          ov.y AS mc_y, 
          ov.z AS mc_z,
          log10(norm2(p.px, p.py, p.pz)) AS mc_log10_p,
          p.px/p.pz AS mc_tx, 
          p.py/p.pz AS mc_ty,
          pseudorapidity(p.px, p.py, p.pz) AS mc_eta,
          azimuthal(p.px, p.py, p.pz) AS mc_phi,
          abs(p.pid) == 11 AS mc_is_e,
          abs(p.pid) == 13 AS mc_is_mu,
          (
            abs(p.pid) == 211 OR abs(p.pid) == 321 OR abs(p.pid) == 2212  
          ) AS mc_is_h,
          propagation_charge(p.pid) AS mc_charge
        FROM MCParticles AS p
        INNER JOIN MCVertices AS ov ON p.production_vertex = ov.mcvertex_id
        WHERE 
          p.pz > 1.
          AND
          propagation_charge(p.pid) <> 0.
        )",
        "tmp_acceptance_out", {"acceptance"}, {"mcparticle_id"}
      );

  acceptance_model.execute();


  SQLamarr::Plugin efficiency_model (db,
        "../temporary_data/models/lhcb.trk.2016MU.so",
        "efficiency", 
        R"(
        SELECT 
          mcparticle_id,
          ov.x AS mc_x, 
          ov.y AS mc_y, 
          ov.z AS mc_z,
          log10(norm2(p.px, p.py, p.pz)) AS mc_log10_p,
          p.px/p.pz AS mc_tx, 
          p.py/p.pz AS mc_ty,
          pseudorapidity(p.px, p.py, p.pz) AS mc_eta,
          azimuthal(p.px, p.py, p.pz) AS mc_phi,
          abs(p.pid) == 11 AS mc_is_e,
          abs(p.pid) == 13 AS mc_is_mu,
          (
            abs(p.pid) == 211 OR abs(p.pid) == 321 OR abs(p.pid) == 2212  
          ) AS mc_is_h,
          propagation_charge(p.pid) AS mc_charge
        FROM MCParticles AS p
        INNER JOIN MCVertices AS ov ON p.production_vertex = ov.mcvertex_id
        WHERE 
          p.pz > 1.
          AND
          propagation_charge(p.pid) <> 0.
        )",
        "tmp_efficiency_out", 
        {"not_recoed", "long", "upstream", "downstream"}, 
        {"mcparticle_id"}
      );

  efficiency_model.execute();

  SQLamarr::TemporaryTable assign_track_cat (db,
      "tmp_particles_recoed_as", {"mcparticle_id", "track_type"},
      R"( SELECT 
            eff.mcparticle_id,
            random_category(not_recoed, 0, 0, long, upstream) AS track_type
          FROM tmp_efficiency_out AS eff
          INNER JOIN tmp_acceptance_out AS acc
            ON eff.mcparticle_id = acc.mcparticle_id
          WHERE random_category(1 - acc.acceptance, acc.acceptance) == 1
      )",
      /*make_persistent*/ true
      );

  assign_track_cat.execute();

  SQLamarr::TemporaryTable propagate_to_ctb (db,
      "tmp_closest_to_beam", {"mcparticle_id", "x", "y", "z"},
      R"( WITH ctb AS (
            SELECT 
              mcparticle_id,
              z_closest_to_beam(ov.x, ov.y, ov.z, p.px/p.pz, p.py/p.pz) AS z,
              ov.x AS x0,
              ov.y AS y0,
              ov.z AS z0,
              p.px/p.pz AS tx,
              p.py/p.pz AS ty
            FROM MCParticles AS p
            INNER JOIN MCVertices AS ov
              ON p.production_vertex = ov.mcvertex_id
          )
          SELECT 
            mcparticle_id,
            x0 + (z - z0)*tx AS x,
            y0 + (z - z0)*ty AS y,
            z AS z
          FROM ctb;
      )",
    /*make_persistent*/ true
    );
  propagate_to_ctb.execute();
              

  SQLamarr::GenerativePlugin resolution_model (db,
        "../temporary_data/models/lhcb.trk.2016MU.so",
        "resolution", 
        R"(
        SELECT 
          p.mcparticle_id,
          ctb.x AS mc_x, 
          ctb.y AS mc_y, 
          ctb.z AS mc_z, 
          p.px/p.pz AS mc_tx, 
          p.py/p.pz AS mc_ty,
          log10(norm2(p.px, p.py, p.pz)) AS mc_log10_p,
          abs(p.pid) == 11 AS mc_is_e,
          abs(p.pid) == 13 AS mc_is_mu,
          (abs(p.pid) = 211 OR abs(p.pid) = 321 OR abs(p.pid) = 2212) AS is_h,
          (recguess.track_type == 3) AS is_long, 
          (recguess.track_type == 4) AS is_upstream, 
          (recguess.track_type == 5) AS is_downstream 
        FROM MCParticles AS p
        INNER JOIN MCVertices AS ov ON p.production_vertex = ov.mcvertex_id
        INNER JOIN tmp_particles_recoed_as AS recguess 
          ON p.mcparticle_id = recguess.mcparticle_id
        INNER JOIN tmp_closest_to_beam AS ctb 
          ON p.mcparticle_id = ctb.mcparticle_id
        WHERE 
          recguess.track_type BETWEEN 3 AND 5;
        )",
        "tmp_resolution_out", 
        {"dx", "dy", "dz", "dtx", "dty", "dp", 
          "chi2PerDoF", "nDoF_f", "ghostProb"}, 
        128,
        {"mcparticle_id"}
      );

  resolution_model.execute();


  SQLamarr::GenerativePlugin covariance_model (db,
        "../temporary_data/models/lhcb.trk.2016MU.so",
        "covariance", 
        R"(
        SELECT 
          p.mcparticle_id,
          ctb.x AS mc_x, 
          ctb.y AS mc_y, 
          ctb.z AS mc_z, 
          p.px/p.pz AS mc_tx, 
          p.py/p.pz AS mc_ty,
          log10(norm2(p.px, p.py, p.pz)) AS mc_log10_p,
          tmpres.chi2PerDoF AS chi2PerDoF,
          tmpres.nDoF_f AS nDoF_f,
          tmpres.ghostProb AS ghostProb,
          abs(p.pid) == 11 AS mc_is_e,
          abs(p.pid) == 13 AS mc_is_mu,
          (abs(p.pid) = 211 OR abs(p.pid) = 321 OR abs(p.pid) = 2212) AS is_h,
          (recguess.track_type == 3) AS is_long, 
          (recguess.track_type == 4) AS is_upstream, 
          (recguess.track_type == 5) AS is_downstream 
        FROM MCParticles AS p
        INNER JOIN MCVertices AS ov ON p.production_vertex = ov.mcvertex_id
        INNER JOIN tmp_particles_recoed_as AS recguess 
          ON p.mcparticle_id = recguess.mcparticle_id
        INNER JOIN tmp_resolution_out AS tmpres 
          ON p.mcparticle_id = tmpres.mcparticle_id
        INNER JOIN tmp_closest_to_beam AS ctb 
          ON p.mcparticle_id = ctb.mcparticle_id
        WHERE 
          recguess.track_type >= 3 AND recguess.track_type <= 5
        )",
        "tmp_covariance_out", 
        {
          "log_cov_ClosestToBeam_0_0",
          "log_cov_ClosestToBeam_1_1",
          "log_cov_ClosestToBeam_2_2",
          "log_cov_ClosestToBeam_3_3",
          "log_cov_ClosestToBeam_4_4",
          "corr_ClosestToBeam_0_1",
          "corr_ClosestToBeam_0_2",
          "corr_ClosestToBeam_1_2",
          "corr_ClosestToBeam_0_3",
          "corr_ClosestToBeam_1_3",
          "corr_ClosestToBeam_2_3",
          "corr_ClosestToBeam_0_4",
          "corr_ClosestToBeam_1_4",
          "corr_ClosestToBeam_2_4",
          "corr_ClosestToBeam_3_4"
        }, 
        128,
        {"mcparticle_id"}
      );

  covariance_model.execute();

  SQLamarr::TemporaryTable covariance (db,
      "covariance", 
      { "cov00", "cov01", "cov02", "cov03", "cov04", 
                 "cov11", "cov02", "cov03", "cov04", 
                          "cov22", "cov03", "cov04", 
                                   "cov33", "cov04", 
                                            "cov04" 
      },
      R"(
      WITH shortcut AS (
          SELECT
            sqrt(exp(log_cov_ClosestToBeam_0_0)) AS sig0, 
            sqrt(exp(log_cov_ClosestToBeam_1_1)) AS sig1, 
            sqrt(exp(log_cov_ClosestToBeam_2_2)) AS sig2, 
            sqrt(exp(log_cov_ClosestToBeam_3_3)) AS sig3, 
            sqrt(exp(log_cov_ClosestToBeam_4_4)) AS sig4, 
            corr_ClosestToBeam_0_1               AS cor01,
            corr_ClosestToBeam_0_2               AS cor02,
            corr_ClosestToBeam_1_2               AS cor03,
            corr_ClosestToBeam_0_3               AS cor04,
            corr_ClosestToBeam_1_3               AS cor11,
            corr_ClosestToBeam_2_3               AS cor12,
            corr_ClosestToBeam_0_4               AS cor13,
            corr_ClosestToBeam_1_4               AS cor23,
            corr_ClosestToBeam_2_4               AS cor24,
            corr_ClosestToBeam_3_4               AS cor34
          FROM tmp_covariance_out
        )
      SELECT
        sig0 * sig0,
        cor01 * sig0 * sig1, 
        cor02 * sig0 * sig2, 
        cor03 * sig0 * sig3, 
        cor04 * sig0 * sig4, 
        sig1 * sig1,
        cor12 * sig1 * sig2, 
        cor13 * sig1 * sig3, 
        cor14 * sig1 * sig4, 
        sig2 * sig2,
        cor23 * sig2 * sig3, 
        cor24 * sig2 * sig4, 
        sig3 * sig3,
        cor34 * sig3 * sig4, 
        sig4 * sig3
      FROM shortcut;
      )", /*make_persistent*/ true);



  std::cout << SQLamarr::dump_table(db, "SELECT COUNT(*) as n_files FROM DataSources") << std::endl;
  std::cout << SQLamarr::dump_table(db, "SELECT * FROM DataSources LIMIT 10") << std::endl;
  std::cout << SQLamarr::dump_table(db, "SELECT * FROM GenEvents LIMIT 10") << std::endl;
  std::cout << SQLamarr::dump_table(db, "SELECT * FROM GenVertices LIMIT 10") << std::endl;
  std::cout << SQLamarr::dump_table(db, "SELECT * FROM GenParticles LIMIT 10") << std::endl;
  std::cout << SQLamarr::dump_table(db, "SELECT * FROM GenParticles WHERE status =889 LIMIT 10") << std::endl;
  std::cout << SQLamarr::dump_table(db, "SELECT * FROM MCVertices LIMIT 10") << std::endl;
  std::cout << SQLamarr::dump_table(db, 
      "SELECT * FROM MCParticles WHERE is_signal == TRUE LIMIT 10") << std::endl;
  std::cout << SQLamarr::dump_table(db, R"(
    SELECT pid, COUNT(*) 
    FROM GenParticles 
    WHERE 
      status == 889 
      AND
      production_vertex IS NOT NULL 
      AND 
      end_vertex IS NOT NULL 
      AND 
      production_vertex != end_vertex 
    GROUP BY pid;
    )");

  std::cout << SQLamarr::dump_table(db, "SELECT * FROM Vertices LIMIT 10") << std::endl;

  std::cout << SQLamarr::dump_table(db, R"(
    SELECT acc.acceptance AS acc, eff.*,
      not_recoed + long + upstream + downstream AS norm,
      red.track_type
      FROM tmp_efficiency_out as eff
      INNER JOIN tmp_acceptance_out as acc 
        ON eff.mcparticle_id = acc.mcparticle_id
      LEFT JOIN tmp_particles_recoed_as AS red
        ON eff.mcparticle_id = red.mcparticle_id
      LIMIT 10)") << std::endl;

  std::cout << SQLamarr::dump_table(db, R"(
      SELECT * FROM tmp_resolution_out LIMIT 10
    )");

  std::cout << SQLamarr::dump_table(db, R"(
      SELECT * FROM tmp_covariance_out LIMIT 10
    )");

  std::cout << SQLamarr::dump_table(db, R"(
    SELECT * FROM tmp_closest_to_beam LIMIT 10
  )");
     

  return 0;
}


std::vector<std::string> globVector(const std::string& pattern)
{
  std::vector<std::string> files;

  glob_t glob_result;
  glob(pattern.c_str(),GLOB_TILDE,NULL,&glob_result);

  for(unsigned int i = 0; i < glob_result.gl_pathc; ++i)
    files.push_back(string(glob_result.gl_pathv[i]));

  globfree(&glob_result);

  return files;
}
