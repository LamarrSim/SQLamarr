#pragma once

// STL
#include <string>
#include <vector>

// SQLamarr
#include "SQLamarr/BaseSqlInterface.h"
#include "SQLamarr/db_functions.h"


namespace SQLamarr
{
  /** A simplified interface to create and populate temporary tables.

  Temporary tables are used to store partial results with one 
  (expensive or random) producer and multiple consumers. 

  The class SQLamarr::TemporaryTable provides a simplified interface 
  to define operations resulting into a temporary table.

  When `execute()` is call, tables in the database with the same 
  name as the output table are overwritten without further warning.

  Example.
  ```cpp
  SQLamarr::SQLite3DB db = SQLamarr::make_database(":memory:");

  SQLamarr::TemporaryTable some_parts(db,
    "my_output_table", {"mcparticle_id", "output_column_2"},
    "SELECT mcparticle_id, random_uniform() < 0.1 FROM MCParticles"
    );

  some_parts.execute();
  ```

  See also:
   * `SQLamarr::Plugin` creating temporary tables based on an external lib
   * `SQLamarr::GenerativePlugin` creating a temporary table with random numbers

  */
  class TemporaryTable: public BaseSqlInterface
  {
    public:
      /// Define the operations to define to create the temp table
      TemporaryTable (
          SQLite3DB& db,
            ///< Reference to the database
          const std::string& output_table,
            ///< Name of the output table
          const std::vector<std::string>& columns,
            ///< List of the names assigned to the output columns
          const std::string& select_statement
            ///< An SQL SELECT statement selecting exactly the same 
            ///  number of variables as defined by the column argument
          );


      /// Execute the SQL statements creating and populating the temp table
      void execute ();

    private:
      // Name of the output temporary table
      const std::string m_output_table;

      // Name of the output temporary table
      const std::vector<std::string> m_columns;

      // Select statement
      const std::string m_select_statement;

    private: 
      std::string compose_create_query() const;
      std::string compose_delete_query() const;
      std::string compose_insert_query() const;
  };
}
