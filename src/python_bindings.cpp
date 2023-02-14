#include <string.h>
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
#include "SQLamarr/CleanEventStore.h"

using SQLamarr::SQLite3DB;

std::vector<std::string> tokenize (const char*);

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
// Plugin
//==============================================================================
extern "C"
void *new_Plugin (
    void *db,
    const char* library_path,
    const char* function_name,
    const char* query,
    const char* output_table,
    const char* comma_separated_outputs,
    const char* comma_separated_references
    )
{
  SQLite3DB *udb = reinterpret_cast<SQLite3DB *>(db);

  return reinterpret_cast<void *> (new SQLamarr::Plugin(*udb,
        library_path,
        function_name,
        query,
        output_table,
        tokenize(comma_separated_outputs),
        tokenize(comma_separated_references)
        ));
}

extern "C"
void del_Plugin (void *self)
{
  delete reinterpret_cast<SQLamarr::Plugin*>(self);
}

//==============================================================================
// GenerativePlugin
//==============================================================================
extern "C"
void *new_GenerativePlugin (
    void *db,
    const char* library_path,
    const char* function_name,
    const char* query,
    const char* output_table,
    const char* comma_separated_outputs,
    int n_random,
    const char* comma_separated_references
    )
{
  SQLite3DB *udb = reinterpret_cast<SQLite3DB *>(db);

  return reinterpret_cast<void *> (new SQLamarr::GenerativePlugin(*udb,
        library_path,
        function_name,
        query,
        output_table,
        tokenize(comma_separated_outputs),
        n_random,
        tokenize(comma_separated_references)
        ));
}

extern "C"
void del_GenerativePlugin (void *self)
{
  delete reinterpret_cast<SQLamarr::GenerativePlugin*>(self);
}

//==============================================================================
// TemporaryTable
//==============================================================================
extern "C"
void *new_TemporaryTable (
    void *db,
    const char* output_table,
    const char* comma_separated_outputs,
    const char* query,
    bool make_persistent
    )
{
  SQLite3DB *udb = reinterpret_cast<SQLite3DB *>(db);

  return reinterpret_cast<void *> (new SQLamarr::TemporaryTable(*udb,
        output_table,
        tokenize(comma_separated_outputs),
        query,
        make_persistent
        ));
}

extern "C"
void del_TemporaryTable (void *self)
{
  delete reinterpret_cast<SQLamarr::TemporaryTable*>(self);
}


//==============================================================================
// CleanEventStore
//==============================================================================
extern "C"
void *new_CleanEventStore (void *db)
{
  SQLite3DB *udb = reinterpret_cast<SQLite3DB *>(db);
  return reinterpret_cast<void *> (new SQLamarr::CleanEventStore(*udb));
}

extern "C"
void del_CleanEventStore (void *self)
{
  delete reinterpret_cast<SQLamarr::CleanEventStore*>(self);
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


//==============================================================================
// Additional functions
//==============================================================================
std::vector<std::string> tokenize (const char* input_str)
{
  // Return buffer
  std::vector<std::string> ret;

  // Copy the input string in a writable buffer
  char buf[1024];
  strcpy(buf, input_str);

  // Tokenize the string
  char *token = strtok(buf, ",");

  while (token != NULL)
  {
    ret.push_back(token);
    token = strtok(NULL, ",");
  }

  // Return buffer
  return ret;
}


