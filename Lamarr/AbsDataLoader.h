// STL
#include <string_view>
#include <memory>

// HepMC3
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/ReaderAsciiHepMC2.h"

#include "Lamarr/db_functions.h"
#include "Lamarr/BaseSqlInterface.h"

namespace Lamarr
{
  class AbsDataLoader: public BaseSqlInterface
  {
    public:
      using BaseSqlInterface::BaseSqlInterface;

    protected: // insert functions
      int insert_datasource (
          std::string_view data_source,
          uint64_t run_number,
          uint64_t evt_number
          );

      int insert_event (
          int datasource_id, 
          int collision,
          float t,
          float x, 
          float y,
          float z
          );

      int insert_vertex (
          int genevent_id,
          int hepmc_id,
          int status,
          float t,
          float x, 
          float y,
          float z,
          bool is_primary
          );

      int insert_particle (
          int genevent_id,
          int hepmc_id,
          int production_vertex,
          int end_vertex,
          int pid,
          int status,
          float pe,
          float px,
          float py,
          float pz,
          float m
          );
  };
}

