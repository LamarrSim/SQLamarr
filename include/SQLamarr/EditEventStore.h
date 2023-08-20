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
#include "SQLamarr/Transformer.h"

namespace SQLamarr
{
  /** Execute a query non returning anything, but modifying the event store.
   *
   * Sometimes it may be necessary to modify the event model during the
   * execution of a pipeline, for example to drop tables or to add columns 
   * to existing ones, or even to update some table.
   *
   * EditEventStore provides a generic building block which enables 
   * executing arbitrary queries to the SQLite3 database, without storing the 
   * output, if any.
   *
   */
  class EditEventStore: public BaseSqlInterface, public Transformer
  {
    public:
      /// Constructor
      EditEventStore (
          SQLite3DB& db, 
            ///< Reference to the database
          const std::vector<std::string>& queries
            ///< Vector of queries to be executed in a single transaction
          );


      /// Execute the algorithm, cleaning the database 
      void execute () override;

    private: 
      const std::vector<std::string> m_queries;
  };
}

