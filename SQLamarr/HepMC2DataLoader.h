// (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration.
//
// This software is distributed under the terms of the GNU General Public
// Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
//
// In applying this licence, CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


// STL
#include <string>
#include <unordered_map>
#include <memory>

// HepMC3
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/ReaderAsciiHepMC2.h"

#include "SQLamarr/db_functions.h"
#include "SQLamarr/AbsDataLoader.h"

namespace SQLamarr
{
  /** `AbsDataLoader` implementation for Ascii HepMC2 data.
  
  To export an HepMC2 instance of the `GenEvent` class, one should 
  use code similar to:
  ```cpp
  #include "HepMC/IO_GenEvent.h"

  ...

  HepMC::IO_GenEvent ascii_out("my_event.mc2");
  for (GenEvent* gen_event: collisions_in_the_event)
    ascii_out << evt;
  ``` 
 
  HepMC2DataLoader assumes each of the `GenEvent` instances in the input 
  file represent a collision in the same, single, event.

  The `HepMC2DataLoader::load` function can be called multiple times 
  passing the path of new file, and the corresponding run and event number, 
  as arguments.

  For example,
  ```cpp
  std::vector<std::string> input_files = {"my_event_1.mc2", "my_event_2.mc2"};
  SQLite3DB db = make_database(":memory:");
  HepMC2DataLoader loader (db);

  const int runNumber = 0xE0001;
  int evtNumber = 1;

  for (auto input_file: input_files)
    loader.load(file_path, runNumber, evtNumber++);
  ```

  */
  class HepMC2DataLoader: public AbsDataLoader
  {
    public:
      using AbsDataLoader::AbsDataLoader;

      /// Load an event (possibly including multiple `GenEvent`s) from an ASCII
      /// file, as produced by the HepMC2 `IO_GenEvent` class. 
      void load (
          const std::string& file_path, ///< Full path to the ASCII file
          size_t run_number,          ///< Unique identifier of the run 
          size_t evt_number           ///< Unique identifier of the event 
          );
  };
}
