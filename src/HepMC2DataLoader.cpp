// STL
#include <iostream>

// Local
#include "Lamarr/HepMC2DataLoader.h"
#include "Lamarr/db_functions.h"
#include "Lamarr/errorcodes.h"

namespace Lamarr
{
  //==========================================================================
  // load
  //==========================================================================
  void HepMC2DataLoader::load (
      std::string_view file_path, 
      size_t run_number, 
      size_t evt_number
      )
  {
    begin_transaction();
    const int ds_id = insert_datasource(file_path, run_number, evt_number);

    HepMC3::ReaderAsciiHepMC2 reader(file_path.data());
    while ( !reader.failed() ) 
    {
      HepMC3::GenEvent evt(HepMC3::Units::MEV, HepMC3::Units::MM);
      reader.read_event(evt);

      auto pos = evt.event_pos();
      const int event_id = insert_event(
          ds_id,
          evt.event_number(),
          pos.t(),
          pos.x(),
          pos.y(),
          pos.z()
          );

      std::unordered_map<int, int> vtxid_mapping;
      for (auto vertex: evt.vertices())
        vtxid_mapping[vertex->id()] = insert_vertex(
              event_id,
              vertex->id(),
              vertex->status(),
              vertex->position().t(),
              vertex->position().x(),
              vertex->position().y(),
              vertex->position().z()
            );

      for (auto particle: evt.particles())
      {
        auto pv = particle->production_vertex();
        auto ev = particle->end_vertex();


        insert_particle(
              event_id,
              particle->id(),
              (pv ? vtxid_mapping[pv->id()] : LAMARR_BAD_INDEX),
              (ev ? vtxid_mapping[ev->id()] : LAMARR_BAD_INDEX),
              particle->pid(),
              particle->status(),
              particle->momentum().e(),
              particle->momentum().px(),
              particle->momentum().py(),
              particle->momentum().pz(),
              particle->generated_mass()
            );
      }
    }

    end_transaction();
  }
}

