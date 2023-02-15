// (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration.
//
// This software is distributed under the terms of the GNU General Public
// Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
//
// In applying this licence, CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


#pragma once
//STL
#include <vector>
#include <string>

// SQLamarr
#include "SQLamarr/Plugin.h"

namespace SQLamarr
{
  /// Interface to dynamically linked *generative* parametrizations.
  ///
  /// A `SQLamarr::lugin` is a parametrization defined in a shared object
  /// dynamically linked at run time.
  /// 
  /// `GenerativePlugin` specialize the `Plugin` class to parametrizations
  /// taking as an input one or more normally distributed random features.
  /// These additional random features are usually injected at some point 
  /// of the pipeline generating random features according to a pdf 
  /// conditioned by the features. 
  /// 
  /// As for the `Plugin` class, `GenerativePlugin` takes care of 
  /// selecting the input columns from the database, links the external 
  /// library based on its path and the name of the function and finally 
  /// creates a table with the output.
  /// 
  /// If a table with the same name as the output table exists in the 
  /// database, it is overwritten without warning.
  ///
  /// In order to match the output table to the input, one or more 
  /// integer variables can be defined as reference keys which are not 
  /// used as inputs for the parametrization, but transparently copied to 
  /// the output table.
  ///
  class GenerativePlugin: public Plugin
  {
    public:
      /// Constructor
      GenerativePlugin (
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
          unsigned int n_random,
            ///< Size of the random space to be concatenated to the 
            ///  conditions as an input to GANs
          const std::vector<std::string> reference_keys = {"ref_id"}
            ///< List of column names ignored.
          ) 
          : Plugin(db, library, function_name, select_query, 
              output_table, outputs, reference_keys)
          , m_n_random (n_random)
      {}

    protected:
      virtual void eval_parametrization (float* output, const float* input);
      typedef float *(*ganfunc)(float *, const float*, const float*);


    private:
      unsigned int m_n_random;
  };
}


