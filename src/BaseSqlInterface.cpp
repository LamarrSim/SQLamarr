#include "SQLamarr/BaseSqlInterface.h"

namespace SQLamarr
{
  BaseSqlInterface::BaseSqlInterface(SQLite3DB& db)
  : m_database (db)
  {}


  //==========================================================================
  // get_statement
  //==========================================================================
  sqlite3_stmt* BaseSqlInterface::get_statement (
      const std::string& name, 
      const std::string_view query
      )
  {
    if (m_queries.find(name) == m_queries.end())
      m_queries[name] = prepare_statement(m_database, query);

    sqlite3_reset(m_queries[name]);
    return m_queries[name];
  }

}
