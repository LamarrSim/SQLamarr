// (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration.
//
// This software is distributed under the terms of the GNU General Public
// Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
//
// In applying this licence, CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


#include <string.h>
#include <memory>
#include <exception>
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
#include "SQLamarr/SQLiteError.h"

constexpr int SQL_ERRORSHIFT = 10000;
constexpr int LOGIC_ERRORSHIFT = 20000;

using SQLamarr::SQLite3DB;

typedef enum {
    PVFinder 
    , MCParticleSelector 
    , PVReconstruction
    , Plugin
    , GenerativePlugin
    , TemporaryTable
    , CleanEventStore
  } TransformerType;

struct TransformerPtr {
  TransformerType dtype; 
  void *p;
};

SQLamarr::Transformer* resolve_polymorphic_transformer(TransformerPtr);
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
TransformerPtr new_PVFinder (void *db, int signal_status)
{
  SQLite3DB *udb = reinterpret_cast<SQLite3DB *>(db);
  auto p = new SQLamarr::PVFinder(*udb, signal_status);
  return {PVFinder, p};
}

//==============================================================================
// MCParticleSelector
//==============================================================================
extern "C"
TransformerPtr new_MCParticleSelector (void *db)
{
  SQLite3DB *udb = reinterpret_cast<SQLite3DB *>(db);
  return {
    MCParticleSelector, 
    static_cast<void *> (new SQLamarr::MCParticleSelector(*udb))
  };
}

//==============================================================================
// PVReconstruction
//==============================================================================
extern "C"
TransformerPtr new_PVReconstruction (
    void *db, 
    const char* file_path,
    const char* table_name,
    const char* condition
    )
{
  SQLite3DB *udb = reinterpret_cast<SQLite3DB *>(db);
  auto pars = SQLamarr::PVReconstruction::load_parametrization(
      file_path, table_name, condition);

  return {PVReconstruction, new SQLamarr::PVReconstruction(*udb, pars)};
}

//==============================================================================
// Plugin
//==============================================================================
extern "C"
TransformerPtr new_Plugin (
    void *db,
    const char* library_path,
    const char* function_name,
    const char* query,
    const char* output_table,
    const char* semicolon_separated_outputs,
    const char* semicolon_separated_references
    )
{
  SQLite3DB *udb = reinterpret_cast<SQLite3DB *>(db);

  return {Plugin, new SQLamarr::Plugin(*udb,
        library_path,
        function_name,
        query,
        output_table,
        tokenize(semicolon_separated_outputs),
        tokenize(semicolon_separated_references)
        )};
}

//==============================================================================
// GenerativePlugin
//==============================================================================
extern "C"
TransformerPtr new_GenerativePlugin (
    void *db,
    const char* library_path,
    const char* function_name,
    const char* query,
    const char* output_table,
    const char* semicolon_separated_outputs,
    int n_random,
    const char* semicolon_separated_references
    )
{
  SQLite3DB *udb = reinterpret_cast<SQLite3DB *>(db);

  return {GenerativePlugin, new SQLamarr::GenerativePlugin(*udb,
        library_path,
        function_name,
        query,
        output_table,
        tokenize(semicolon_separated_outputs),
        n_random,
        tokenize(semicolon_separated_references)
        )};
}


//==============================================================================
// TemporaryTable
//==============================================================================
extern "C"
TransformerPtr new_TemporaryTable (
    void *db,
    const char* output_table,
    const char* semicolon_separated_outputs,
    const char* semicolon_separated_queries,
    bool make_persistent
    )
{
  SQLite3DB *udb = reinterpret_cast<SQLite3DB *>(db);

  return {TemporaryTable, new SQLamarr::TemporaryTable(*udb,
        output_table,
        tokenize(semicolon_separated_outputs),
        tokenize(semicolon_separated_queries),
        make_persistent
        )};
}


//==============================================================================
// CleanEventStore
//==============================================================================
extern "C"
TransformerPtr new_CleanEventStore (void *db)
{
  SQLite3DB *udb = reinterpret_cast<SQLite3DB *>(db);
  return {CleanEventStore, new SQLamarr::CleanEventStore(*udb)};
}

//==============================================================================
// Delete Transformer
//==============================================================================
extern "C"
void del_Transformer (TransformerPtr self)
{
  switch (self.dtype)
  {
    case PVFinder:
      delete reinterpret_cast<SQLamarr::PVFinder*> (self.p);
      break;
    case MCParticleSelector:
      delete reinterpret_cast<SQLamarr::MCParticleSelector*> (self.p);
      break;
    case PVReconstruction:
      delete reinterpret_cast<SQLamarr::PVReconstruction*> (self.p);
      break;
    case Plugin:
      delete reinterpret_cast<SQLamarr::Plugin*> (self.p);
      break;
    case GenerativePlugin:
      delete reinterpret_cast<SQLamarr::GenerativePlugin*> (self.p);
      break;
    case TemporaryTable:
      delete reinterpret_cast<SQLamarr::TemporaryTable*> (self.p);
      break;
    case CleanEventStore:
      delete reinterpret_cast<SQLamarr::CleanEventStore*> (self.p);
      break;
    default:
      throw std::bad_cast();
  }
}


//==============================================================================
// resolve_polymorphic_transformer
//==============================================================================
SQLamarr::Transformer* resolve_polymorphic_transformer(TransformerPtr self)
{
  switch (self.dtype)
  {
    case PVFinder:
      return reinterpret_cast<SQLamarr::PVFinder*> (self.p);
    case MCParticleSelector:
      return reinterpret_cast<SQLamarr::MCParticleSelector*> (self.p);
    case PVReconstruction:
      return reinterpret_cast<SQLamarr::PVReconstruction*> (self.p);
    case Plugin:
      return reinterpret_cast<SQLamarr::Plugin*> (self.p);
    case GenerativePlugin:
      return reinterpret_cast<SQLamarr::GenerativePlugin*> (self.p);
    case TemporaryTable:
      return reinterpret_cast<SQLamarr::TemporaryTable*> (self.p);
    case CleanEventStore:
      return reinterpret_cast<SQLamarr::CleanEventStore*> (self.p);
  }

  throw std::bad_cast();
}

//==============================================================================
// Execute Pipeline
//==============================================================================
extern "C"
int execute_pipeline(int algc, TransformerPtr* algv)
{
  int iAlg;

  for (iAlg = 0; iAlg < algc; ++iAlg)
  {
    SQLamarr::Transformer* t = resolve_polymorphic_transformer(algv[iAlg]);
    try
    {
      t->execute();
    }
    catch (const SQLamarr::SQLiteError& e)
    {
      std::cerr << "Caught SQL error processing #alg: " << iAlg << "\n";
      std::cerr << e.what() << std::endl;
      return SQL_ERRORSHIFT + iAlg; 
    }
    catch (const std::logic_error& e)
    {
      std::cerr << "Caught logic error processing #alg: " << iAlg << "\n";
      std::cerr << e.what() << std::endl;
      return LOGIC_ERRORSHIFT + iAlg; 
    }
  }

  return 0;
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
  char *token = strtok(buf, ";");

  while (token != NULL)
  {
    ret.push_back(token);
    token = strtok(NULL, ";");
  }

  // Return buffer
  return ret;
}


