#include "SQLamarr/BaseSqlInterface.h"
#include "SQLamarr/custom_sql_functions.h"
#include "sqlite3.h"
#include <iostream>

namespace SQLamarr
{
  //==========================================================================
  // Constructor
  //==========================================================================
  BaseSqlInterface::BaseSqlInterface(SQLite3DB& db)
  : m_database (db)
  {
    sqlamarr_create_sql_functions(db.get());
  }

  //==========================================================================
  // Destructor
  //==========================================================================
  BaseSqlInterface::~BaseSqlInterface()
  {
    for (auto q = m_queries.begin(); q != m_queries.end(); ++q)
      sqlite3_finalize(q->second);
  }

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

  //==========================================================================
  // exec_stmt
  //==========================================================================
  bool BaseSqlInterface::exec_stmt (sqlite3_stmt* stmt)
  {
    switch (sqlite3_step(stmt))
    {
      case SQLITE_DONE: return false;
      case SQLITE_ROW:  return true;
      default:
        std::cerr << sqlite3_errmsg(m_database.get()) << std::endl;
        throw std::logic_error("SQL Error");
    }

    return false;
  }

}
