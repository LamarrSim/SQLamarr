#include "Lamarr/MCParticleSelector.h"
#include <iostream>
#include <algorithm>

namespace Lamarr
{
  MCParticleSelector::MCParticleSelector(
      SQLite3DB& db,
      const std::vector<uint64_t> retained_status_values,
      const std::vector<uint64_t> retained_abspid_values
      )
    : BaseSqlInterface(db)
    , m_retained_status_values(retained_status_values)
    , m_retained_abspid_values(retained_abspid_values)
  {}

  void MCParticleSelector::execute()
  {
    sqlite3_stmt* get_root = get_statement("get_root", R"(
        SELECT p.genparticle_id, mcv.mcvertex_id, p.genevent_id, v.genvertex_id 
        FROM GenParticles AS p 
        INNER JOIN GenVertices AS v ON v.genvertex_id = p.production_vertex 
        INNER JOIN MCVertices AS mcv ON p.genevent_id = mcv.genevent_id 
        WHERE v.is_primary == TRUE AND mcv.is_primary == TRUE 
        )");

    begin_transaction();
    while (sqlite3_step(get_root) == SQLITE_ROW)
      process_particle (
          sqlite3_column_int(get_root, 0),
          sqlite3_column_int(get_root, 1)
          );

    end_transaction();
  }

  bool MCParticleSelector::process_particle(int genparticle_id, int prod_vtx)
  {
    sqlite3_stmt* get_particle = get_statement("get_particle", R"(
        SELECT 
          status,
          pid,
          production_vertex IS NOT NULL,
          end_vertex IS NOT NULL
        FROM GenParticles 
        WHERE genparticle_id = ?
      )");
    sqlite3_bind_int(get_particle, 1, genparticle_id);


    sqlite3_stmt* get_daughters = get_statement("get_daughters", R"(
        SELECT daughter.genparticle_id
        FROM GenParticles AS mother
        INNER JOIN GenParticles AS daughter 
          ON mother.end_vertex = daughter.production_vertex
        WHERE mother.genparticle_id = ?
      )");
    sqlite3_bind_int(get_daughters, 1, genparticle_id);


    if (sqlite3_step(get_particle) != SQLITE_ROW)
    {
      std::cerr << "Stop iteration at particle " << genparticle_id << std::endl;
      return false;
    }

    const int status = sqlite3_column_int(get_particle, 0);
    const int pid = sqlite3_column_int(get_particle, 1);
    const int abspid = abs(pid);
    const int valid_prod = sqlite3_column_int(get_particle, 2);
    const int valid_end = sqlite3_column_int(get_particle, 3);

    bool kept = keep(status, abspid);
    const int end_vtx = (kept && valid_end) ? 
      get_or_create_end_vertex(genparticle_id) : prod_vtx;

    std::vector<uint64_t> daughter_ids;
    while (valid_end && sqlite3_step(get_daughters) == SQLITE_ROW)
      daughter_ids.push_back (sqlite3_column_int(get_daughters, 0));

    for (uint64_t daughter_id: daughter_ids)
      process_particle (daughter_id, end_vtx);

    if (valid_prod && kept) // && prod_vtx != end_vtx)
    {
      sqlite3_stmt* insert_mc_particle = get_statement("insert_mc_particle", R"(
        INSERT INTO MCParticles (
          genparticle_id, 
          genevent_id,
          pid, pe, px, py, pz, m,
          is_signal
          )
        SELECT 
          genparticle_id, 
          genevent_id,
          pid, pe, px, py, pz, m,
          status == 889
        FROM GenParticles
        WHERE genparticle_id = ?;
      )");
      sqlite3_bind_int(insert_mc_particle, 1, genparticle_id);
      sqlite3_step(insert_mc_particle);

      const int mcparticle_id = last_insert_row();

      sqlite3_stmt* set_mc_vertices = get_statement("set_mc_vertices", R"(
        UPDATE MCParticles 
        SET
          production_vertex = ?,
          end_vertex = ?
        WHERE 
          mcparticle_id = ?;
      )");
      sqlite3_bind_int(set_mc_vertices, 1, prod_vtx);
      if (valid_end)
        sqlite3_bind_int(set_mc_vertices, 2, end_vtx);
      else
        sqlite3_bind_null(set_mc_vertices, 2);

      sqlite3_bind_int(set_mc_vertices, 3, mcparticle_id);

      sqlite3_step(set_mc_vertices);
    }

    return true;
  }

  bool MCParticleSelector::keep(int status, int abspid) const
  {
    const auto& s_ok = m_retained_status_values;
    if (std::find(s_ok.begin(), s_ok.end(), status) != s_ok.end())
      return true;

    const auto& id_ok = m_retained_abspid_values;
    if (std::find(id_ok.begin(), id_ok.end(), status) != id_ok.end())
      return true;

    if (abspid <= 8) // quarks
      return false;

    if (abspid >= 11 && abspid <= 18) // leptons
      return true;

    return false;
  }

  uint64_t MCParticleSelector::get_or_create_end_vertex (int genparticle_id)
  {
    /** Insert vertex **/
    sqlite3_stmt* insert_end_vertex = get_statement("insert_end_vertex", R"(
      INSERT OR IGNORE INTO 
        MCVertices (genvertex_id, genevent_id, status, is_primary, t, x, y, z)
      SELECT
        gv.genvertex_id, gv.genevent_id, 
        gv.status, gv.is_primary, gv.t, gv.x, gv.y, gv.z
      FROM GenParticles AS gp
      INNER JOIN GenVertices AS gv 
        ON gp.end_vertex == gv.genvertex_id
      WHERE gp.genparticle_id = ? AND is_primary == FALSE;
    )");
    sqlite3_bind_int (insert_end_vertex, 1, genparticle_id);

    sqlite3_stmt* get_end_vertex = get_statement("get_end_vertex", R"(
      SELECT mcv.mcvertex_id
      FROM GenParticles AS gp
      INNER JOIN GenVertices AS gv 
        ON gp.end_vertex == gv.genvertex_id
      INNER JOIN MCVertices AS mcv 
        ON gv.genvertex_id == mcv.genvertex_id
      WHERE gp.genparticle_id = ? 
    )");
    sqlite3_bind_int (get_end_vertex, 1, genparticle_id);

    sqlite3_step(insert_end_vertex);
    if (sqlite3_step(get_end_vertex) != SQLITE_ROW)
      throw std::logic_error("MCParticleSelector failed to insert an end-vertex");

    return sqlite3_column_int (get_end_vertex, 0);
  }
}
