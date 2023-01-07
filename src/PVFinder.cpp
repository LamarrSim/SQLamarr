#include "Lamarr/PVFinder.h"
#include "Lamarr/preprocessor_symbols.h"
#include <iostream>

namespace Lamarr
{
  PVFinder::PVFinder (
          SQLite3DB& db, 
          std::string_view input_table, 
          std::string_view output_table
          )
    : BaseSqlInterface(db)
    , m_input_table(input_table)
    , m_output_table(output_table)
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
        WHERE status==889
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

    std::cout 
      << "Imported PVs" << std::endl
      << dump_table(m_database, "SELECT SUM(is_primary) AS nPV FROM GenVertices") 
      << std::endl;

    sqlite3_step(make_pv_from_signal);

    std::cout 
      << "Imported PVs + PVs from signal" << std::endl
      << dump_table(m_database, "SELECT SUM(is_primary) AS nPV FROM GenVertices") 
      << std::endl;

    sqlite3_step(make_pv_from_barcode);

    std::cout 
      << "Imported PVs + PVs from signal + PVs from barcode" << std::endl
      << dump_table(m_database, "SELECT SUM(is_primary) AS nPV FROM GenVertices") 
      << std::endl;

  }
}
