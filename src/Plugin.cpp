// Standard 
#include <dlfcn.h>
#include <iostream>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <iterator>

// SQLite3
#include "sqlite3.h"

// SQLamarr
#include "SQLamarr/Plugin.h"

namespace SQLamarr 
{
  //============================================================================
  // Constructor
  //============================================================================
  Plugin::Plugin(
          SQLite3DB& db,
          const std::string& library,
          const std::string& function_name,
          const std::string& select_query,
          const std::string& output_table,
          const std::vector<std::string> outputs,
          const std::vector<std::string> reference_keys
      )
    : BaseSqlInterface(db)
    , m_library (library)
    , m_function_name (function_name)
    , m_select_query (select_query)
    , m_output_table (output_table)
    , m_outputs (outputs)
    , m_refkeys (reference_keys)
    , m_handle (dlopen(library.c_str(), RTLD_LAZY))
  {
    if (!m_handle)
    {
      std::cerr << "Failure while loading " << m_library << std::endl;
      throw std::runtime_error("Failed loading library");
    }

    m_func = mlfunc(dlsym(m_handle, m_function_name.c_str()));
    if (!m_func)
    {
      std::cerr << "Failure while loading " << m_function_name << std::endl;
      throw std::runtime_error("Failed loading function");
    }

    // Throw an error if tokens are not alphanumeric (possible SQL injection)
    validate_token(m_output_table);
    for (const std::string& t: m_outputs) validate_token(t);
    for (const std::string& t: m_refkeys) validate_token(t);
  }

  //============================================================================
  // get_column_names. Internal.
  //============================================================================
  std::vector<std::string> Plugin::get_column_names() const
  {
    std::vector<std::string> ret;
    ret.insert(ret.end(), m_refkeys.begin(), m_refkeys.end());
    ret.insert(ret.end(), m_outputs.begin(), m_outputs.end());
    
    return ret;
  }

  //============================================================================
  // compose_delete_query. Internal.
  //============================================================================
  std::string Plugin::compose_delete_query()
  {
    std::stringstream s;
    s << "DELETE FROM " << m_output_table << ";";
    return s.str();
  }

  //============================================================================
  // compose_create_query. Internal.
  //============================================================================
  std::string Plugin::compose_create_query()
  {
    std::stringstream s;
    s << "CREATE TEMPORARY TABLE IF NOT EXISTS " 
      << m_output_table << "(";

    for (auto c: m_refkeys)
      s << c << " INTEGER" << ", ";

    for (auto c: m_outputs)
      s << c << " REAL" << (c != m_outputs.back() ? ", ": "");

    s << ");";

    return s.str();
  }

  //============================================================================
  // compose_create_query. Internal.
  //============================================================================
  std::string Plugin::compose_insert_query()
  {
    std::stringstream s;
    s << "INSERT INTO " << m_output_table << " (";

    std::vector<std::string> col_names = get_column_names();
    for (auto c: col_names)
      s << c << (c != col_names.back() ? ", ": "");
    s << ") VALUES ( ";

    for (auto c: col_names)
      s << "?" << (c != col_names.back() ? ", ": "");
    s << ");";

    return s.str();
  }

  //============================================================================
  // execute
  //============================================================================
  void Plugin::execute ()
  {
    begin_transaction();

    // Prepare the queries and initialize the database
    // CREATE TEMPORARY TABLE IF NOT EXISTS
    sqlite3_stmt* create_output_table = get_statement(
        "create_output_table", compose_create_query().c_str()
        );
    sqlite3_step(create_output_table);
    
    // DELETE FROM table
    sqlite3_stmt* delete_output_table = get_statement(
        "delete_output_table", compose_delete_query().c_str()
      );
    sqlite3_step(delete_output_table);

    // INSERT INTO TABLE
    sqlite3_stmt* insert_in_output_table = get_statement(
        "insert_in_output_table", compose_insert_query().c_str()
        );

    // SELECT ... FROM 
    sqlite3_stmt* select_input = get_statement(
        "select_input",
        m_select_query.c_str()
        );


    // Main loop on selected rows
    while (sqlite3_step(select_input) == SQLITE_ROW)
    {
      sqlite3_reset(insert_in_output_table);
      // Buffers for parametrization input and output
      std::vector<float> input;
      std::vector<float> output(m_outputs.size());

      // Loop on the columns of each row
      const int nCols = sqlite3_column_count(select_input);
      for (int iCol=0; iCol < nCols; ++iCol)
      {
        // Check for reserved column (external indices)
        const std::string column(sqlite3_column_name(select_input, iCol));
        std::vector<std::string> col_names = get_column_names();
        auto col_iterator = std::find(m_refkeys.begin(), m_refkeys.end(), column);

        // if an index, wraps it to the insert query
        if (col_iterator != m_refkeys.end())
        {

          sqlite3_bind_int(
              insert_in_output_table, 
              1 + col_iterator - m_refkeys.begin(), 
              sqlite3_column_int(select_input, iCol)
              );
        }
        else // otherwise, it is an input for the parametrization
          input.push_back(read_as_float(select_input, iCol));
      }

      // Execute the external function defining the parametrization

      m_func(output.data(), input.data());

      // Fill the output table with the parametrization output
      for (int iOutput = 0; iOutput < m_outputs.size(); ++iOutput)
        sqlite3_bind_double(
            insert_in_output_table, 
            m_refkeys.size() + iOutput + 1, 
            output[iOutput]
            );

      sqlite3_step(insert_in_output_table);
    }

    end_transaction();
  }
}
