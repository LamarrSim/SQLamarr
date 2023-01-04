// STL
#include <string_view>
#include <unordered_map>
#include <memory>

// HepMC3
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/ReaderAsciiHepMC2.h"

#include "Lamarr/db_functions.h"
#include "Lamarr/AbsDataLoader.h"

namespace Lamarr
{
  class HepMC2DataLoader: public AbsDataLoader
  {
    public:
      using AbsDataLoader::AbsDataLoader;

      void load (
          std::string_view file_path, 
          size_t run_number, 
          size_t evt_number
          );
  };
}
