// (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration.
//
// This software is distributed under the terms of the GNU General Public
// Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
//
// In applying this licence, CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


#pragma once
#include <iostream>
#include <string>
#include <unordered_map>

#include "SQLamarr/BaseSqlInterface.h"
#include "SQLamarr/db_functions.h"

namespace SQLamarr
{
  /** Abstract interface with helper functions to access an SQLite DB

    SQLamarr uses SQLite to replace the Transient Event Store of Gaudi
    and to provide persistency in a consistent, standard format easly 
    read and written in C/C++ and other languages.

    The helper class BaseSqlInterface defines helper functions to prepare
    SQL queries once and run them multiple times on different batches of data.

    It also standardize the calls to the BEGIN and END transaction queries that
    are used to drastically speed up sets of multiple queries when using a 
    database with persistency.

  */
  class BaseSqlInterface
  {
    public: 
      /// Constructor, acquiring the database without ownership
      BaseSqlInterface(SQLite3DB& db);
      virtual ~BaseSqlInterface();

    protected: // members
      SQLite3DB& m_database; ///< Reference to the SQLite database (not owned).


    private: //members
      std::unordered_map<std::string, sqlite3_stmt*> m_queries;

    protected: // methods
      /// Creates or retrieve from cache a statement
      sqlite3_stmt* get_statement (
          const std::string& name,      ///< Human-readable uid of the query
                                        ///  used as key in a hash table
          const std::string& query      ///< SQL query used to initialize the 
                                        ///  in case it is not present in cache
          );

      /// Begin an SQL transaction stopping update to disk util
      ///`end_transaction()` is issued 
      void begin_transaction () { sqlite3_exec(m_database.get(), "BEGIN", 0, 0, 0); }

      /// End an SQL transaction re-enabling disk updates
      void end_transaction () { sqlite3_exec(m_database.get(), "END", 0, 0, 0); }

      /// Return the index of the last rows inserted in any table
      int last_insert_row () { return sqlite3_last_insert_rowid(m_database.get()); }

      /// Register a static function in DB, enabling usage from SQL.
      /// 
      /// Function prototype should be:
      /// ```
      /// void (sqlite3_context* context, int argc, sqlite3_value** argv);
      /// ```
      void using_sql_function (
          const std::string& name,  ///< Name of the SQL function
          int argc,                 ///< Max. number of arguments
          void (*xFunc)(sqlite3_context*, int, sqlite3_value**) 
                                    ///< Function pointer
          );

      /// Execute a statement, possibly throwing an exception on failure
      bool exec_stmt (sqlite3_stmt*);
  };
}
