// (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration.
//
// This software is distributed under the terms of the GNU General Public
// Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
//
// In applying this licence, CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


#pragma once

#include <unordered_map>
#include <algorithm>
#include <random>
#include <memory>
#include <iostream>
#include <mutex>

#include "sqlite3.h"

namespace SQLamarr
{
  /** Singleton handler of Pseudo-Random Number Generator(s)
  
  The generation of pseudo-random numbers for Monte Carlo applications is a 
  critical aspect in the design of an application.
  
  Pseudo-random numbers should be generated upon a known seed.
  Reproducibility of the complete simulation process must 
  be guaranteed, provided the same seed.

  A further complication arises from multithreaded applications 
  with multiple threads sharing a single Pseudo-Random Number 
  Generator (PRNG). Indeed, the order with which threads may
  advance the sequence of pseudo-random numbers is defined by 
  the order of execution of threads which is, by nature, 
  stocastic. 

  In SQLamarr, PRNs are generated with a generator connected 
  to the database instance. Database instances are supposed to 
  live in a thread as concurrent access to the same SQLite 
  database introduces an easily avoidable overhead.
  If such a good practice is followed, then each thread 
  gets associated to a different random number generator, seeded 
  indepedently (for example based on the number of threads).

  From a technical perspective, a Singleton GlobalPRNG class 
  handles a hash table associating a random number generator 
  to each known database instance. 

  On the attempt to access a generator for a database instance
  not previously mapped in the hash table, a new instance is 
  allocated, using the seed provided as an argument.
  If the seed is not provided, an exception is thrown, unless
  explicitely ignored compiling with flag
  `-DALLOW_RANDOM_DEVICE_FOR_SEEDING`.


  Generators can be reseeded at any time, by accessing the 
  generator associated to an existing database instance, while 
  passing the seed.

  Any pseudo random number generator defined according the STL
  generator interface can be used to specialize the template 
  class `T_GlobalPRNG` defining the behaviour described above.

  The default `GlobalPRNG` specialization uses a RANLUX 
  with 48 bit generator as
  [provided](https://cplusplus.com/reference/random/ranlux48/) 
  by the C++ STL.

  Note on multithreading. A std::unordered_map is used to associate a 
  random number generator to a given database. Since the operations 
  of consulting and updating an unordered_map are not thread-safe
  and since, by design, such a mapping must be shared across multiple
  threads, the access to the map is protected by a **std::mutex**. 

  */  
  template <class PRNG>
  class T_GlobalPRNG
  {
    public: // static methods
      /// Higher part of the int64 is used to identify missing 
      /// or invalid seed.
      static constexpr uint64_t no_seed = 0xBAD0000000000000L;


      /// Return a handle to the hash table, not to the single 
      /// generator (see below).
      static T_GlobalPRNG& handle()
      {
        static T_GlobalPRNG instance;
        return instance;
      }


      /// Return a pointer to an initialized generator
      static PRNG* get_or_create (
          const sqlite3_context* db, 
          uint64_t seed = no_seed
          )
      {
        sqlite3_context* db_ = const_cast<sqlite3_context*> (db);
        return get_or_create(sqlite3_context_db_handle(db_), seed);
      }


      /// Return a pointer to an initialized generator
      static PRNG* get_or_create (
          const sqlite3* db, 
          uint64_t seed = no_seed
          )
      {
        // Gets the singleton handle
        T_GlobalPRNG& h {T_GlobalPRNG::handle()};
        std::lock_guard<std::mutex>(h.m_mutex); // Forbids multithreading

        // Looks for the DB in the hash table
        auto gen_it = h.m_generators.find(db);

        // If initialized return it, possibly after re-seeding.
        if (gen_it != h.m_generators.end()) 
        {
          if (seed == no_seed)
            return gen_it->second.get();

          // Reseeding
          gen_it->second->seed(static_cast<uint32_t>(0xFFFFFFFFL & seed));
          return gen_it->second.get();
        }

        // If it is a new instance:
        //  - if no seed is passed, use random_device for seeding or throw
        if (seed == no_seed)
        {
#ifndef ALLOW_RANDOM_DEVICE_FOR_SEEDING
          std::cerr << "Trying to use unseeded generator. "
            << "Compile with -DALLOW_RANDOM_DEVICE_FOR_SEEDING to ignore."
            << std::endl;
          throw std::logic_error("Random seeding disabled.");
#endif
          std::random_device seeder;
          h.m_generators[db] = std::unique_ptr<PRNG>(new PRNG(seeder()));
          return h.m_generators[db].get();
        }

        //  - if a seed is provided, create a new generator
        h.m_generators[db] = std::unique_ptr<PRNG>(new PRNG(
            static_cast<uint32_t>(0xFFFFFFFFL & seed)
            ));

        return h.m_generators[db].get();
      }

      /// Releases a generator (delete). May require re-seeding.
      static bool release (const sqlite3* db)
      {
        T_GlobalPRNG& h {T_GlobalPRNG::handle()};
        std::lock_guard<std::mutex>(h.m_mutex); // Forbids multithreading

        auto it = h.m_generators.find(db);
        bool releasing_unexisting = (it == h.m_generators.end());
        h.m_generators.erase(db);
        return releasing_unexisting;
      }


    private:
      T_GlobalPRNG() {}

      std::unordered_map<const sqlite3*, std::unique_ptr<PRNG> > m_generators;

      std::mutex m_mutex;

    public:
      /// Copy constructor disabled as per singleton pattern
      T_GlobalPRNG(T_GlobalPRNG const&)    = delete;

      /// Copy operator disabled as per singleton pattern
      void operator=(T_GlobalPRNG const&)  = delete;
  };

  /// Default specialization using RANLUX48
  typedef T_GlobalPRNG<std::ranlux48> GlobalPRNG;

  /// An additional specialization for testing purpose using Marsenne Twister
  typedef T_GlobalPRNG<std::mt19937> GlobalPRNG_MT;
}
