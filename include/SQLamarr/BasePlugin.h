// (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration.
//
// This software is distributed under the terms of the GNU General Public
// Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
//
// In applying this licence, CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


#pragma once

// Standard C
#include <dlfcn.h>

// STL
#include <vector>
#include <string>

// SQLamarr
#include "SQLamarr/db_functions.h"
#include "SQLamarr/BaseSqlInterface.h"
#include "SQLamarr/Transformer.h"

namespace SQLamarr
{
  /// Interface to dynamically linked parametrizations.
  ///
  /// A SQLamarr `BasePlugin` is a parametrization defined in a shared object
  /// dynamically linked at run time.
  /// For an example to generate such parametrizations, please refer to 
  /// the [scikinC](https://github.com/landerlini/scikinC) project.
  /// 
  /// The `BasePlugin` class is an abstract class defining a generic interface.
  /// Client applications are expected to use specialized plugins, obtained 
  /// by defining the function signature and its call. 
  /// 
  /// In particular, the function `eval_parametrization` is a pure virtual 
  /// function that must be overridden with the logic to call the 
  /// dynamically loaded function with the appropriate signature.
  /// 
  /// The `BasePlugin` class implements selecting the input columns from 
  /// the database, links the external library based on its path and 
  /// the name of the function and finally creates a table with the output.
  /// 
  /// If a table with the same name as the output table exists in the 
  /// database, it is overwritten without warning.
  ///
  /// In order to match the output table to the input, one or more 
  /// integer variables can be defined as reference keys which are not 
  /// used as inputs for the parametrization, but transparently copied to 
  /// the output table.
  ///
  /// @see SQLamarr::Plugin implementing the function signature 
  ///       `float* (float*, const float*)`
  /// @see SQLamarr::GenerativePlugin implementing the function signature
  ///       `float* (float*, const float*)`
  /// 
  class BasePlugin: public BaseSqlInterface, public Transformer
  {
    public:
      /// Constructor
      BasePlugin (
          SQLite3DB& db,
            ///< Reference to the database, passed without ownership
          const std::string& library,
            ///< Path to the shared object (library). If in CWD, prepend "./".
          const std::string& function_name,
            ///< Linking symbol of the target function as set at compile-time
          const std::string& select_query,
            ///< SQL Query selecting the inputs. Number and order of the
            ///  parameters columns are relevant. Names not listed in
            /// `reference_keys` are ignored.
          const std::string& output_table,
            ///< SQL name of the output table. Must be alphanumeric. 
          const std::vector<std::string> outputs,
            ///< Vector of column names for the output table. 
            ///  The order must match the output produced by the
            ///  compiled parametrization.
          const std::vector<std::string> reference_keys = {"ref_id"}
            ///< List of column names ignored.
          );

      BasePlugin (BasePlugin&) = delete;

      virtual ~BasePlugin() {dlclose(m_handle);}

      /// Execute the external function and copies the output 
      /// in a new table.
      void execute () override;

    protected:
      /// Evaluate the external parametrization. This function can be 
      /// overridden to provide custom preprocessing and postprocessing steps,
      /// or to link to functions with custom signature.
      virtual void eval_parametrization (float* output, const float* input) = 0;

    private: // Properties
      const std::string m_library;
      const std::string m_function_name;
      const std::string m_select_query;
      const std::string m_output_table;
      const std::vector<std::string> m_outputs;
      const std::vector<std::string> m_refkeys;

      void *m_handle;

    private: // Methods
      std::vector<std::string>  get_column_names() const;
      std::string  compose_delete_query();
      std::string  compose_create_query();
      std::string  compose_insert_query();

    protected:
      /// Load a generic-typed function from an external library
      template <typename Func_t> Func_t load_func (const std::string& fname);
  };


  // Templated implementation
  template <typename Func_t> 
  Func_t BasePlugin::load_func (const std::string& fname)
  {
    Func_t ret = Func_t(dlsym(m_handle, fname.c_str()));

    if (!ret)
    {
      std::cerr << "Failure while loading " << fname << std::endl;
      throw std::runtime_error("Failed loading function");
    }

    return ret;
  }
}
