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
  };
}
