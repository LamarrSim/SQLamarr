#include <memory>
#include <iostream>
#include "SQLamarr/HepMC2DataLoader.h"
#include "SQLamarr/PVFinder.h"
#include "SQLamarr/db_functions.h"
#include "SQLamarr/MCParticleSelector.h"
#include "SQLamarr/PVReconstruction.h"
#include "SQLamarr/Plugin.h"
#include "SQLamarr/GenerativePlugin.h"
#include "SQLamarr/GlobalPRNG.h"
#include "SQLamarr/TemporaryTable.h"
#include "SQLamarr/BlockLib/LbParticleId.h"

using SQLamarr::SQLite3DB;

//==============================================================================
// make_database
//==============================================================================
extern "C"
void *make_database(const char* db_file)
{
  SQLite3DB *db = new SQLite3DB(SQLamarr::make_database(db_file));
  return reinterpret_cast<void *>(db);
}

//==============================================================================
// delete_database
//==============================================================================
extern "C"
void del_database(void *db)
{
  delete reinterpret_cast<SQLite3DB*> (db);
}

//==============================================================================
// GlobalPRNG_get_or_create
//==============================================================================
extern "C"
void GlobalPRNG_get_or_create(void* db, int seed)
{
  SQLamarr::GlobalPRNG::get_or_create(
      reinterpret_cast<SQLite3DB*>(db)->get(),
      seed
    );
}

//==============================================================================
// HepMC2DataLoader
//==============================================================================
extern "C"
void* new_HepMC2DataLoader (void *db)
{
  SQLite3DB *udb = reinterpret_cast<SQLite3DB *>(db);
  return reinterpret_cast<void *> (new SQLamarr::HepMC2DataLoader(*udb));
}

extern "C"
void del_HepMC2DataLoader (void *self)
{
  delete reinterpret_cast<SQLamarr::HepMC2DataLoader *> (self);
}

extern "C"
void HepMC2DataLoader_load (
      void *self, 
      const char* file_path, 
      size_t runNumber, 
      size_t evtNumber
    )
{
  reinterpret_cast<SQLamarr::HepMC2DataLoader *>(self)
    ->load(file_path, runNumber, evtNumber);
}


//==============================================================================
// PVFinder
//==============================================================================
extern "C"
void *new_PVFinder (void *db, int signal_status)
{
  SQLite3DB *udb = reinterpret_cast<SQLite3DB *>(db);
  return reinterpret_cast<void *> (new SQLamarr::PVFinder(*udb, signal_status));
}

extern "C"
void del_PVFinder (void *self)
{
  delete reinterpret_cast<SQLamarr::PVFinder*>(self);
}

//==============================================================================
// MCParticleSelector
//==============================================================================
extern "C"
void *new_MCParticleSelector (void *db)
{
  SQLite3DB *udb = reinterpret_cast<SQLite3DB *>(db);
  return reinterpret_cast<void *> (new SQLamarr::MCParticleSelector(*udb));
}

extern "C"
void del_MCParticleSelector (void *self)
{
  delete reinterpret_cast<SQLamarr::MCParticleSelector*>(self);
}

//==============================================================================
// PVReconstruction
//==============================================================================
extern "C"
void *new_PVReconstruction (
    void *db, 
    const char* file_path,
    const char* table_name,
    const char* condition
    )
{
  SQLite3DB *udb = reinterpret_cast<SQLite3DB *>(db);
  auto pars = SQLamarr::PVReconstruction::load_parametrization(
      file_path, table_name, condition);


  return reinterpret_cast<void *> (new SQLamarr::PVReconstruction(*udb, pars));
}

extern "C"
void del_PVReconstruction (void *self)
{
  delete reinterpret_cast<SQLamarr::PVReconstruction*>(self);
}


//==============================================================================
// Execute Pipeline
//==============================================================================
extern "C"
void execute_pipeline(int algc, void** algv)
{
  int iAlg;

  for (iAlg = 0; iAlg < algc; ++iAlg)
    reinterpret_cast<SQLamarr::Transformer*> (algv[iAlg])->execute();
}

  
