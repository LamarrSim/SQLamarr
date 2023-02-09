// STL
#include <sstream>

// SQLite
#include "sqlite3.h"

// SQLamarr
#include "SQLamarr/TemporaryTable.h"

namespace SQLamarr
{
  //============================================================================
  // Constructor (from list of statements)
  //============================================================================
  TemporaryTable::TemporaryTable (
          SQLite3DB& db,
          const std::string& output_table,
          const std::vector<std::string>& columns,
          const std::vector<std::string>& select_statements,
          bool make_persistent
          )
    : BaseSqlInterface (db)
    , m_output_table (output_table)
    , m_columns (columns)
    , m_select_statements (select_statements)
    , m_make_persistent (make_persistent)
  {
    validate_token (output_table);
    for (auto& column_name: m_columns)
      validate_token (column_name);
  }
  
  //============================================================================
  // Constructor (from a single statement)
  //============================================================================
  TemporaryTable::TemporaryTable (
          SQLite3DB& db,
          const std::string& output_table,
          const std::vector<std::string>& columns,
          const std::string& select_statement,
          bool make_persistent
          )
    : BaseSqlInterface (db)
    , m_output_table (output_table)
    , m_columns (columns)
    , m_select_statements ({select_statement})
    , m_make_persistent (make_persistent)
  {
    validate_token (output_table);
    for (auto& column_name: m_columns)
      validate_token (column_name);
  }

  //============================================================================
  // compose_create_query. Internal
  //============================================================================
  std::string TemporaryTable::compose_create_query() const
  {
    std::stringstream s;
    s << "CREATE ";

    if (!m_make_persistent)
      s << "TEMPORARY ";

    s << "TABLE IF NOT EXISTS " 
      << m_output_table << "(";

    for (auto c: m_columns)
      s << c << (c != m_columns.back() ? ", ": "");

    s << ");";

    return s.str();
  }

  //============================================================================
  // compose_delete_query. Internal.
  //============================================================================
  std::string TemporaryTable::compose_delete_query() const
  {
    std::stringstream s;
    s << "DELETE FROM " << m_output_table << ";";
    return s.str();
  }

  //============================================================================
  // compose_create_query. Internal.
  //============================================================================
  std::string TemporaryTable::compose_insert_query(const std::string& st) const
  {
    std::stringstream s;
    s << "INSERT INTO " << m_output_table << " (";

    for (auto c: m_columns)
      s << c << (c != m_columns.back() ? ", ": "");
    s << ") ";

    s << st;

    return s.str();
  }

  //============================================================================
  // Execute
  //============================================================================
  void TemporaryTable::execute ()
  {
    // Prepare the queries and initialize the database
    // CREATE TEMPORARY TABLE IF NOT EXISTS
    sqlite3_stmt* create_output_table = get_statement(
        "create_output_table", compose_create_query().c_str()
        );
    exec_stmt(create_output_table);
    
    // DELETE FROM table
    sqlite3_stmt* delete_output_table = get_statement(
        "delete_output_table", compose_delete_query().c_str()
      );
    exec_stmt(delete_output_table);

    // INSERT INTO TABLE
    int c = 0;
    char buffer[128];
    for (auto& stmt: m_select_statements)
    {
      sprintf(buffer, "insert_in_output_table_%d", c++);
      exec_stmt(get_statement(buffer, compose_insert_query(stmt).c_str()));
    }
  }
}
