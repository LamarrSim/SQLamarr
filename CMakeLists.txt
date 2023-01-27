cmake_minimum_required(VERSION 3.1)
project(SQLamarr)

  set(PREFIX_PATH $ENV{CONDA_PREFIX})
  set(CMAKE_CXX_STANDARD 17)

  set(HEPMC3_LIB ${PREFIX_PATH}/lib/libHepMC3.so)
  set(HEPMC3_INCLUDE_DIR ${PREFIX_PATH}/include)
  set(SQLite3_LIBRARIES ${PREFIX_PATH}/lib/libsqlite3.so)
  set(SQLite3_INCLUDE_DIRS ${PREFIX_PATH}/include)

  set(LAMARR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}) 

  include_directories(${HEPMC3_INCLUDE_DIR})
  include_directories(${SQLite_INCLUDE_DIRS})
  include_directories(${LAMARR_INCLUDE_DIR})

  add_executable(SQLamarr
      src/db_functions.cpp
      src/BaseSqlInterface.cpp
      src/AbsDataLoader.cpp
      src/HepMC2DataLoader.cpp
      src/PVFinder.cpp
      src/PVReconstruction.cpp
      src/MCParticleSelector.cpp
      src/main.cpp
      )

  target_link_libraries(SQLamarr ${HEPMC3_LIB} ${SQLite3_LIBRARIES})


  add_custom_target (docs ALL 
      COMMAND doxygen 
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      )

