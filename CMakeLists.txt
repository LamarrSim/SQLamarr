cmake_minimum_required(VERSION 3.1)
project(Lamarr)

  set(CMAKE_CXX_STANDARD 17)
  find_package(HepMC3 3.2.5 REQUIRED PATHS /home/lucio/hepmc3/hepmc3-install)
  set(SQLite3_LIBRARIES /usr/lib/x86_64-linux-gnu/libsqlite3.so)
  set(SQLite3_INCLUDE_DIRS /usr/include)

  set(LAMARR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}) 

  include_directories(${HEPMC3_INCLUDE_DIR})
  include_directories(${SQLite_INCLUDE_DIRS})
  include_directories(${LAMARR_INCLUDE_DIR})

  add_executable(Lamarr
      src/AbsDataLoader.cpp
      src/HepMC2DataLoader.cpp
      src/db_functions.cpp
      src/main.cpp
      )

  target_link_libraries(Lamarr ${HEPMC3_LIB} ${SQLite3_LIBRARIES})
