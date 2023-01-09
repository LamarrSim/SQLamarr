#pragma once
#include <string>
#include <string_view>

#include "sqlite3.h"
#include "Lamarr/db_functions.h"
#include "Lamarr/BaseSqlInterface.h"

namespace Lamarr 
{
  class PVFinder: public BaseSqlInterface
  {
    public:
      PVFinder (SQLite3DB& db, int signal_status_code = 889);

      void execute();

    private: // properties
      int m_signal_status_code;
      
  };
}
