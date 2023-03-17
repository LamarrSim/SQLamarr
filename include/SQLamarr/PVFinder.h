// (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration.
//
// This software is distributed under the terms of the GNU General Public
// Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
//
// In applying this licence, CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


#pragma once
#include <string>
#include <string_view>

#include "sqlite3.h"
#include "SQLamarr/db_functions.h"
#include "SQLamarr/BaseSqlInterface.h"
#include "SQLamarr/Transformer.h"

namespace SQLamarr
{
  /** Identifies the primary vertices

    Primary vertices are identified using up to three different methods. 
    The first method succeeding interrupts the search and the resulting 
    `GenVertex` is imported to the `MCVertices` table as a primary vertex.

    Methods:
      1. If a vertex was marked as primary when importing the data, no
         further search is performed for that `GenEvent` (or collision);
      2. If particles with "signal" status code are found, the production vertex
         of the particle with lower HepMC ID is assigned the role of PV;
      3. The production vertex of the particle with the lowest HepMC ID
         is called the PV.

      The value of the HepMC status identifying signal particles depends 
      may depend on the generator and is defined as argument of the 
      constructor.
  */ 
  class PVFinder: public BaseSqlInterface, public Transformer
  {
    public:
      /// Constructor, with configurable signal status code
      PVFinder (SQLite3DB& db, int signal_status_code = 889);

      /// Execute the algorithm
      void execute() override;

    private: // properties
      int m_signal_status_code;
      
  };
}
