# (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration. 
#                                                                             
# This software is distributed under the terms of the GNU General Public
# Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
#                                                                             
# In applying this licence, CERN does not waive the privileges and immunities
# granted to it by virtue of its status as an Intergovernmental Organization  
# or submit itself to any jurisdiction.

cmake_minimum_required(VERSION 3.1)
project(SQLamarr)

  set(PREFIX_PATH $ENV{CONDA_PREFIX})
  set(CMAKE_CXX_STANDARD 17)

  set(HEPMC3_LIB ${PREFIX_PATH}/lib/libHepMC3.so)
  set(HEPMC3_INCLUDE_DIR ${PREFIX_PATH}/include)
  set(SQLite3_LIBRARIES ${PREFIX_PATH}/lib/libsqlite3.so)
  set(SQLite3_INCLUDE_DIRS ${PREFIX_PATH}/include)

  set(LAMARR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include) 

  include_directories(${HEPMC3_INCLUDE_DIR})
  include_directories(${SQLite_INCLUDE_DIRS})
  include_directories(${LAMARR_INCLUDE_DIR})

  add_executable(hardcoded_pipeline
      src/custom_sql_functions.cpp
      src/db_functions.cpp
      src/BaseSqlInterface.cpp
      src/AbsDataLoader.cpp
      src/HepMC2DataLoader.cpp
      src/PVFinder.cpp
      src/PVReconstruction.cpp
      src/MCParticleSelector.cpp
      src/BasePlugin.cpp
      src/Plugin.cpp
      src/GenerativePlugin.cpp
      src/TemporaryTable.cpp
      src/CleanEventStore.cpp
      src/version.cpp
      src/blib/LbParticleId.cpp
      src/main.cpp
      )

  target_link_libraries(hardcoded_pipeline 
      m dl
      ${HEPMC3_LIB} 
      ${SQLite3_LIBRARIES}
      )

  add_library(SQLamarr SHARED
      src/custom_sql_functions.cpp
      src/db_functions.cpp
      src/BaseSqlInterface.cpp
      src/AbsDataLoader.cpp
      src/HepMC2DataLoader.cpp
      src/PVFinder.cpp
      src/PVReconstruction.cpp
      src/MCParticleSelector.cpp
      src/version.cpp
      src/Plugin.cpp
      src/GenerativePlugin.cpp
      src/TemporaryTable.cpp
      src/CleanEventStore.cpp
      src/python_bindings.cpp
      )

  target_link_libraries(SQLamarr
      m dl
      ${HEPMC3_LIB} 
      ${SQLite3_LIBRARIES}
      )

#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DALLOW_RANDOM_DEVICE_FOR_SEEDING")


  add_custom_target (docs ALL 
      COMMAND doxygen 
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      )

