// STL
#include <iostream>

// Local
#include "SQLamarr/AbsDataLoader.h"
#include "SQLamarr/db_functions.h"
#include "SQLamarr/preprocessor_symbols.h"

namespace SQLamarr
{
  //==========================================================================
  // insert_event
  //==========================================================================
  int AbsDataLoader::insert_event (
      std::string_view datasource,
      uint64_t run_number,
      uint64_t evt_number
      )
  {
    sqlite3_stmt* stmt = get_statement ("insert_event",
        "INSERT INTO DataSources(datasource, run_number, evt_number) "
        "VALUES (?, ?, ?); "
        );

    sqlite3_bind_text (stmt, 1, datasource.data(), datasource.length()+1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, run_number);
    sqlite3_bind_int64(stmt, 3, evt_number);

    exec_stmt(stmt);

    return last_insert_row();
  }


  //==========================================================================
  // insert_collision
  //==========================================================================
  int AbsDataLoader::insert_collision (
      int datasource_id,
      int collision,
      float t,
      float x,
      float y,
      float z
      )
  {
    sqlite3_stmt* stmt = get_statement ("insert_collision",
        "INSERT INTO GenEvents(datasource_id, collision, x, y, z, t) "
        "VALUES (?, ?, ?, ?, ?, ?) "
        );

    sqlite3_bind_int(stmt, 1, datasource_id);
    sqlite3_bind_int(stmt, 2, collision);
    sqlite3_bind_double(stmt, 3, x);
    sqlite3_bind_double(stmt, 4, y);
    sqlite3_bind_double(stmt, 5, z);
    sqlite3_bind_double(stmt, 6, t);

    exec_stmt(stmt);

    return last_insert_row();
  }


  //==========================================================================
  // insert_vertex
  //==========================================================================
  int AbsDataLoader::insert_vertex (
      int genevent_id,
      int hepmc_id,
      int status,
      float t,
      float x, 
      float y,
      float z,
      bool is_primary
      )
  {
    sqlite3_stmt* stmt = get_statement ("insert_vertex",
        "INSERT INTO GenVertices"
        "  (genevent_id, hepmc_id, status, x, y, z, t, is_primary) "
        "VALUES (?, ?, ?, ?,  ?, ?, ?, ?) "
        );

    sqlite3_bind_int(stmt, 1, genevent_id);
    sqlite3_bind_int(stmt, 2, hepmc_id);
    sqlite3_bind_int(stmt, 3, status);
    sqlite3_bind_double(stmt, 4, x);
    sqlite3_bind_double(stmt, 5, y);
    sqlite3_bind_double(stmt, 6, z);
    sqlite3_bind_double(stmt, 7, t);
    sqlite3_bind_int(stmt, 8, int(is_primary));

    exec_stmt(stmt);

    return last_insert_row();
  }

  //==========================================================================
  // insert_particle
  //==========================================================================
  int AbsDataLoader::insert_particle (
      int genevent_id,
      int hepmc_id,
      int production_vertex,
      int end_vertex,
      int pid,
      int status,
      float pe,
      float px,
      float py,
      float pz,
      float m
      )
  {
    sqlite3_stmt* stmt = get_statement ("insert_particle", R"(
        INSERT INTO GenParticles(
            genevent_id, hepmc_id, 
            production_vertex, end_vertex,
            pid, status, 
            pe, px, py, pz, m
          )
          VALUES (?, ?,  ?, ?,  ?, ?,  ?, ?, ?, ?, ?); "
        )");

    int iVar = 1;
    sqlite3_bind_int(stmt, iVar++, genevent_id);
    sqlite3_bind_int(stmt, iVar++, hepmc_id);

    if (production_vertex != LAMARR_BAD_INDEX)
      sqlite3_bind_int(stmt, iVar++, production_vertex);
    else
    {
      sqlite3_bind_null(stmt, iVar++);
    }

    if (end_vertex != LAMARR_BAD_INDEX)
      sqlite3_bind_int(stmt, iVar++, end_vertex);
    else
      sqlite3_bind_null(stmt, iVar++);

    sqlite3_bind_int(stmt, iVar++, pid);
    sqlite3_bind_int(stmt, iVar++, status);
    sqlite3_bind_double(stmt, iVar++, pe);
    sqlite3_bind_double(stmt, iVar++, px);
    sqlite3_bind_double(stmt, iVar++, py);
    sqlite3_bind_double(stmt, iVar++, pz);
    sqlite3_bind_double(stmt, iVar++, m);

    exec_stmt(stmt);

    return last_insert_row();
  }
}


