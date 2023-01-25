// STL
#include <string_view>
#include <memory>

// HepMC3
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/ReaderAsciiHepMC2.h"

#include "SQLamarr/db_functions.h"
#include "SQLamarr/BaseSqlInterface.h"

namespace SQLamarr
{
  /** 
    Abstract Interface for loading data to the internal database.
  
    `AbsDataLoader` provides a generic interface to load generator-level 
    data to the database from external sources. 

    The input data follows the schema of the HepMC data format with 
    generator-level vertices (`GenVertex`) nodes connected by generator-level
    particles (`GenParticle`). Each particle connects at most two vertices:
     - a `production_vertex` representing the origin of the particle;
     - an `end_vertex` representing the end of the particle trajectory.

    Each `GenVertex` may have multiple input *and* multiple output particles. 

    In the HepMC standard, vertices and particles belonging to the same 
    graph are grouped in a `GenEvent` object.
    Unfortunately, in LHCb as in most experiments with accelerators, 
    the word "event" refers to a time-slice corresponding to a bunch crossing 
    and may include multiple concurrent collisions, each resulting in an 
    independent graph. 

    In Lamarr, we stick to such a naming convention: 
     - what HepMC names as `GenEvent`, in Lamarr is a `collision`
     - a collection of simultaneous `GenEvent`s due to pile-up, is an `event`.
    
    `AbsDataLoader` should be implemented to provide the logic for *loading*
    the data, without the need of re-implementing the interactions with 
    the database.
  */
  class AbsDataLoader: public BaseSqlInterface
  {
    public:
      using BaseSqlInterface::BaseSqlInterface;

    protected: 
      
      /// Insert data source reference in the `DataSources` table
      int insert_event (
          /// Unique identifer of the data source, e.g. a file path 
          std::string_view data_source,

          /// Unique identifier of the run
          uint64_t run_number,

          /// Unique identifier of the event 
          uint64_t evt_number
          );

      /// Insert a collision in the `GenEvent` table.
      /// Coordinates represent the origin of the event enabling global
      /// and rigid translation of the whole decay tree.
      int insert_collision (
          /// Unique identifier of the HEP event
          int datasource_id, 
          /// HepMC identifier of the `GenEvent`
          int collision,
          float t, ///< Time coordinate
          float x, ///< x coordinate 
          float y, ///< y coordinate
          float z  ///< z coordinate
          );

      /// Insert a vertex in the `GenVertices`
      int insert_vertex (
          int genevent_id, ///< Global identifier of the collision
          int hepmc_id,    ///< HepMC identifier of the vertex
          int status,      ///< HepMC status
          float t,  ///< Vertex time coordinate
          float x,  ///< Vertex *x* coordinate
          float y,  ///< Vertex *y* coordinate
          float z,  ///< Vertex *z* coordinate
          bool is_primary ///< Boolean flag identifying primary vertices
          );

      /// Insert a particle in the `GenParticles` table
      int insert_particle (
          int genevent_id, ///< Global identifier of the collision
          int hepmc_id,    ///< HepMC identifier of the particle
          int production_vertex, ///< Global identifier of the production vertex
          int end_vertex,  ///< Global identifier of the end vertex
          int pid,         ///< PDG Identifier of the particle type
          int status,      ///< HepMC status value
          float pe,        ///< Energy of the particle
          float px,        ///< *x* coordinate of the momentum
          float py,        ///< *y* coordinate of the momentum
          float pz,        ///< *z* coordinate of the momentum
          float m          ///< Generated mass which in HepMC may differ from
                           ///  \f$\sqrt{e^2-p^2}\f$ for resonances
          );
  };
}

