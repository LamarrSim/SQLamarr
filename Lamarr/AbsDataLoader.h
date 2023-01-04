// STL
#include <string_view>
#include <unordered_map>
#include <memory>

// HepMC3
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/ReaderAsciiHepMC2.h"

#include "Lamarr/db_functions.h"

namespace Lamarr
{
  class AbsDataLoader
  {
    public:
      AbsDataLoader(SQLite3DB& db);

    protected: // insert functions
      void begin_transaction () { sqlite3_exec(m_database.get(), "BEGIN", 0, 0, 0); }
      void end_transaction () { sqlite3_exec(m_database.get(), "END", 0, 0, 0); }

      int insert_datasource (
          std::string_view data_source,
          uint64_t run_number,
          uint64_t evt_number
          );

      int insert_event (
          int datasource_id, 
          int collision,
          float t,
          float x, 
          float y,
          float z
          );

      int insert_vertex (
          int genevent_id,
          int hepmc_id,
          int status,
          float t,
          float x, 
          float y,
          float z
          );

      int insert_particle (
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
          );
          



    protected: // members
      SQLite3DB& m_database;

    private: //members
      std::unordered_map<std::string, sqlite3_stmt*> m_queries;


    protected: // methods
      sqlite3_stmt* get_statement (
          const std::string& name, 
          const std::string_view query
          );
  };
}

