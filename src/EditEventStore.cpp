// (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration.
//
// This software is distributed under the terms of the GNU General Public
// Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
//
// In applying this licence, CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


// STL
#include <iostream>
#include <algorithm>

// SQLite3
#include "sqlite3.h"

// SQLamarr
#include "SQLamarr/EditEventStore.h"

namespace SQLamarr
{
  //============================================================================
  // Constructor
  //============================================================================
  EditEventStore::EditEventStore(
      SQLite3DB& db,
      const std::vector<std::string>& queries
      )
    : BaseSqlInterface(db)
    , m_queries (queries)
  {}

  //============================================================================
  // execute
  //============================================================================
  void EditEventStore::execute()
  {
    int counter = 0;
    char buffer[1024];
    exec_stmt(get_statement("BEGIN", "BEGIN EXCLUSIVE TRANSACTION"));
    for (auto& query: m_queries)
    {
      sprintf(buffer, "EditEventStore%02d", counter++);
      exec_stmt(get_statement(buffer, query.c_str()));
    }
      
    exec_stmt(get_statement("COMMIT", "COMMIT TRANSACTION"));
  }
}

