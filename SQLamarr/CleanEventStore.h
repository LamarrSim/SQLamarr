// (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration.
//
// This software is distributed under the terms of the GNU General Public
// Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
//
// In applying this licence, CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


#pragma once

#include "SQLamarr/BaseSqlInterface.h"
#include "SQLamarr/Transformer.h"

namespace SQLamarr
{
  /** Removes all data from DB without affecting the schema
   *
   * When `execute`d, `CleanEventStore` gets a list of tables, 
   * including temporary instances, from SQLite3 schema, then loops over the 
   * existing tables and deletes all the rows.
   * This won't change the DB schema, nor change the DB connection data,
   * but clean the database to process another batch
   */
  class CleanEventStore: public BaseSqlInterface, public Transformer
  {
    public:
      using BaseSqlInterface::BaseSqlInterface;

      /// Execute the algorithm, cleaning the database 
      void execute () override;
  };
}
