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
      PVFinder (
          SQLite3DB& db, 
          std::string_view input_table, 
          std::string_view output_table
          );

      void execute();

    private:  // members
      std::string_view m_input_table, m_output_table;

    private: // methods
      int get_pv_as_beam_endvertex (int genevent_id);
      
  };
}
