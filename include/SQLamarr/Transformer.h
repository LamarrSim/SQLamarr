// (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration.
//
// This software is distributed under the terms of the GNU General Public
// Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
//
// In applying this licence, CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


#pragma once

namespace SQLamarr
{
  /// Interface to an executable class used for polymorphism
  class Transformer
  {
    public:
      /// Function to be overridden defining the execution step
      virtual void execute() = 0;

      /// @private Virtual destructor
      virtual ~Transformer() {};
  };
}
