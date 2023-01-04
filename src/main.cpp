#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include "Lamarr/HepMC2DataLoader.h"
#include "Lamarr/db_functions.h"
#include <memory>
#include <glob.h>

#include "sqlite3.h"
#include <stdio.h>

#include <filesystem>


// Helper function to list files in a dir
std::vector<std::string> globVector(const std::string& pattern);


int main(int argc, char* argv[])
{
  std::string path{"/home/lucio/test-data/DSt_Pi.hepmc2/*.mc2"};


  std::vector<std::string> file_paths = globVector(path); 
//  {
//    "/home/lucio/test-data/DSt_Pi.hepmc2/evt0.mc2", 
//    "/home/lucio/test-data/DSt_Pi.hepmc2/evt1.mc2", 
//    "/home/lucio/test-data/DSt_Pi.hepmc2/evt2.mc2", 
//    "/home/lucio/test-data/DSt_Pi.hepmc2/evt3.mc2", 
//    "/home/lucio/test-data/DSt_Pi.hepmc2/evt4.mc2", 
//    "/home/lucio/test-data/DSt_Pi.hepmc2/evt5.mc2" 
//  };
//
  // Create the in-memory database
  Lamarr::SQLite3DB db = Lamarr::make_database(argc > 1 ? argv[1] : ":memory:" );


  // Define a data loader to take data from HepMC2-Ascii format
  Lamarr::HepMC2DataLoader loader (db);
  
  // Loads the data to the database
  size_t evtNumber = 123;
  size_t runNumber = 456;

  for (std::string& file_path: file_paths)
    loader.load(file_path, runNumber, evtNumber++);

  std::cout << Lamarr::dump_table(db, "SELECT COUNT(*) as n_files FROM DataSources") << std::endl;
  std::cout << Lamarr::dump_table(db, "SELECT * FROM DataSources LIMIT 10") << std::endl;
  std::cout << Lamarr::dump_table(db, "SELECT * FROM GenEvents LIMIT 10") << std::endl;
  std::cout << Lamarr::dump_table(db, "SELECT * FROM GenVertices LIMIT 10") << std::endl;
  std::cout << Lamarr::dump_table(db, "SELECT * FROM GenParticles LIMIT 10") << std::endl;

  return 0;
}


std::vector<std::string> globVector(const std::string& pattern)
{
  std::vector<std::string> files;

  glob_t glob_result;
  glob(pattern.c_str(),GLOB_TILDE,NULL,&glob_result);

  for(unsigned int i = 0; i < glob_result.gl_pathc; ++i)
    files.push_back(string(glob_result.gl_pathv[i]));

  globfree(&glob_result);

  return files;
}
