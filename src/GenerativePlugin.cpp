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
#include "SQLamarr/GenerativePlugin.h"
#include "SQLamarr/GlobalPRNG.h"

#include <iostream> 


namespace SQLamarr 
{
  void GenerativePlugin::eval_parametrization (float* output, const float* input)
  { 
    std::vector<float> rnd(m_n_random);
    auto generator = GlobalPRNG::get_or_create(m_database.get());
    std::normal_distribution<float> gaussian;

    for (auto& r: rnd) 
      r = gaussian(*generator);

    m_func(output, input, rnd.data()); 
  }
}
