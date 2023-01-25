#pragma once
#include <string>
#include <string_view>

#include "sqlite3.h"
#include "SQLamarr/db_functions.h"
#include "SQLamarr/BaseSqlInterface.h"

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
  class PVFinder: public BaseSqlInterface
  {
    public:
      /// Constructor, with configurable signal status code
      PVFinder (SQLite3DB& db, int signal_status_code = 889);

      /// Execute the algorithm
      void execute();

    private: // properties
      int m_signal_status_code;
      
  };
}
