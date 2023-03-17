// (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration.
//
// This software is distributed under the terms of the GNU General Public
// Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
//
// In applying this licence, CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

// Standard C
#include <dlfcn.h>

// STL
#include <random>

// SQLamarr
#include "SQLamarr/Plugin.h"
#include "SQLamarr/GlobalPRNG.h"

#include <iostream> 


namespace SQLamarr 
{
  void Plugin::eval_parametrization (float* output, const float* input)
  { 
    m_func(output, input); 
  }
  
}

