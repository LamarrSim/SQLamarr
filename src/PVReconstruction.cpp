#include "Lamarr/PVReconstruction.h"
#include "sqlite3.h"

#include <iostream>

namespace Lamarr 
{
  // Internal helper function
  PVReconstruction::SmearingParametrization_1D _get_param_line (
      sqlite3_stmt* stmt,
      unsigned int year,
      std::string polarity,
      std::string coord
      )
  {
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, year);
    sqlite3_bind_text(stmt, 2, polarity.c_str(), -1, nullptr);
    sqlite3_bind_text(stmt, 3, coord.c_str(), -1, nullptr);

    if (sqlite3_step(stmt) != SQLITE_ROW)
    {
      std::cerr 
        << "Failed query for " << year << "-" << polarity 
        << " coordinate: " << coord 
        << std::endl; 
      throw std::logic_error("Cannot load parametrization line");
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


  PVReconstruction::SmearingParametrization 
    PVReconstruction::load_parametrization (
          const std::string file_path,
          const std::string table_name,
          unsigned int year,
          std::string polarity
      )
  {
    std::cout << "Loading " << file_path << std::endl;
    sqlite3* db;

    sqlite3_open_v2(file_path.c_str(), &db, SQLITE_OPEN_READONLY, nullptr);

    if (!db)
      throw std::logic_error("Cannot open PVReconstruction DB");

    char query[1024];
    sprintf(query, R"(
    SELECT 
      mu, f1, f2, sigma1, sigma2, sigma3
    FROM %s
    WHERE 
      year = ?
      AND 
      polarity = ?
      AND 
      coord = ?
    COLLATE NOCASE;
    )", table_name.c_str());
    sqlite3_stmt* load_stmt;
    if (sqlite3_prepare_v2(db, query, -1, &load_stmt, nullptr) != SQLITE_OK)
    {
      std::cerr << sqlite3_errmsg(db) << std::endl;
      throw std::logic_error ("Failed preparing a statement");
    }
    

    SmearingParametrization ret;
    ret.x() = _get_param_line (load_stmt, year, polarity, "x");
    ret.y() = _get_param_line (load_stmt, year, polarity, "y");
    ret.z() = _get_param_line (load_stmt, year, polarity, "z");
    
    sqlite3_close(db);


    return ret;
  }

}
