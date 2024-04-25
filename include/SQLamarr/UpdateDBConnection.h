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

// SQLamarr
#include "SQLamarr/db_functions.h"
#include "SQLamarr/Transformer.h"

namespace SQLamarr
{
  /** Reset the database connection forcing flushing the db status.
   *
   * In the interaction with Python or other frameworks it is sometimes
   * necessary to ensure db synchronization with disk or shared memory.
   * This can be achieved refreshing the connection to the database, 
   * by closing it and reopening. 
   *
   * WARNING! Executing UpdateDBConnection drops TEMPORARY tables and views. 
   */
  class UpdateDBConnection: public Transformer
  {
    public:
      /// Constructor
      UpdateDBConnection (
          SQLite3DB& db, 
            ///< Reference to the database
          std::string filename,
            ///< Filename or URI of the (possibly new) connection to the database
          int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI
            ///< Flags
          );


      /// Execute the algorithm, cleaning the database 
      void execute () override;

    private: // members
      SQLite3DB& m_database; ///< Reference to the SQLite database (not owned).
      const std::string m_filename; ///< Filename or URI of the database
      const int m_flags; ///< SQLite flags to open to database (see sqlite_open_v3)
  };
}


