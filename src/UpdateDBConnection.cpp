// (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration.
//
// This software is distributed under the terms of the GNU General Public
// Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
//
// In applying this licence, CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


// STL
#include <memory>
#include <random>

// SQLite3
#include "sqlite3.h"

// SQLamarr
#include "SQLamarr/UpdateDBConnection.h"
#include "SQLamarr/GlobalPRNG.h"
#include "SQLamarr/db_functions.h"

namespace SQLamarr
{
  //============================================================================
  // Constructor
  //============================================================================
  UpdateDBConnection::UpdateDBConnection(
      SQLite3DB& db,
      std::string filename,
      int flags
      )
    : m_database(db)
    , m_filename(filename)
    , m_flags(flags)
  {}

  //============================================================================
  // execute
  //============================================================================
  void UpdateDBConnection::execute()
  {
    update_db_connection(m_database, m_filename, m_flags);
  }
}


