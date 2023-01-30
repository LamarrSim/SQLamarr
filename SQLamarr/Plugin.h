#pragma once

// STL
#include <vector>
#include <string>

// SQLamarr
#include "SQLamarr/db_functions.h"
#include "SQLamarr/BaseSqlInterface.h"

namespace SQLamarr
{
  class Plugin: public BaseSqlInterface
  {
    public:
      typedef float *(*mlfunc)(float *, const float*);

      Plugin (
          SQLite3DB& db,
          const std::string& library,
          const std::string& function_name,
          const std::string& select_query,
          const std::string& output_table,
          const std::vector<std::string> outputs,
          const std::vector<std::string> reserved_keys = {"jKey"}
          );

      void execute ();

    protected:
      virtual void eval_parametrization (float* output, const float* input)
      { m_func(output, input); }

    private:
      const std::string m_library;
      const std::string m_function_name;
      const std::string m_select_query;
      const std::string m_output_table;
      const std::vector<std::string> m_outputs;
      const std::vector<std::string> m_reskeys;

      void *m_handle;
      mlfunc m_func;

      std::vector<std::string>  get_column_names() const;
      std::string  compose_delete_query();
      std::string  compose_create_query();
      std::string  compose_insert_query();
  };

}
