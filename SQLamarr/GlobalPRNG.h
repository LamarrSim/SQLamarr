#pragma once

#include <unordered_map>
#include <algorithm>
#include <random>
#include <memory>

#include "sqlite3.h"

namespace SQLamarr
{
  template <class PRNG>
  /// Singleton handler of Pseudo-Random Number Generator(s)
  class T_GlobalPRNG
  {
    public: // static methods
      static constexpr uint64_t no_seed = 0xFFFFFFFF00000000L;


      static T_GlobalPRNG& handle()
      {
        static T_GlobalPRNG instance;
        return instance;
      }


      static PRNG* get_or_create (
          const sqlite3_context* db, 
          int64_t seed = no_seed
          )
      {
        sqlite3_context* db_ = const_cast<sqlite3_context*> (db);
        return get_or_create(sqlite3_context_db_handle(db_), seed);
      }


      static PRNG* get_or_create (
          const sqlite3* db, 
          int64_t seed = no_seed
          )
      {
        T_GlobalPRNG& h {T_GlobalPRNG::handle()};

        auto gen_it = h.m_generators.find(db);

        if (gen_it != h.m_generators.end()) // Initialized generator exists
        {
          if (seed == no_seed)
            return gen_it->second.get();

          gen_it->second->seed(static_cast<uint32_t>(0xFFFFFFFFL & seed));
          return gen_it->second.get();
        }

        h.m_generators[db] = std::make_unique<PRNG>(
            (seed == no_seed)
            ? PRNG::default_seed
            : static_cast<uint32_t>(0xFFFFFFFFL & seed)
            );

        return h.m_generators[db].get();
      }

      static bool release (const sqlite3* db)
      {
        T_GlobalPRNG& h {T_GlobalPRNG::handle()};
        auto it = h.m_generators.find(db);
        bool releasing_unexisting = (it == h.m_generators.end());
        h.m_generators.erase(db);
        return releasing_unexisting;
      }


    private:
      T_GlobalPRNG() {}

      std::unordered_map<const sqlite3*, std::unique_ptr<PRNG> > m_generators;

    public:
      T_GlobalPRNG(T_GlobalPRNG const&)      = delete;
      void operator=(T_GlobalPRNG const&)  = delete;
  };

  typedef T_GlobalPRNG<std::mt19937> GlobalPRNG;
}
