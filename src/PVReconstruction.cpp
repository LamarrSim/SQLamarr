// (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration.
//
// This software is distributed under the terms of the GNU General Public
// Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
//
// In applying this licence, CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


// STL
#include <iostream>
#include <random>

// SQLite3 
#include "sqlite3.h"

// Local
#include "SQLamarr/preprocessor_symbols.h"
#include "SQLamarr/PVReconstruction.h"
#include "SQLamarr/GlobalPRNG.h"
#include "SQLamarr/SQLiteError.h"

namespace SQLamarr 
{
  // Internal helper function
  PVReconstruction::SmearingParametrization_1D _get_param_line (
      sqlite3_stmt* stmt,
      std::string condition,
      std::string coord
      )
  {
    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt, 1, condition.c_str(), -1, nullptr);
    sqlite3_bind_text(stmt, 2, coord.c_str(), -1, nullptr);

    if (sqlite3_step(stmt) != SQLITE_ROW)
    {
      std::cerr 
        << "Failed query for " << condition 
        << " coordinate: " << coord 
        << std::endl; 
      throw SQLiteError("Cannot load parametrization line");
    }

    return {
        static_cast<float>(sqlite3_column_double (stmt, 0)),
        static_cast<float>(sqlite3_column_double (stmt, 1)),
        static_cast<float>(sqlite3_column_double (stmt, 2)),
        static_cast<float>(sqlite3_column_double (stmt, 3)),
        static_cast<float>(sqlite3_column_double (stmt, 4)),
        static_cast<float>(sqlite3_column_double (stmt, 5))
    };
  }


  //============================================================================
  // Constructor
  //============================================================================
  PVReconstruction::PVReconstruction (
      SQLite3DB& db,
      const SmearingParametrization& parametrization 
      )
    : BaseSqlInterface(db)
    , m_parametrization (parametrization)
  {
    using_sql_function( "rnd_ggg", 6, &_sql_rnd_ggg );
  }

  //============================================================================
  // SQLite3 extension: rnd_ggg
  //============================================================================
  void PVReconstruction::_sql_rnd_ggg (
      sqlite3_context *context,
      int argc,
      sqlite3_value **argv
      )
  {
    if (argc == 6)
    {
      const double mu = sqlite3_value_double(argv[0]);
      const double f1 = sqlite3_value_double(argv[1]);
      const double f2 = sqlite3_value_double(argv[2]);
      const double sigma1 = sqlite3_value_double(argv[3]);
      const double sigma2 = sqlite3_value_double(argv[4]);
      const double sigma3 = sqlite3_value_double(argv[5]);

      auto generator = GlobalPRNG::get_or_create(context);
      std::normal_distribution<double> one_g;
      std::uniform_real_distribution<double> uniform;
      const double r = uniform(*generator);
      const double sigma = (
          r < f1 ? sigma1 :
          r < f1 + f2 ? sigma2 :
          sigma3
          );

      const double smear = one_g(*generator)*sigma + mu;
      
      sqlite3_result_double(context, smear);
      return;
    }
    sqlite3_result_null(context);
  }



  //============================================================================
  // load_parametrization
  //============================================================================
  PVReconstruction::SmearingParametrization 
    PVReconstruction::load_parametrization (
          const std::string file_path,
          const std::string table_name,
          const std::string condition
      )
  {
    sqlite3* db;

    sqlite3_open_v2(file_path.c_str(), &db, SQLITE_OPEN_READONLY, nullptr);

    if (!db)
      throw SQLiteError("Cannot open PVReconstruction DB");

    char query[1024];
    sprintf(query, R"(
    SELECT 
      mu, f1, f2, sigma1, sigma2, sigma3
    FROM %s
    WHERE 
      condition = ?
      AND 
      coord = ?
    COLLATE NOCASE;
    )", table_name.c_str());

    sqlite3_stmt* load_stmt;
    if (sqlite3_prepare_v2(db, query, -1, &load_stmt, nullptr) != SQLITE_OK)
    {
      std::cerr << sqlite3_errmsg(db) << std::endl;
      throw SQLiteError("Failed preparing a statement");
    }
    

    SmearingParametrization ret;
    ret.x() = _get_param_line (load_stmt, condition, "x");
    ret.y() = _get_param_line (load_stmt, condition, "y");
    ret.z() = _get_param_line (load_stmt, condition, "z");
    
    sqlite3_finalize(load_stmt);
    sqlite3_close(db);


    return ret;
  }


  //============================================================================
  // execute
  //============================================================================
  void PVReconstruction::execute ()
  {
    sqlite3_stmt* reco_pv = get_statement("reco_pv", R"(
      INSERT INTO Vertices (
        mcvertex_id, genevent_id, 
        vertex_type, 
        x, y, z,
        sigma_x, sigma_y, sigma_z 
        )
      SELECT 
        mcv.mcvertex_id, mcv.genevent_id, 
        ? AS vertex_type, 
        mcv.x + rnd_ggg(?, ?, ?, ?, ?, ?), 
        mcv.y + rnd_ggg(?, ?, ?, ?, ?, ?), 
        mcv.z + rnd_ggg(?, ?, ?, ?, ?, ?),
        ? AS sigma_x,
        ? AS sigma_y,
        ? AS sigma_z
      FROM MCVertices AS mcv
      WHERE mcv.is_primary == TRUE
      )");

    int slot_id = 1;
    sqlite3_bind_int(reco_pv, slot_id++, LAMARR_VERTEX_PRIMARY);
    
    for (int iCoord = 0; iCoord < 3; ++iCoord)
    {
      sqlite3_bind_double(reco_pv, slot_id++, m_parametrization.data[iCoord].mu);
      sqlite3_bind_double(reco_pv, slot_id++, m_parametrization.data[iCoord].f1);
      sqlite3_bind_double(reco_pv, slot_id++, m_parametrization.data[iCoord].f2);
      sqlite3_bind_double(reco_pv, slot_id++, m_parametrization.data[iCoord].sigma1);
      sqlite3_bind_double(reco_pv, slot_id++, m_parametrization.data[iCoord].sigma2);
      sqlite3_bind_double(reco_pv, slot_id++, m_parametrization.data[iCoord].sigma3);
    }

    for (int iCoord = 0; iCoord < 3; ++iCoord)
    {
      float min_sigma = m_parametrization.data[iCoord].sigma1;
      if (min_sigma < m_parametrization.data[iCoord].sigma2) 
        min_sigma = m_parametrization.data[iCoord].sigma2;
      if (min_sigma < m_parametrization.data[iCoord].sigma3) 
        min_sigma = m_parametrization.data[iCoord].sigma3;

      sqlite3_bind_double(reco_pv, slot_id++, min_sigma);
    }


    exec_stmt(reco_pv);
  }
}
