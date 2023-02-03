// STL
#include <sstream>

// SQLite
#include "sqlite3.h"

// SQLamarr
#include "SQLamarr/TemporaryTable.h"

namespace SQLamarr
{
  //============================================================================
  // Constructor
  //============================================================================
  TemporaryTable::TemporaryTable (
          SQLite3DB& db,
          const std::string& output_table,
          const std::vector<std::string>& columns,
          const std::string& select_statement
          )
    : BaseSqlInterface (db)
    , m_output_table (output_table)
    , m_columns (columns)
    , m_select_statement (select_statement)
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
    s << "CREATE TEMPORARY TABLE IF NOT EXISTS " 
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
  std::string TemporaryTable::compose_insert_query() const
  {
    std::stringstream s;
    s << "INSERT INTO " << m_output_table << " (";

    for (auto c: m_columns)
      s << c << (c != m_columns.back() ? ", ": "");
    s << ") ";

    s << m_select_statement;

    return s.str();
  }

  //============================================================================
  // Constructor
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
    sqlite3_stmt* insert_in_output_table = get_statement(
        "insert_in_output_table", compose_insert_query().c_str()
        );
    exec_stmt(insert_in_output_table);
  }
}
