#pragma once
#include <string>
#include <unordered_map>

#include "Lamarr/BaseSqlInterface.h"
#include "Lamarr/db_functions.h"

namespace Lamarr
{
  class BaseSqlInterface
  {
    public: // constructor
      BaseSqlInterface(SQLite3DB& db);

    protected: // members
      SQLite3DB& m_database;


    private: //members
      std::unordered_map<std::string, sqlite3_stmt*> m_queries;

    protected: // methods
      sqlite3_stmt* get_statement (
          const std::string& name, 
          const std::string_view query
          );

      void begin_transaction () { sqlite3_exec(m_database.get(), "BEGIN", 0, 0, 0); }
      void end_transaction () { sqlite3_exec(m_database.get(), "END", 0, 0, 0); }

      int last_insert_row () { return sqlite3_last_insert_rowid(m_database.get()); }
  };
}
