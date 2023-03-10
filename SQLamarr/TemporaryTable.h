// (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration.
//
// This software is distributed under the terms of the GNU General Public
// Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
//
// In applying this licence, CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


#pragma once

// STL
#include <string>
#include <vector>

// SQLamarr
#include "SQLamarr/BaseSqlInterface.h"
#include "SQLamarr/db_functions.h"
#include "SQLamarr/Transformer.h"



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
  class TemporaryTable: public BaseSqlInterface, public Transformer
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
          const std::vector<std::string>& select_statements,
            ///< A list of SQL SELECT statements selecting exactly the same 
            ///  number of variables as defined by the column argument
          bool make_persistent = false
            ///< Override the temporary configuration to make 
            //   the table "persistent"
          );
     
      /// Define the operations to define to create the temp table
      TemporaryTable (
          SQLite3DB& db,
            ///< Reference to the database
          const std::string& output_table,
            ///< Name of the output table
          const std::vector<std::string>& columns,
            ///< List of the names assigned to the output columns
          const std::string& select_statement,
            ///< A SQL SELECT statements selecting exactly the same 
            ///  number of variables as defined by the column argument
          bool make_persistent = false
            ///< Override the temporary configuration to make 
            //   the table "persistent"
          );


      /// Execute the SQL statements creating and populating the temp table
      void execute () override;

    private:
      // Name of the output temporary table
      const std::string m_output_table;

      // Name of the output temporary table
      const std::vector<std::string> m_columns;

      // Select statement
      const std::vector<std::string> m_select_statements;

    private: 
      std::string compose_create_query() const;
      std::string compose_delete_query() const;
      std::string compose_insert_query(const std::string& st) const;
      bool m_make_persistent;
  };
}
