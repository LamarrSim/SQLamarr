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

  //==========================================================================
  // create_sql_function
  //==========================================================================
  void BaseSqlInterface::using_sql_function (
      const std::string& name,
      int argc,
      void (*xFunc)(sqlite3_context*, int, sqlite3_value**) 
      )
  {
    sqlite3_create_function(
        m_database.get(), 
        name.c_str(), argc,
        SQLITE_UTF8, nullptr, xFunc, nullptr, nullptr
        );
  }

}
