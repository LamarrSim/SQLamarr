#include "SQLamarr/PVFinder.h"
#include "SQLamarr/preprocessor_symbols.h"
#include <iostream>

namespace SQLamarr
{
  PVFinder::PVFinder (SQLite3DB& db, int signal_status_code)
    : BaseSqlInterface(db)
    , m_signal_status_code (signal_status_code)
  {}

  void PVFinder::execute(void) 
  {
    sqlite3_stmt* make_pv_from_signal = get_statement("make_pv_from_signal", R"(
      WITH noPV_events
      AS (
        SELECT genevent_id
        FROM GenVertices 
        GROUP BY genevent_id
        HAVING 
          SUM(is_primary) = 0
      ),
      signal_root
      AS
      (
        SELECT 
          genevent_id, 
          FIRST_VALUE(production_vertex) 
            OVER (PARTITION BY genevent_id ORDER BY hepmc_id ASC) AS root_vtx
        FROM GenParticles 
        WHERE status==?
        GROUP BY genevent_id
      ),
      selected_vertices 
      AS
      (
        SELECT genvertex_id 
        FROM GenVertices AS v
        INNER JOIN noPV_events AS pv ON v.genevent_id = pv.genevent_id
        INNER JOIN signal_root AS p ON p.root_vtx = v.genvertex_id
      )
      UPDATE GenVertices
      SET 
        is_primary = TRUE
      WHERE EXISTS (
        SELECT NULL FROM selected_vertices 
        WHERE selected_vertices.genvertex_id = GenVertices.genvertex_id
        );
    )");

    sqlite3_stmt* make_pv_from_barcode = get_statement("make_pv_from_barcode", R"(
      WITH noPV_events
      AS (
        SELECT genevent_id
        FROM GenVertices 
        GROUP BY genevent_id
        HAVING 
          SUM(is_primary) = 0
      ),
      signal_root
      AS
      (
        SELECT 
          genevent_id, 
          FIRST_VALUE(production_vertex) 
            OVER (PARTITION BY genevent_id ORDER BY hepmc_id ASC) AS root_vtx
        FROM GenParticles 
        GROUP BY genevent_id
      ),
      selected_vertices 
      AS
      (
        SELECT genvertex_id 
        FROM GenVertices AS v
        INNER JOIN noPV_events AS pv ON v.genevent_id = pv.genevent_id
        INNER JOIN signal_root AS p ON p.root_vtx = v.genvertex_id
      )
      UPDATE GenVertices
      SET 
        is_primary = TRUE
      WHERE EXISTS (
        SELECT NULL FROM selected_vertices 
        WHERE selected_vertices.genvertex_id = GenVertices.genvertex_id
        );
    )");


    sqlite3_stmt* import_pv = get_statement("import_pv", R"(
      INSERT INTO MCVertices (
        genvertex_id, genevent_id, 
        status, is_primary, 
        t, x, y, z 
        )
      SELECT 
        genvertex_id, genevent_id, 
        status, is_primary, t, x, y, z
      FROM GenVertices
      WHERE 
        is_primary == TRUE
      GROUP BY genevent_id
      HAVING 
        hepmc_id == MAX(hepmc_id);
      )");


    sqlite3_stmt* count_missing_PVs = get_statement("count_missing_PVs", R"(
        SELECT COUNT(genevent_id)
        FROM GenVertices 
        GROUP BY genevent_id
        HAVING 
          SUM(is_primary) = 0
      )");

    sqlite3_step(count_missing_PVs);
    if (sqlite3_column_int(count_missing_PVs, 0))
    {
      sqlite3_bind_int(make_pv_from_signal, 1, m_signal_status_code);
      sqlite3_step(make_pv_from_signal);
    }

    sqlite3_reset(count_missing_PVs);

    if (sqlite3_column_int(count_missing_PVs, 0))
      sqlite3_step(make_pv_from_barcode);

    sqlite3_step(import_pv);

  }
}
